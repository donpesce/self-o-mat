//
// Created by clemens on 21.01.19.
//

#include "GphotoCamera.h"

const std::string GphotoCamera::TAG = "GPHOTO CAMERA";

#define CHECK(it) (it == GP_OK)

CameraStartResult GphotoCamera::start() {

    lastDrainTime = boost::posix_time::min_date_time;
    focusStartedTime = boost::posix_time::min_date_time;


    const char **gphotoVersion = gp_library_version(GP_VERSION_VERBOSE);
    LOG_D(TAG, "GPhoto Version: " << (*gphotoVersion));

    gp = gp_context_new();

    CameraList *list;
    gp_list_new(&list);
    gp_camera_autodetect(list, gp);

    // Print the cameras
    int count = gp_list_count(list);
    cout << "Found " << count << " cameras." << endl;
    gp_list_free(list);

    gp_camera_new(&camera);


    int retval = gp_camera_init(camera, gp);
    if (retval != GP_OK) {
        cerr << "Error opening camera, error" << gp_result_as_string(retval) << endl;
        setState(STATE_ERROR);
        return START_RESULT_ERROR;
    }

    cout << "Successfully opened camera" << endl;

    gp_camera_get_config(camera, &rootWidget, gp);


    // We have the root widget, find a controller which works with the connected camera
    if(!findCameraController()) {
        LOG_E(TAG, "Could not find a controller for the connected camera.");
        return START_RESULT_ERROR;
    }



    // HACK set it to center
    //current_exposure_correction_choice = exposurecompensationChoices.size() / 2;

    setState(STATE_WORKING);

    /*if (!CHECK(gp_widget_get_child_by_name(rootWidget, "exposurecompensation", &exposureCorrectionWidget))) {
        cerr << "error getting exposure correction widget" << endl;
        return START_RESULT_ERROR;
    }*/

    drainEventQueue(false);


    // Get shutter speed, iso and apterture


    //listWidgets(0, rootWidget);

    camera_name = getCameraPropertyString("cameramodel");
    lens_name = getCameraPropertyString("lensname");
    cout << "We're running with a " << camera_name << " with " << lens_name << " attached. yeah!" << endl;

//    setCameraProperty("shutterspeed", "1/4000");


//    drainEventQueue(true);

//    stop();


    return START_RESULT_SUCCESS;
}

bool GphotoCamera::setCameraProperty(string property_name, string value) {
    cout << "Settign widget " << endl;
    CameraWidget *widget;
    int retval = gp_widget_get_child_by_name(rootWidget, property_name.c_str(), &widget);
    if (retval != GP_OK) {
        cout << "error setting property1" << endl;
        return false;
    }
    retval = gp_widget_set_value(widget, value.c_str());
    if (retval != GP_OK) {
        cout << "error setting property2" << endl;
        return false;
    }
    retval = gp_camera_set_config(camera, rootWidget, gp);
    if (retval != GP_OK) {
        cout << "error setting property3" << endl;
        return false;
    }
    return false;
}

void GphotoCamera::listWidgets(int currentDepth, CameraWidget *parent) {
    const char *name;
    gp_widget_get_name(parent, &name);

    CameraWidgetType type;
    gp_widget_get_type(parent, &type);
    string typestr("unknown");

    int choice_count;
    const char *tmp;

    bool get_value = false;
    switch (type) {
        case CameraWidgetType::GP_WIDGET_BUTTON:
            typestr = "Button";
            break;
        case CameraWidgetType::GP_WIDGET_DATE:
            typestr = "Date";
            break;
        case CameraWidgetType::GP_WIDGET_MENU:
            get_value = true;
            typestr = "Menu";
            break;
        case CameraWidgetType::GP_WIDGET_RADIO:
            get_value = true;
            typestr = "Radio";


            choice_count = gp_widget_count_choices(parent);
            for (int choice = 0; choice < choice_count; choice++) {
                gp_widget_get_choice(parent, choice, &tmp);
                cout << "Choice: " << tmp << endl;
            }

            break;
        case CameraWidgetType::GP_WIDGET_RANGE:
            typestr = "Range";
            break;
        case CameraWidgetType::GP_WIDGET_TEXT:
            get_value = true;
            typestr = "Text";
            break;
        case CameraWidgetType::GP_WIDGET_WINDOW:
            typestr = "Window";
            break;
        case CameraWidgetType::GP_WIDGET_SECTION:
            typestr = "Section";
            break;
        case CameraWidgetType::GP_WIDGET_TOGGLE:
            typestr = "Toggle";
            break;
    }

    const char *value = nullptr;
    if (get_value) {
        gp_widget_get_value(parent, &value);
    }

    string value_string;
    if (value != nullptr)
        value_string = value;

    for (int i = 0; i < currentDepth; i++)
        cout << "-";

    cout << " ";

    cout << "Found Widget: " << name << " with type: " << typestr << " and value: " << value_string << endl;


    int child_count = gp_widget_count_children(parent);
    CameraWidget *child = nullptr;
    for (int i = 0; i < child_count; i++) {
        gp_widget_get_child(parent, i, &child);
        listWidgets(currentDepth + 1, child);
    }
}


