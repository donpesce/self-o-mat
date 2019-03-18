//
// Created by clemens on 21.01.19.
//

#include "OpenCVCamera.h"


CameraStartResult OpenCVCamera::start() {

    if (!cap.open(0)) {
        cout << "CVCamera: Error opening camera" << endl;
        return START_RESULT_ERROR;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

    return START_RESULT_SUCCESS;
}

bool OpenCVCamera::capturePreviewBlocking(void **buffer, size_t *bufferSize, ImageInfo *resultInfo) {

    Mat image;
    cap >> image;


    auto bytesUsed = static_cast<size_t>(4 * image.cols * image.rows);


    if (!buffers::requireBufferWithSize(buffer, bufferSize, bytesUsed)) {
        cerr << "Error allocating buffer" << endl;
        return false;
    }

    cv::resize(image, image, cv::Size(1280,800));
    Mat target(image.rows, image.cols, CV_8UC4, *buffer);


    cv::cvtColor(image, target, CV_RGB2BGRA);

    resultInfo->width = target.cols;
    resultInfo->height = target.rows;

    return true;
}


void OpenCVCamera::stop() {
    cap.release();
}

bool OpenCVCamera::triggerCaptureBlocking() {
    return true;
}

bool OpenCVCamera::readImageBlocking(void **fullJpegBuffer, size_t *fullJpegBufferSize, std::string *fullJpegFilename, void **previewBuffer,
                                     size_t *previewBufferSize, ImageInfo *previewImageInfo) {

    Mat image;
    cap >> image;

    vector<uchar> vect;
    cv::imencode(".jpg", image, vect);

    buffers::requireBufferWithSize(fullJpegBuffer, fullJpegBufferSize, vect.size());
    memcpy(*fullJpegBuffer, vect.data(), vect.size());


    cv::Size previewSize(1920/2, 1080/2);
    auto size = previewSize.height * previewSize.width*4;
    buffers::requireBufferWithSize(previewBuffer, previewBufferSize, size);


    cv::Mat resized;
    cv::resize(image, resized, previewSize);

    cv::Mat previewImage(previewSize.height, previewSize.width, CV_8UC4, *previewBuffer);
    cvtColor(resized, previewImage, CV_RGB2BGRA);


    previewImageInfo->width = previewSize.width;
    previewImageInfo->height = previewSize.height;

    *fullJpegFilename = "webcam.jpg";
    return true;
}


int OpenCVCamera::getIso() {
    return 0;
}

int OpenCVCamera::getShutterSpeed() {
    return 0;
}

int OpenCVCamera::getAperture() {
    return 0;
}

int OpenCVCamera::getShootingMode() {
    return 0;
}

vector<string> *OpenCVCamera::getIsoChoices() {
    return nullptr;
}

vector<string> *OpenCVCamera::getShutterSpeedChoices() {
    return nullptr;
}

vector<string> *OpenCVCamera::getApertureChoices() {
    return nullptr;
}

vector<string> *OpenCVCamera::getShootingModeChoices() {
    return nullptr;
}

bool OpenCVCamera::setIso(int iso_choice) {
    return false;
}

bool OpenCVCamera::setShutterSpeed(int shutter_speed_choice) {
    return false;
}

bool OpenCVCamera::setAperture(int aperture_choice) {
    return false;
}

bool OpenCVCamera::autofocusBlocking() {
    return false;
}

vector<string> *OpenCVCamera::getExposureCorrectionModeChoices() {
    return nullptr;
}

vector<string> *OpenCVCamera::getImageFormatChoices() {
    return nullptr;
}

vector<string> *OpenCVCamera::getImageFormatSdChoices() {
    return nullptr;
}

string OpenCVCamera::getCameraName() {
    return std::__cxx11::string();
}

string OpenCVCamera::getLensName() {
    return std::__cxx11::string();
}

int OpenCVCamera::getExposureCorrection() {
    return 0;
}

int OpenCVCamera::getImageFormat() {
    return 0;
}

int OpenCVCamera::getImageFormatSd() {
    return 0;
}

bool OpenCVCamera::setExposureCorrection(int exposure_correction_choice) {
    return false;
}

bool OpenCVCamera::setImageFormat(int image_format_choice) {
    return false;
}

bool OpenCVCamera::setImageFormatSd(int image_format_sd_choice) {
    return false;
}

bool OpenCVCamera::getLastRawImage(void **targetBuffer, size_t *targetSize, std::string *filename) {
    return false;
}

