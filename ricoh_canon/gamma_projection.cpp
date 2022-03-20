#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <process.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <vector>
#include <string>
#include <time.h>
#include <windows.h>
//#include <xiApiPlusOcv.hpp> 
//#include <xiApiPlusOcv.cpp> 

#include "EDSDK.h"             // Checked at EDSDK 3.8
#include <opencv2/opencv.hpp>  // Checked at OpenCV 4.0.0-pre

#include <fstream>

//#include "xiApiPlusOcv.hpp" 
//#include "xiApiPlusOcv.cpp" 


using namespace cv;
using namespace std;
using namespace aruco;


#define NUMBER 2

string filename = "6_Gamma_2_Pro" + to_string(NUMBER) + "_";


#define WIDTH 1280
#define HEIGHT 800
#define CAMWIDTH 6960
#define CAMHEIGHT 3904

#define X_MIN 3305
#define Y_MIN 2209
#define X_MAX 4031
#define Y_MAX 2725

static bool flag = false;
static EdsBaseRef eventRef = NULL;

static EdsError EDSCALLBACK handler(EdsUInt32 inEvent, EdsBaseRef inRef,
    EdsVoid* inContext) {
    flag = true;
    if (inEvent == kEdsObjectEvent_DirItemRequestTransfer) {
        eventRef = inRef;
    }
    else {
        EdsRelease(inRef);
        eventRef = NULL;
    }
    return EDS_ERR_OK;
}


cv::Mat execute() {
    EdsError err;

    // -----------------------------------
    // ----- Get reference to camera -----
    // -----------------------------------
    EdsCameraListRef cameraList = NULL;
    err = EdsGetCameraList(&cameraList);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to obtain camera list" << std::endl;
        //return;
    }

    EdsUInt32 count = 0;
    err = EdsGetChildCount(cameraList, &count);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to obtain number of cameras" << std::endl;
        EdsTerminateSDK();
        //return;
    }
    if (count == 0) {
        std::cout << "Camera was not found" << std::endl;
        EdsTerminateSDK();
        //return;
    }

    EdsCameraRef camera = NULL;
    err = EdsGetChildAtIndex(cameraList, 0, &camera);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to get first camera" << std::endl;
        EdsTerminateSDK();
        //return;
    }

    EdsDeviceInfo deviceInfo;
    err = EdsGetDeviceInfo(camera, &deviceInfo);
    if (err != EDS_ERR_OK || camera == NULL) {
        std::cout << "Failed to get device info" << std::endl;
        EdsTerminateSDK();
        //return;
    }
    bool isLegacy = deviceInfo.deviceSubType == 0;

    EdsRelease(cameraList);

    // ------------------------
    // ----- Open session -----
    // ------------------------
    err = EdsOpenSession(camera);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to open session" << std::endl;
        //return;
    }

    // -------------------------
    // ----- Configuration -----
    // -------------------------
    EdsUInt32 quality = EdsImageQuality_LJN;  // Save as normal jpeg
    err = EdsSetPropertyData(camera, kEdsPropID_ImageQuality, 0, sizeof(quality),
        &quality);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        //return;
    }

    EdsUInt32 saveTo = kEdsSaveTo_Host;
    err =
        EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(saveTo), &saveTo);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        //return;
    }

    EdsCapacity capacity = { 0x7FFFFFFF, 0x1000, 1 };
    err = EdsSendStatusCommand(camera, kEdsCameraStatusCommand_UILock, 0);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        //return;
    }
    err = EdsSetCapacity(camera, capacity);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        //return;
    }
    err = EdsSendStatusCommand(camera, kEdsCameraStatusCommand_UIUnLock, 0);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        //return;
    }

    // ------------------------------------
    // ----- Set Object Event Handler -----
    // ------------------------------------
    err = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handler, NULL);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set event handler" << std::endl;
        //return;
    }

    // ---------------------------
    // ----- Camera settings -----
    // ---------------------------
    // Read "EDSDK API Programming Reference" to know property values
    // (ex. ISO 400 -> 0x58)
    EdsUInt32 iso = 0x00000048;  // ISO 400
    err = EdsSetPropertyData(camera, kEdsPropID_ISOSpeed, 0, sizeof(iso), &iso);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set ISO" << std::endl;
        //return;
    }

    EdsUInt32 av = 0x68;  // Av 20
    err = EdsSetPropertyData(camera, kEdsPropID_Av, 0, sizeof(av), &av);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set Av" << std::endl;
        //return;
    }

    EdsUInt32 tv = 0x64;  // Tv 1/4
    err = EdsSetPropertyData(camera, kEdsPropID_Tv, 0, sizeof(tv), &tv);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set Tv" << std::endl;
        //return;
    }

    // --------------------------
    // ----- Take a picture -----
    // --------------------------
    err = EdsSendCommand(camera, kEdsCameraCommand_TakePicture, 0);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to take a picture" << std::endl;
        //return;
    }
    std::cout << "set shutter" << std::endl;

    while (!flag) EdsGetEvent();  // wait until finish
    flag = false;
    if (eventRef == NULL) {
        std::cout << "Failed to handle event" << std::endl;
        //return;
    }

    // -------------------------------------------
    // ----- Send the image to host computer -----
    // -------------------------------------------
    EdsDirectoryItemInfo dirItemInfo;
    err = EdsGetDirectoryItemInfo(eventRef, &dirItemInfo);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to obtain directory" << std::endl;
        //return;
    }

    EdsStreamRef stream = NULL;

    // Case 1 : Save image as file
    // -----------------------------
    // err = EdsCreateFileStream(dirItemInfo.szFileName,
    //   kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
    // if (err != EDS_ERR_OK) {
    //   std::cout << "Failed to create file stream" << std::endl;
    //   return false;
    // }

    // err = EdsCreateMemoryStream(dirItemInfo.size, &stream);
    // if (err != EDS_ERR_OK) {
    //   std::cout << "Failed to create file stream" << std::endl;
    //   return false;
    // }

    // err = EdsDownload(eventRef, dirItemInfo.size, stream);
    // if (err != EDS_ERR_OK) {
    //   std::cout << "Failed to download" << std::endl;
    //   return false;
    // }
    // -----------------------------

    // Case 2 : Convert the image into cv::Mat
    // (You have to set image quality as jpeg)
    // -----------------------------------------
    err = EdsCreateMemoryStream(dirItemInfo.size, &stream);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to create file stream" << std::endl;
        //return;
    }

    err = EdsDownload(eventRef, dirItemInfo.size, stream);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to download" << std::endl;
        //return;
    }

    err = EdsDownloadComplete(eventRef);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to download" << std::endl;
        //return;
    }

    unsigned char* data = NULL;
    err = EdsGetPointer(stream, (EdsVoid**)&data);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to get pointer" << std::endl;
        //return;
    }

    EdsUInt64 size = 0;
    err = EdsGetLength(stream, &size);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to get image size" << std::endl;
        //return;
    }

    std::vector<unsigned char> buffer(data, data + size);
    cv::Mat img = cv::imdecode(buffer, cv::ImreadModes::IMREAD_COLOR);
    // `img` should be used after release `eventRef` and `stream`
    // -----------------------------------------

    err = EdsRelease(eventRef);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to release" << std::endl;
        //return;
    }
    eventRef = NULL;

    err = EdsRelease(stream);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to release" << std::endl;
        //return;
    }

    // ---------------------
    // ----- Terminate -----
    // ---------------------
    err = EdsRelease(camera);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to release" << std::endl;
        //return;
    }
    std::cout << "released" << std::endl;
    // 2枚目のディスプレイにフルスクリーン表示

   /* cv::imshow("test", img);
    cv::imwrite("test.jpg", img);
    cv::waitKey();*/

    return img;
}


