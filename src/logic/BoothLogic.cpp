//
// Created by clemens on 21.01.19.
//
#include "BoothLogic.h"

using namespace std;
using namespace selfomat::logic;
using namespace selfomat::camera;
using namespace selfomat::ui;

std::string BoothLogic::TAG = "BoothLogic";

bool BoothLogic::start() {


    wasSuccessfullyStopped = false;
    LOG_D(TAG, "Starting Logic");

    if (has_button) {
        LOG_D(TAG, "Seraching for connected Controller");
        selfomatController.autoconnect(controllerBoardPrefix);

        if (!show_led_setup) {
            // Set the default LEDs.
            selfomatController.setLedType(SelfomatController::LED_TYPE::RGB.controllerValue);
            selfomatController.setLedCount(16);
            selfomatController.commit();
        }
        selfomatController.setWatchdogEnabled(!disable_watchdog);
    }

    if (showAgreement) {
        gui->showAgreement();
        selfomatController.showAgreement();
    }

    LOG_D(TAG, "Initializing Image Processor");
    if (!imageProcessor.start())
        return false;

    LOG_D(TAG, "Starting Printer");
    if (!printerManager.start())
        return false;

    // Start the threads
    isLogicThreadRunning = isCameraThreadRunning = isPrinterThreadRunning = true;
    logicThreadHandle = boost::thread(boost::bind(&BoothLogic::logicThread, this));
    cameraThreadHandle = boost::thread(boost::bind(&BoothLogic::cameraThread, this));
    printThreadHandle = boost::thread(boost::bind(&BoothLogic::printerThread, this));

    return true;
}

void BoothLogic::triggerFlash() {
    if (!flashEnabled)
        return;
    selfomatController.triggerFlash();
}

void BoothLogic::stop(bool update_mode) {
    if (wasSuccessfullyStopped)
        return;


    wasSuccessfullyStopped = true;

    LOG_I(TAG, "stopping logic. Update mode was: ", std::to_string(update_mode));

    isLogicThreadRunning = false;

    if (logicThreadHandle.joinable()) {
        LOG_D(TAG, "waiting for logic");
        logicThreadHandle.join();
    }

    if (update_mode) {
        selfomatController.enterUpdateMode();
        returnCode = 0x42;
    } else {
        returnCode = -1;
    }

    writeSettings();

    selfomatController.stopBlocking();

    isCameraThreadRunning = false;
    if (cameraThreadHandle.joinable()) {
        LOG_D(TAG, "waiting for cam");
        cameraThreadHandle.join();
    }
    isPrinterThreadRunning = false;
    if (printThreadHandle.joinable()) {
        LOG_D(TAG, "waiting for print");
        printThreadHandle.join();
    }

    if (gui != nullptr) {
        gui->stop();
        gui = nullptr;
    }

    imageProcessor.stop();
}