void GphotoCamera::stop() {
    cout << "Stopping camera!" << endl;
    // Forbid new capture or preview events
    setState(STATE_STOPPED);

    // drain the queue
    cameraIoMutex.lock();
    drainEventQueue(false);
    cameraIoMutex.unlock();

    // free the widgets
    gp_widget_free(rootWidget);

    gp_camera_exit(camera, gp);
    gp_camera_free(camera);


    gp_context_unref(gp);

    // Free the buffers
    if (latestBuffer != nullptr) {
        free(latestBuffer);
        latestBuffer = nullptr;
    }
    if (tmpBuffer != nullptr) {
        free(tmpBuffer);
        tmpBuffer = nullptr;
    }
}

void GphotoCamera::drainEventQueue(bool waitForPhoto) {
    lastDrainTime = boost::posix_time::microsec_clock::local_time();
    CameraEventType evtype;
    void *data = nullptr;
    int retval;

    while (true) {
        retval = gp_camera_wait_for_event(camera, 10, &evtype, &data, gp);
        if (retval != GP_OK) {
            if (data != nullptr) {
                free(data);
            }
            cerr << "return from waitevent in trigger sample with " << gp_result_as_string(retval) << endl;
            return;
        }
        switch (evtype) {
            case GP_EVENT_TIMEOUT:
                free(data);
                // We're done if we don't need a photo
                if (!waitForPhoto)
                    return;
                break;
            case GP_EVENT_FILE_ADDED: {
                cout << "Event FILE ADDED" << endl;
                CameraFile *cameraFile = nullptr;
                gp_file_new(&cameraFile);

                auto *path = static_cast<CameraFilePath *>(data);
                retval = gp_camera_file_get(camera, path->folder, path->name, CameraFileType::GP_FILE_TYPE_NORMAL,
                                            cameraFile, gp);
                if (retval != GP_OK) {
                    cerr << "Error obtaining camera file handle: " << gp_result_as_string(retval) << endl;
                    free(data);
                    gp_file_free(cameraFile);
                    return;
                }

                if (boost::ends_with(path->name, ".jpg")) {
                    cout << "We got the JPG" << endl;

                    // release the trigger if still pressed
                    cameraController->releaseTrigger();

                    const char *imageData = nullptr;
                    unsigned long int imageDataSize;
                    // Get the data. This is jpeg
                    retval = gp_file_get_data_and_size(cameraFile, &imageData, &imageDataSize);

                    if (retval != GP_OK) {
                        free(data);
                        gp_file_free(cameraFile);
                        cerr << "Error fetching image data: " << gp_result_as_string(retval) << endl;
                        return;
                    }

                    // Copy the data into our latest buffer
                    buffers::requireBufferWithSize(&latestBuffer, &latestBufferSize, imageDataSize);
                    memcpy(latestBuffer, imageData, imageDataSize);

                } else {
                    cout << "Some other file" << endl;
                }

                gp_file_free(cameraFile);

                // delete the file from the camera
                retval = gp_camera_file_delete(camera, path->folder, path->name, gp);

                free(data);

                if (waitForPhoto)
                    return;
            }
                break;
            default:
                // Free the data and do nothing
                free(data);
                break;
        }
    }
}