int main() {
    //canon初期化
   /* EdsError err;

    err = EdsInitializeSDK();
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to load SDK" << std::endl;
        return 0;
    }*/


    Mat img, img_copy;
    //Mat haikei = Mat(CAMHEIGHT, CAMWIDTH, CV_8UC3);
    Mat haikei = Mat(800, 1280, CV_8UC3);

    Mat img2 = Mat(HEIGHT, WIDTH, CV_8UC3);
    vector<cv::Mat> splits;

    Mat homography2 = (Mat_<double>(3, 3) <<
        2.414292407924759, -0.01805056408768506, -135.0440503207785,
        -0.01018302623782823, 2.42647682545644, -150.1289603544682,
        -3.673621261807999e-05, -2.670135083004852e-06, 1
        );


    //float wb_r = 4.9;
    //float wb_g = 3.3;
    //float wb_b = 4.2;

    //float wb_r = 4.1;
    //float wb_g = 2.5;
    //float wb_b = 2.6;

    //float wb_r = 0.9;
    //float wb_g = 0.9;
    //float wb_b = 0.9;
    float wb_b = 0.9;
    float wb_g = 0.8;
    float wb_r = 0.8;
    int cam_time = 166666;
    //xiAPIplusCameraOcv cam;
    //cam.OpenFirst();
    //cam.SetExposureTime(cam_time); //10000 us = 10 ms
    //cam.SetImageDataFormat(XI_RGB24);
    //cam.SetWhiteBalanceBlue(wb_b);
    //cam.SetWhiteBalanceGreen(wb_g);
    //cam.SetWhiteBalanceRed(wb_r);
    ////cam.EnableWhiteBalanceAuto();
    //cam.StartAcquisition();
    //cam.SetGammaLuminosity(1.0);
    //cam.SetGammaChromaticity(1.0);
    int count = 0;
    double ave = 0;
    double ave_count = 0;

    cv::namedWindow("haikei", cv::WINDOW_NORMAL);
    cv::resizeWindow("haikei", WIDTH, HEIGHT);
    //cv::moveWindow("haikei", 1920 * NUMBER, 0);
    cv::moveWindow("haikei", 1920, 0);
    cv::setWindowProperty("haikei", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    char key = 0;
    while (key != 'q') {
        key = cv::waitKey(1);
    }

    for (int BGR = 2; BGR < 3; BGR++) {

        std::printf("start | %d\n", BGR);
        //std::ofstream os(filename + std::to_string(BGR) + ".csv");
        count = 0;
        //haikei = Mat(CAMHEIGHT, CAMWIDTH, CV_8UC3);
        haikei = Mat(800, 1280, CV_8UC3);

        while (1) {

            //split(haikei, splits);
            if (BGR == 0) {
                for (int h = 0; h < 800; h++) {
                    for (int w = 0; w < 1280; w++) {
                        haikei.at<cv::Vec3b>(h, w)[0] = count * 5;
                        haikei.at<cv::Vec3b>(h, w)[1] = 0;
                        haikei.at<cv::Vec3b>(h, w)[2] = 0;

                        //splits[BGR] = count * 5;
                    }
                }
            }

            if (BGR == 1) {
                for (int h = 0; h < 800; h++) {
                    for (int w = 0; w < 1280; w++) {
                        haikei.at<cv::Vec3b>(h, w)[0] = 0;
                        haikei.at<cv::Vec3b>(h, w)[1] = count * 5;
                        haikei.at<cv::Vec3b>(h, w)[2] = 0;

                        //splits[BGR] = count * 5;
                    }
                }
            }

            if (BGR == 2) {
                for (int h = 0; h < 800; h++) {
                    for (int w = 0; w < 1280; w++) {
                        haikei.at<cv::Vec3b>(h, w)[0] = 0;
                        haikei.at<cv::Vec3b>(h, w)[1] = 0;
                        haikei.at<cv::Vec3b>(h, w)[2] = count * 5;
                        //splits[BGR] = count * 5;
                    }
                }
            }
            //for (int h = Y_MIN; h < Y_MAX; h++) {
            //	for (int w = X_MIN; w < X_MAX; w++) {
            //		haikei.at<cv::Vec3b>(h, w)[0] = 0;
            //		haikei.at<cv::Vec3b>(h, w)[1] = 0;
            //		haikei.at<cv::Vec3b>(h, w)[2] = count * 5;
            //		//splits[BGR] = count * 5;
            //	}
            //}
            //merge(splits, haikei);

            //warpPerspective(haikei, img2, homography2, img2.size());

            cv::imshow("haikei", haikei);
            //cv::imshow("haikei", img2);
            key = 0;
            //std::cout << "push:q" << std::endl;
            while (key != 'q')
            {
                key = cv::waitKey(1);


            }


            //img = execute();
            ////flip(img, img, -1);
            //img.copyTo(img_copy);
            ////resize(img, img, cv::Size(), 0.5, 0.5);
            ////cv::imshow("aaa", img);

            //cv::imwrite("gamma_pro1_" + to_string(BGR) + "_" + to_string(count) + "_.jpg", img_copy);
            //for (int i = X_MIN; i <= X_MAX; i++) {
            //    for (int j = Y_MIN; j <= Y_MAX; j++) {
            //        //for (int i = 300; i <= 350; i++) {
            //        //	for (int j = 240; j <= 280; j++) {
            //        ave_count++;
            //        if (BGR == 0) {
            //            ave += (double)img_copy.at<cv::Vec3b>(j, i)[0];
            //        }
            //        if (BGR == 1) {
            //            ave += (double)img_copy.at<cv::Vec3b>(j, i)[1];
            //        }
            //        if (BGR == 2) {
            //            ave += (double)img_copy.at<cv::Vec3b>(j, i)[2];
            //        }

            //    }
            //}


            ave /= ave_count;
            //os << ave << std::endl;
            std::printf("%d | %lf \n", count * 5, ave);

            count++;
            ave = 0;
            ave_count = 0;

            if (count == 52) {
                break;
            }

        }
        //os.close();

    }
    //EdsTerminateSDK();
    std::printf("\ncomplete!!!!!\n");

}