void BoothLogic::cameraThread() {
    LOG_D(TAG, "Starting Camera Thread");
    gui->initialized();
    while (isCameraThreadRunning) {
        if (camera->getState() != CameraState::STATE_WORKING) {
            // Camera is not working, try to get it working
            LOG_D(TAG, "Starting Camera");
            CameraStartResult result = camera->start();

            switch (result) {
                case START_RESULT_SUCCESS:
                    LOG_D(TAG, "...camera started successfully!");
                    continue;
                case START_RESULT_ERROR:
                    LOG_D(TAG, "Fatal error starting camera.");
                    break;
                case START_RESULT_NOT_FOUND:
                    LOG_D(TAG, "No camera found. Retrying");
                    break;
            }
            // delay searching to avoid spamming the bus
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        } else {
            // Camera is working, use it
            bool capture = triggered;
            if (capture) {
                triggerMutex.lock();
                triggered = false;
                triggerMutex.unlock();
            }

            if (capture) {
                // Do the capture
                LOG_D(TAG, "Doing the capture");
                gui->hidePreviewImage();

                // We need to wait for the printer thread (not really hopefully)
                {
                    LOG_D(TAG, "[Camera Thread] Waiting for printer thread to finish");
                    boost::unique_lock<boost::mutex> lk(printerStateMutex);
                    while (printerState != PRINTER_STATE_IDLE) {
                        printerStateCV.wait(lk);
                    }
                }


                // Update printer thread state to one
                {
                    boost::unique_lock<boost::mutex> lk(printerStateMutex);
                    printerState = PRINTER_STATE_WAITING_FOR_DATA;
                    printerStateCV.notify_all();
                }

                cancelPrintMutex.lock();
                printCanceled = false;
                cancelPrintMutex.unlock();

                // Get the mutex for the last jpeg image. We should have no problem doing this
                jpegImageMutex.lock();

                timespec tend;
                clock_gettime(CLOCK_MONOTONIC, &tend);
                LOG_D(TAG, "Time before trigger:", std::to_string(((double) tend.tv_sec + 1.0e-9 * tend.tv_nsec) -
                                                                  ((double) triggerStart.tv_sec +
                                                                   1.0e-9 * triggerStart.tv_nsec)) + " sec");

                clock_gettime(CLOCK_MONOTONIC, &triggerStart);

                camera->triggerCaptureBlocking();
                clock_gettime(CLOCK_MONOTONIC, &tend);
                LOG_D(TAG, "Time needed to trigger:", std::to_string(((double) tend.tv_sec + 1.0e-9 * tend.tv_nsec) -
                                                                     ((double) triggerStart.tv_sec +
                                                                      1.0e-9 * triggerStart.tv_nsec)) + " sec");
                LOG_D(TAG, "Successfully triggered");
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                auto success = camera->readImageBlocking(&latestJpegBuffer, &latestJpegBufferSize, &latestJpegFileName,
                                                         &imageBuffer, &imageBufferSize, &imageInfo);
                jpegImageMutex.unlock();

                {
                    boost::unique_lock<boost::mutex> lk(printerStateMutex);
                    printerState = PRINTER_STATE_WAITING_FOR_USER_INPUT;
                    printerStateCV.notify_all();
                }

                if (success) {
                    gui->updatePreviewImage(imageBuffer, imageInfo.width, imageInfo.height);
                    gui->notifyFinalImageSent();
                    selfomatController.showPrinting();

                    // 4500ms from here for the user to decide
                    boost::this_thread::sleep(boost::posix_time::milliseconds(4500));

                    // Notify the printer thread
                    {
                        boost::unique_lock<boost::mutex> lk(printerStateMutex);
                        printerState = PRINTER_STATE_WORKING;
                        printerStateCV.notify_all();
                    }

                    if (printerEnabled && !printCanceled && printerManager.getCurrentPrinterState() != STATE_STOPPED) {
                        gui->addAlert(ALERT_PRINTER_HINT, L"Foto wird gedruckt...", true, true);
                    }
                } else {
                    LOG_E(TAG, "Got an error");
                }

                selfomatController.sendPictureTaken();

                gui->notifyPreviewIncoming();
            } else {
                // Get a preview
                auto success = camera->capturePreviewBlocking(&imageBuffer, &imageBufferSize, &imageInfo);
                if (success) {
                    gui->updatePreviewImage(imageBuffer, imageInfo.width, imageInfo.height);
                } else {
                    LOG_E(TAG, "Error capturing preview. Restarting camera");
                    camera->stop();
                }
            }
        }
    }

    if (camera != nullptr) {
        camera->stop();
        camera = NULL;
    }
}


void BoothLogic::logicThread() {
    LOG_D(TAG, "Starting Logic Thread");

    while (isLogicThreadRunning) {
        // Send the heartbeat
        selfomatController.sendHeartbeat();

        //flash_serial_port.write_some(asio::buffer("i", 1));
        boost::this_thread::sleep(boost::posix_time::seconds(1));


        // check for an image and save it. but only if printer thread is idle
        // THIS IS ONLY FOR RAW FILES!!!
        bool allowSave = false;
        {
            boost::unique_lock<boost::mutex> lk(printerStateMutex);
            allowSave = (0 == printerState);
        }
        if (allowSave) {
            void *rawBuffer = nullptr;
            size_t rawSize = 0;
            std::string rawFilename;
            if (camera->getLastRawImage(&rawBuffer, &rawSize, &rawFilename)) {
                saveImage(rawBuffer, rawSize, rawFilename, true);
                free(rawBuffer);
            }
        }

        // check the printer state
        printerManager.refreshPrinterState();
        if (printerManager.getCurrentPrinterState() == STATE_STOPPED && printerEnabled) {
            gui->addAlert(ALERT_PRINTER, L"Drucker wurde gestoppt");
        } else {
            gui->removeAlert(ALERT_PRINTER);
        }


        // check the camera state
        if (camera->getState() != STATE_WORKING) {
            gui->addAlert(ALERT_CAMERA, L"Prüfe deine Kamera");
        } else {
            gui->removeAlert(ALERT_CAMERA);
        }

        int freeStorage = getFreeStorageSpaceMB();
        if (freeStorage < 500) {
            if (freeStorage == -1)
                gui->addAlert(ALERT_STORAGE, L"Kein Speichermedium gefunden");
            else
                gui->addAlert(ALERT_STORAGE, L"Geringe Speicherkapazität: " + to_wstring(freeStorage) + L"MB");
        } else {
            gui->removeAlert(ALERT_STORAGE);
        }

    }

    // Sync to disk
    LOG_I(TAG, "Syncing changes to disk");
    boost::thread syncThreadHandle(sync);
    while (!syncThreadHandle.try_join_for(boost::chrono::milliseconds(1000))) {
        LOG_D(TAG, "Still syncing...");
        selfomatController.sendHeartbeat();
    }
    LOG_I(TAG, "Syncing done");
}