void GphotoCamera::drainEventQueueWhenNeeded() {
    auto now = boost::posix_time::microsec_clock::local_time();
    auto timeSinceLastDrain = (now - lastDrainTime).total_milliseconds();

    if (timeSinceLastDrain > 1000) {
        //cout << "Event queue not drained since " << timeSinceLastDrain << " millis" << endl;
        drainEventQueue(false);
    }
}

bool GphotoCamera::capturePreviewBlocking(void **buffer, size_t *bufferSize, ImageInfo *resultInfo) {
    if (getState() != STATE_WORKING)
        return false;
    cameraIoMutex.lock();
    CameraFile *cameraFile = NULL;

    gp_file_new(&cameraFile);
    int retval = gp_camera_capture_preview(camera, cameraFile, gp);

    if (retval != GP_OK) {
        cerr << "Error capturing preview:" << gp_result_as_string(retval) << endl;
        gp_file_free(cameraFile);
        drainEventQueueWhenNeeded();
        cameraIoMutex.unlock();
        return false;
    }

    const char *data;
    unsigned long int size;
    // Get the data. This is jpeg
    retval = gp_file_get_data_and_size(cameraFile, &data, &size);

    if (retval != GP_OK) {
        cerr << "Error fetching image data: " << gp_result_as_string(retval) << endl;
        cameraIoMutex.unlock();
        return false;
    }

    // It is already a preview image, do not scale.
    auto success = jpegDecoder.decodeJpeg((unsigned char *) data, size, &tmpBuffer, &tmpBufferSize, resultInfo, RGBA);

    // swap the buffers
    void *tmp = *buffer;
    size_t sizeTmp = *bufferSize;
    *buffer = tmpBuffer;
    *bufferSize = tmpBufferSize;
    tmpBuffer = tmp;
    tmpBufferSize = sizeTmp;

    gp_file_free(cameraFile);

    drainEventQueueWhenNeeded();

    if (settings_dirty) {
        if (!pushCameraSettings()) {

            pullCameraSettings();
        }
        pullCameraSettings();
    }

    if (trigger_focus) {
        cameraController->focus();
        trigger_focus = false;
        focusStartedTime = boost::posix_time::microsec_clock::local_time();
        focus_active = true;
    }
    if (focus_active) {
        auto now = boost::posix_time::microsec_clock::local_time();
        auto focusActiveTime = (now - focusStartedTime).total_milliseconds();
        if (focusActiveTime > 5000) {
            cameraController->stopFocus();
            focus_active = false;
        }
    }

    cameraIoMutex.unlock();

    return success;
}

bool GphotoCamera::pushCameraSettings() {
    return true;
}

bool GphotoCamera::triggerCaptureBlocking() {
    if (getState() != STATE_WORKING)
        return false;
    cameraIoMutex.lock();


    bool success = cameraController->trigger();

    cameraIoMutex.unlock();

    return success;
}


int GphotoCamera::getIso() {
    return 0;
}

int GphotoCamera::getShutterSpeed() {
    return 0;
}

int GphotoCamera::getAperture() {
    return 0;
}

int GphotoCamera::getShootingMode() {
    return -1;
}

bool GphotoCamera::setIso(int iso_choice) {
    return true;
}

bool GphotoCamera::setShutterSpeed(int shutter_speed_choice) {
    return false;
}

bool GphotoCamera::setAperture(int aperture_choice) {
    return false;
}

void GphotoCamera::pullCameraSettings() {

    settings_dirty = false;
}

bool GphotoCamera::loadChoices(string property_name, std::vector<string> &choices) {
    cout << "Querying choices for property: " << property_name << endl;
    CameraWidget *widget;
    if (!CHECK(gp_widget_get_child_by_name(rootWidget, property_name.c_str(), &widget))) {
        cerr << "Error getting choices for property " << property_name << endl;
        return false;
    }

    int choice_count = gp_widget_count_choices(widget);

    for (int i = 0; i < choice_count; i++) {
        const char *choice;
        if (CHECK(gp_widget_get_choice(widget, i, &choice))) {
            std::string choice_string;
            // Use assign to copy the string not just the ptr
            choice_string = choice;
            //cout << "Found choice: " << choice_string << endl;
            choices.push_back(choice_string);
        } else {
            return false;
        }
    }

    return true;
}

