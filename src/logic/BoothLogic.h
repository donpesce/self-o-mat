//
// Created by clemens on 21.01.19.
//

#ifndef SELF_O_MAT_BOOTHLOGIC_H
#define SELF_O_MAT_BOOTHLOGIC_H


#include "../camera/ICamera.h"
#include "ImageProcessor.h"
#include "PrinterManager.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>

#include <boost/algorithm/hex.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

#include <sys/stat.h>
#include <tools/blocking_reader.h>
#include <tools/cobs.h>

#include "SelfomatController.h"
#include "ILogicController.h"

#include <tools/verbose.h>




using namespace selfomat::camera;
using namespace selfomat::ui;

namespace selfomat {
    namespace logic {


        enum PrinterState {
                    // In the idle state, the printer thread is waiting for data
                    // Since nothing else is going on, raw saving is allowed in the background
                    PRINTER_STATE_IDLE = 0,
                    // Waiting for data is the same as idle but with raw processing disabled
                    // (so that we prioritize the JPEG for preview and printing)
                    PRINTER_STATE_WAITING_FOR_DATA = 1,
                    // Waiting for the user to cancel the print or not
                    PRINTER_STATE_WAITING_FOR_USER_INPUT = 2,
                    // We have all we need
                    PRINTER_STATE_WORKING = 3
        };

        class BoothLogic : public ILogicController {
        public:
            explicit BoothLogic(ICamera *camera, IGui *gui, bool has_button, const string &button_port, bool has_flash,
                                string imageDir, bool force_image_dir_mountpoint, bool disable_watchdog, bool show_led_setup) : camera(camera), gui(gui),
                                                                        imageProcessor(gui),
                                                                        printerManager(gui),
                                                                        has_button(has_button),
                                                                        has_flash(has_flash),
                                                                        imageDir(imageDir),
                                                                        controllerBoardPrefix(button_port),
                                                                        selfomatController(),
                                                                        force_image_dir_mountpoint(force_image_dir_mountpoint),
                                                                        show_led_setup(show_led_setup){
                selfomatController.setLogic(this);
                this->triggered = false;
                this->disable_watchdog = disable_watchdog;

                readSettings();
            }


        private:
            static std::string TAG;
            SelfomatController selfomatController;
            bool show_led_setup;
            bool force_image_dir_mountpoint;

            int returnCode = 0;
            string imageDir;

            bool has_button, has_flash, disable_watchdog;
            // hint to how the controller port might be called
            string controllerBoardPrefix;

            // # BOOTH SETTINGS HERE
            int triggerCounter;
            bool storageEnabled;
            bool printerEnabled;
            bool templateEnabled;
            bool flashEnabled;
            bool showAgreement;


            PrinterManager printerManager;
            ImageProcessor imageProcessor;

            bool isLogicThreadRunning, isCameraThreadRunning, isPrinterThreadRunning;
            boost::mutex triggerMutex;
            bool triggered;

            // We have a second thread running which first tries to get the jpegImageMutex
            // as soon as it has the jpegImageMutex it prepares for printing.
            boost::mutex jpegImageMutex;
            // Then the thread waits for the cancelPrintMutex. As soon as it has the cancelPrintMutex it checks if it
            // really needs to print or not. if it wants to print it prints.
            // Finally it unlocks both mutex and exits
            boost::mutex cancelPrintMutex;
            bool printCanceled = false;

            ICamera *camera;
            IGui *gui;

            /* The image buffer for the result image */
            void *imageBuffer = nullptr;
            size_t imageBufferSize = 0;
            ImageInfo imageInfo{};


            void *latestJpegBuffer = nullptr;
            size_t latestJpegBufferSize = 0;
            std::string latestJpegFileName = "";

            boost::condition_variable printerStateCV;
            boost::mutex printerStateMutex;
            PrinterState printerState = PRINTER_STATE_IDLE;

            boost::thread logicThreadHandle;
            boost::thread cameraThreadHandle;
            boost::thread printThreadHandle;

            int filterChoice = 0;
            double filterGain = 1.0;
            bool wasSuccessfullyStopped = false;

            void readSettings();
            void writeSettings();

            void cameraThread();

            void logicThread();




            void printerThread();

            void triggerFlash();

            int getFreeStorageSpaceMB();

            bool saveImage(void *data, size_t size, std::string filename, bool showAlert);
            bool saveImage(void *data, size_t size, std::string filename);

            bool isMountpoint(std::string folder);

            FILTER getFilter();

            timespec triggerStart;
        public:
            bool isStopped();
            void trigger();

            void acceptAgreement();
            void cancelPrint();
            bool start();

            void stop(bool update_mode);

            void stop() override;

            void setFlashEnabled(bool enabled, bool persist = false);
            bool getFlashEnabled();
            bool getFlashAvailable();

            int join() {
                if (logicThreadHandle.joinable()) {
                    logicThreadHandle.join();
                }
                if (cameraThreadHandle.joinable()) {
                    cameraThreadHandle.join();
                }
                if (printThreadHandle.joinable()) {
                    printThreadHandle.join();
                }

                if (returnCode == -1) {
                    reboot(LINUX_REBOOT_CMD_POWER_OFF);
                }

                return returnCode;
            }

            void stopForUpdate();

            virtual ~BoothLogic();

            void incTriggerCounter();
            int getTriggerCounter();

            void setPrinterEnabled(bool printerEnabled, bool persist = false);
            bool getPrinterEnabled();

            void setStorageEnabled(bool storageEnabled, bool persist = false);
            bool getStorageEnabled();

            void setTemplateEnabled(bool templateEnabled, bool persist = false);
            bool getTemplateEnabled();
            bool getTemplateLoaded();

            SelfomatController* getSelfomatController();

            void adjustFocus();

            bool updateTemplate(void *data, size_t size);

            const std::vector<std::string> * getFilterChoices();
            int getFilterChoice();
            void setFilterChoice(int choice, bool persist = false);

            double getFilterGain();
            void setFilterGain(double gain, bool persist = false);

            bool getDebugLogEnabled();
            void setDebugLogEnabled(bool newValue, bool persist = false);

        };

    }
}
#endif //SELF_O_MAT_BOOTHLOGIC_H