BoothLogic::~BoothLogic() {
    free(imageBuffer);
}

void BoothLogic::trigger() {
    if (showAgreement)
        return;

    clock_gettime(CLOCK_MONOTONIC, &triggerStart);

    triggerMutex.lock();
    triggered = true;
    triggerMutex.unlock();
    incTriggerCounter();
}

void BoothLogic::cancelPrint() {
    cancelPrintMutex.lock();
    printCanceled = true;
    gui->cancelPrint();
    cancelPrintMutex.unlock();
}

void BoothLogic::printerThread() {
    while (isPrinterThreadRunning) {
        bool do_print;
        {
            LOG_D(TAG, "[Printer Thread] Waiting for an image to process");
            boost::unique_lock<boost::mutex> lk(printerStateMutex);
            while (printerState == PRINTER_STATE_IDLE || printerState == PRINTER_STATE_WAITING_FOR_DATA) {
                printerStateCV.timed_wait(lk, boost::posix_time::milliseconds(500));
                // application should close
                if (!isPrinterThreadRunning)
                    return;
            }
            do_print = printerEnabled;
        }

        LOG_D(TAG, "[Printer Thread] Processing image. Printing enabled: ", std::to_string(do_print));;

        if (do_print) {
            // We need the final jpeg image. So lock the mutex
            {
                boost::unique_lock<boost::mutex> lk(jpegImageMutex);

                // first we save the image
                saveImage(latestJpegBuffer, latestJpegBufferSize, latestJpegFileName, true);

                cv::Mat toPrepare;
                if (templateEnabled) {
                    toPrepare = imageProcessor.frameImageForPrint(latestJpegBuffer, latestJpegBufferSize, getFilter(),
                                                                  filterGain);
                } else {
                    toPrepare = imageProcessor.decodeImageForPrint(latestJpegBuffer, latestJpegBufferSize, getFilter(),
                                                                   filterGain);
                }

                printerManager.prepareImageForPrint(toPrepare);
                LOG_D(TAG, "[Printer Thread] Prepared");
            }

            {
                LOG_D(TAG, "[Printer Thread] Waiting for user to decide if he wants to print");
                boost::unique_lock<boost::mutex> lk(printerStateMutex);
                while (printerState == PRINTER_STATE_WAITING_FOR_USER_INPUT) {
                    printerStateCV.wait(lk);
                }
            }


            // We need the info if the user wants to print or not
            {
                boost::unique_lock<boost::mutex> lk(cancelPrintMutex);
                if (!printCanceled) {
                    LOG_D(TAG, "[Printer Thread] Printing");
                    printerManager.printImage();
                } else {
                    LOG_D(TAG, "[Printer Thread] Print canceled!");
                    printerManager.cancelPrint();
                }
            }
        } else {
            // We need the final jpeg image. So lock the mutex
            {
                boost::unique_lock<boost::mutex> lk(jpegImageMutex);

                // we only need to save the image
                saveImage(latestJpegBuffer, latestJpegBufferSize, latestJpegFileName, true);
            }

            // wait for logic thread
            {
                boost::unique_lock<boost::mutex> lk(printerStateMutex);
                while (printerState == PRINTER_STATE_WAITING_FOR_USER_INPUT) {
                    printerStateCV.wait(lk);
                }
            }
        }

        {
            boost::unique_lock<boost::mutex> lk(printerStateMutex);
            printerState = PRINTER_STATE_IDLE;
            printerStateCV.notify_all();
        }
    }
}