bool GphotoCamera::setCameraPropertyChoice(string property_name, int choice) {


    return true;
}

int GphotoCamera::getCameraPropertyChoice(string property_name) {


    return 0;
}

string GphotoCamera::getCameraPropertyString(string property_name) {
    CameraWidget *widget;
    // Get the current choice
    const char *value;

    if (!CHECK(gp_widget_get_child_by_name(rootWidget, property_name.c_str(), &widget))) {
        cerr << "Error getting widget for property " << property_name << endl;
        return "N/A";
    }

    if (!CHECK(gp_widget_get_value(widget, &value))) {
        cerr << "Error getting current widget value" << endl;
        return "N/A";
    }

    string value_string = value;

    return value_string;
}

vector<string> *GphotoCamera::getIsoChoices() {
    return nullptr;
}

vector<string> *GphotoCamera::getShutterSpeedChoices() {
    return nullptr;
}

vector<string> *GphotoCamera::getApertureChoices() {
    return nullptr;
}

vector<string> *GphotoCamera::getShootingModeChoices() {
    return nullptr;
}

bool GphotoCamera::autofocusBlocking() {
    trigger_focus = true;
}

vector<string> *GphotoCamera::getExposureCorrectionModeChoices() {
    return nullptr;
}

vector<string> *GphotoCamera::getImageFormatChoices() {
    return nullptr;
}

vector<string> *GphotoCamera::getImageFormatSdChoices() {
    return nullptr;
}

string GphotoCamera::getCameraName() {
    return camera_name;
}

string GphotoCamera::getLensName() {
    return lens_name;
}

int GphotoCamera::getExposureCorrection() {
    return 0;
}

int GphotoCamera::getImageFormat() {
    return 0;
}

int GphotoCamera::getImageFormatSd() {
    return 0;
}

bool GphotoCamera::setExposureCorrection(int exposure_correction_choice) {
    return false;
}

bool GphotoCamera::setImageFormat(int image_format_choice) {
    return false;
}

bool GphotoCamera::setImageFormatSd(int image_format_sd_choice) {
    return false;
}


bool GphotoCamera::readImageBlocking(void **fullJpegBuffer, size_t *fullJpegBufferSize, void **previewBuffer,
                                     size_t *previewBufferSize, ImageInfo *previewImageInfo) {
    if (getState() != STATE_WORKING)
        return false;
    cameraIoMutex.lock();

    // Wait for the photo to arrive
    // TODO: maybe the camera is SO quick that we enter this stage after the photo was already processed. I doubt it tho
    drainEventQueue(true);

    // Jpeg is now in latestBuffer convert to bmp
    auto success = jpegDecoder.decodeJpeg((unsigned char *) latestBuffer, latestBufferSize, previewBuffer,
                           previewBufferSize,
                           previewImageInfo, JPEG_DECODE_COLORS::RGBA, 0, 800, LARGER_THAN_REQUIRED);

    LOG_D(TAG, "Preview image of the final shot has size: " << previewImageInfo->width << "x" << previewImageInfo->height);


    // Swap the buffers for jpeg
    void *tmpBuffer = *fullJpegBuffer;
    size_t tmpBufferSize = *fullJpegBufferSize;

    *fullJpegBuffer = latestBuffer;
    *fullJpegBufferSize = latestBufferSize;


    latestBuffer = tmpBuffer;
    latestBufferSize = tmpBufferSize;

    cameraIoMutex.unlock();
    return success;
}

bool GphotoCamera::findCameraController() {
    // Try the advanced controller first then revert to the basic one
    cameraController = new CanonShutterButtonCameraController(gp, camera, rootWidget);
    if(cameraController->supportsCamera())
        return true;

    // Delete it, try next
    delete(cameraController);

    cameraController = new BasicCameraController(gp, camera, rootWidget);

    if(cameraController->supportsCamera())
        return true;

    delete(cameraController);
    cameraController = nullptr;
    return false;
}