int BoothLogic::getFreeStorageSpaceMB() {
    if (imageDir.empty()) {
        LOG_E(TAG, "No image dir specified!");
        return -1;
    }

    try {
        boost::filesystem::space_info s = boost::filesystem::space(imageDir);

        return static_cast<int>(s.free / 1024 / 1024);
    } catch (exception &e) {
        return -1;
    }
}

bool BoothLogic::isMountpoint(std::string folder) {
    /* get the parent directory  of the file */

    std::string folder_cpy(folder.c_str());
    std::string parent_name = dirname((char *) folder_cpy.c_str());

    /* get the file's stat info */
    struct stat file_stat{};
    if (-1 == stat(folder.c_str(), &file_stat)) {
        LOG_E(TAG, "stat error!");
        return false;
    }

    /* determine whether the supplied file is a directory
      if it isn't, then it can't be a mountpoint. */
    if (!(file_stat.st_mode & S_IFDIR)) {
        LOG_E(TAG, "image dir is not a directory");
        return false;
    }

    /* get the parent's stat info */
    struct stat parent_stat{};
    if (-1 == stat(parent_name.c_str(), &parent_stat)) {
        LOG_E(TAG, "parent stat failed");
        return false;
    }

    // it's a mount point if devices differ
    return file_stat.st_dev != parent_stat.st_dev;
}

bool BoothLogic::saveImage(void *data, size_t size, std::string filename, bool showAlert) {
    auto success = saveImage(data, size, filename);
    if (!success && showAlert) {
        gui->addAlert(ALERT_STORAGE_ERROR, L"Fehler beim Speichern des Fotos", true);
    }

    return success;
}

bool BoothLogic::saveImage(void *data, size_t size, std::string filename) {
    if (!storageEnabled) {
        return true;
    }

    if (imageDir.empty()) {
        LOG_E(TAG, "No image dir specified");
        return false;
    }

    if (force_image_dir_mountpoint && !isMountpoint(imageDir)) {
        LOG_E(TAG, "imageDir not a mountpoint");
        return false;
    }

    // Check if imageDir is a mountpoint

    std::time_t time = std::time(nullptr);

    std::string fullImagePath = imageDir;

    fullImagePath += "/";
    fullImagePath += to_string((long) time);
    fullImagePath += "_";
    fullImagePath += filename;


    LOG_I(TAG, "Writing image to:", fullImagePath);


    FILE *fp;

    fp = fopen(fullImagePath.c_str(), "wb");
    if (fp == nullptr) {
        LOG_E(TAG, "Error opening output file");
        return false;
    }

    fwrite(data, size, 1, fp);

    fclose(fp);

    LOG_D(TAG, "File written to: ",  fullImagePath);

    return true;
}

void BoothLogic::stopForUpdate() {
    stop(true);
}

void BoothLogic::setStorageEnabled(bool storageEnabled, bool persist) {
    this->storageEnabled = storageEnabled;
    if (persist) {
        writeSettings();
    }
}

bool BoothLogic::getStorageEnabled() {
    return storageEnabled;
}

void BoothLogic::setPrinterEnabled(bool printerEnabled, bool persist) {
    this->printerEnabled = printerEnabled;
    gui->setPrinterEnabled(printerEnabled);
    if (persist) {
        writeSettings();
    }
}

bool BoothLogic::getPrinterEnabled() {
    return printerEnabled;
}

int BoothLogic::getTriggerCounter() {
    return triggerCounter;
}

void BoothLogic::incTriggerCounter() {
    this->triggerCounter++;
}

void BoothLogic::readSettings() {
    LOG_D(TAG, "Reading settings from file");
    boost::property_tree::ptree ptree;

    bool success = true;
    try {
        boost::property_tree::read_json(std::string(getenv("HOME")) + "/.selfomat_settings.json", ptree);
    } catch (boost::exception &e) {
        LOG_E(TAG, "Error loading settings settings. Writing defaults. Error was: ", boost::diagnostic_information(e));
        success = false;
    }

    this->triggerCounter = (ptree.get<int>("trigger_counter", 0));
    setStorageEnabled(ptree.get<bool>("storage_enabled", true));
    setPrinterEnabled(ptree.get<bool>("printer_enabled", true));
    setTemplateEnabled(ptree.get<bool>("template_enabled", false));
    setFlashEnabled(ptree.get<bool>("flash_enabled", false));
    this->showAgreement = ptree.get<bool>("show_agreement", true);
    setFilterChoice(ptree.get<int>("filter_choice", BASIC_FILTER));
    setFilterGain(ptree.get<double>("filter_gain", 1.0));
    setDebugLogEnabled(ptree.get<bool>("debug_log_enabled", false));
    if (!success)
        writeSettings();
}

void BoothLogic::writeSettings() {
    boost::property_tree::ptree ptree;
    ptree.put("show_agreement", showAgreement);
    ptree.put("trigger_counter", triggerCounter);
    ptree.put("storage_enabled", storageEnabled);
    ptree.put("printer_enabled", printerEnabled);
    ptree.put("template_enabled", templateEnabled);
    ptree.put("flash_enabled", this->flashEnabled);
    ptree.put("filter_gain", filterGain);
    ptree.put("filter_choice", filterChoice);
    ptree.put("debug_log_enabled", getDebugLogEnabled());
    try {
        boost::property_tree::write_json(std::string(getenv("HOME")) + "/.selfomat_settings.json", ptree);
    } catch (boost::exception &e) {
        LOG_E(TAG, "Error writing settings. Error was: ", boost::diagnostic_information(e));
    }
}

void BoothLogic::setFlashEnabled(bool enabled, bool persist) {
    this->flashEnabled = enabled;

    if (persist) {
        writeSettings();
    }
}

bool BoothLogic::getFlashEnabled() {
    return flashEnabled;
}

bool BoothLogic::getFlashAvailable() {
    return has_flash;
}

void BoothLogic::setTemplateEnabled(bool templateEnabled, bool persist) {
    this->templateEnabled = templateEnabled;
    gui->setTemplateEnabled(templateEnabled);
    if (persist)
        writeSettings();
}

bool BoothLogic::getTemplateEnabled() {
    return templateEnabled;
}

bool BoothLogic::getTemplateLoaded() {
    return imageProcessor.isTemplateLoaded();
}


void BoothLogic::adjustFocus() {
    camera->autofocusBlocking();
    gui->addAlert(ALERT_CAMERA_HINT, L"Fokus wird gesucht", true, true);
}

SelfomatController *BoothLogic::getSelfomatController() {
    return &selfomatController;
}

void BoothLogic::acceptAgreement() {
    gui->hideAgreement();
    showAgreement = false;
    writeSettings();
}

void BoothLogic::stop() {
    stop(false);
}

bool BoothLogic::updateTemplate(void *data, size_t size) {
    bool result = imageProcessor.updateTemplate(data, size);
    gui->reloadTemplate();
    if (result) {
        gui->addAlert(ALERT_TEMPLATE, L"Template wurde gespeichert", true, true);
    }
    return result;
}

const std::vector<std::string> *BoothLogic::getFilterChoices() {
    return imageProcessor.getFilterNames();
}

int BoothLogic::getFilterChoice() {
    return filterChoice;
}

void BoothLogic::setFilterChoice(int choice, bool persist) {
    filterChoice = choice;
    if (persist) {
        writeSettings();
    }
}

double BoothLogic::getFilterGain() {
    return filterGain;
}

void BoothLogic::setFilterGain(double gain, bool persist) {
    filterGain = gain;
    if (persist)
        writeSettings();
}

FILTER BoothLogic::getFilter() {
    if (filterChoice < 0 || filterChoice >= imageProcessor.getFilterNames()->size())
        return NO_FILTER;

    switch (filterChoice) {
        case 0:
            return NO_FILTER;
        case 1:
            return BASIC_FILTER;
    }
}

bool BoothLogic::isStopped() {
    return wasSuccessfullyStopped;
}

void BoothLogic::setDebugLogEnabled(bool newValue, bool persist) {
    if (newValue) {
        if (imageDir.empty()) {
            LOG_E(TAG, "No image dir specified. We cannot enable debug logging");
            return;
        }

        if (force_image_dir_mountpoint && !isMountpoint(imageDir)) {
            LOG_E(TAG, "imageDir not a mountpoint. cannot enable debug logging");
            return;
        }

        FilesystemLogger::INSTANCE.enableLogToFile(imageDir);
    } else {
        FilesystemLogger::INSTANCE.disableLogToFile();
    }

    gui->setDebugOutput(FilesystemLogger::INSTANCE.getLogToFileState() == FilesystemLogger::ENABLED);

    if (persist)
        writeSettings();
}

bool BoothLogic::getDebugLogEnabled() {
    return FilesystemLogger::INSTANCE.getLogToFileState() == FilesystemLogger::ENABLED;
}
