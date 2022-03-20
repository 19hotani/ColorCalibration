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
#include <conio.h>

using namespace cv;
using namespace std;
using namespace aruco;


#define NUMBER 1


#define WIDTH 1280
#define HEIGHT 800
#define CAMWIDTH 6960
#define CAMHEIGHT 4640


#define X_MIN 3725
#define Y_MIN 2065
#define X_MAX 5053
#define Y_MAX 2607

#include "EDSDK.h"             // Checked at EDSDK 3.8
#include <opencv2/opencv.hpp>  // Checked at OpenCV 4.0.0-pre

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
   // Read "EDSDK API Programming Reference" to know property values
    // (ex. ISO 400 -> 0x58)
    EdsUInt32 iso = 0x48;  // ISO 400
    err = EdsSetPropertyData(camera, kEdsPropID_ISOSpeed, 0, sizeof(iso), &iso);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set ISO" << std::endl;
        //return;
    }

    EdsUInt32 av = 0x30;  // Av 20
    err = EdsSetPropertyData(camera, kEdsPropID_Av, 0, sizeof(av), &av);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set Av" << std::endl;
        //return;
    }

    EdsUInt32 tv = 0x60;  // Tv 1/4
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
    float wb_b = 1.5;
    float wb_g = 0.9;
    float wb_r = 1.1;

    Mat T_Pro, img;
    /*double Pro_gamma[3] = {
    2.1893678412980857, 2.930377913390212, 2.5357228292199276
    };*/
    double Pro_gamma[3] = {
      2.1423118282391296, 2.198967693021996, 2.1835081537225856
    };

    double Pro_gamma_blue = 0;
    double Pro_gamma_red = 0;
    double Pro_gamma_green = 0;

    Mat resize_gamma = Mat(HEIGHT, WIDTH, CV_8UC3);
    //T_Pro = imread("projected/down_resize_gray.jpg");
    T_Pro = imread("new_exp/_Parrots.png");
    //T_Pro = imread("resizegamma.jpg");

    cv::resize(T_Pro, T_Pro, Size(1280, 800));
    EdsError err;

    err = EdsInitializeSDK();
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to load SDK" << std::endl;
        return 0;
    }

    Mat resize = imread("new_exp/_Parrots.png");
    for (int i = 0; i < 1280; i++) {
    	for (int j = 0; j < 800; j++) {


    		Pro_gamma_blue = pow((resize.at<cv::Vec3b>(j, i)[0] / (double)255.0), ((double)1 / Pro_gamma[0])) * (double)255;
    		Pro_gamma_green = pow((resize.at<cv::Vec3b>(j, i)[1] / (double)255.0), ((double)1 / Pro_gamma[1])) * (double)255;
    		Pro_gamma_red = pow((resize.at<cv::Vec3b>(j, i)[2] / (double)255.0), ((double)1 / Pro_gamma[2])) * (double)255;

    		if ((0 <= (int)Pro_gamma_blue) && ((int)Pro_gamma_blue <= 255)) {
    			resize_gamma.at<cv::Vec3b>(j, i)[0] = (int)Pro_gamma_blue;
    		}
    		else if ((int)Pro_gamma_blue > 255) {
    			resize_gamma.at<cv::Vec3b>(j, i)[0] = 255;
    		}
    		else if ((int)Pro_gamma_blue < 0) {
    			resize_gamma.at<cv::Vec3b>(j, i)[0] = 0;
    		}

    		if ((0 <= (int)Pro_gamma_green) && ((int)Pro_gamma_green <= 255)) {
    			resize_gamma.at<cv::Vec3b>(j, i)[1] = (int)Pro_gamma_green;
    		}
    		else if ((int)Pro_gamma_green > 255) {
    			resize_gamma.at<cv::Vec3b>(j, i)[1] = 255;
    		}
    		else if ((int)Pro_gamma_green < 0) {
    			resize_gamma.at<cv::Vec3b>(j, i)[1] = 0;
    		}

    		if ((0 <= (int)Pro_gamma_red) && ((int)Pro_gamma_red <= 255)) {
    			resize_gamma.at<cv::Vec3b>(j, i)[2] = (int)Pro_gamma_red;
    		}
    		else if ((int)Pro_gamma_red > 255) {
    			resize_gamma.at<cv::Vec3b>(j, i)[2] = 255;
    		}
    		else if ((int)Pro_gamma_red < 0) {
    			resize_gamma.at<cv::Vec3b>(j, i)[2] = 0;
    		}

    	}
    }
    //imwrite("projected/ガンマのみParrots.jpg", resize_gamma);

    cv::namedWindow("Pro", cv::WINDOW_NORMAL);
    cv::resizeWindow("Pro", WIDTH, HEIGHT);
    cv::moveWindow("Pro", 1920, 0);
    cv::setWindowProperty("Pro", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    int n;
    int sum = 0;

    char key = 0;

    while (1) {
        cv::imshow("Pro", T_Pro);
        waitKey(1000);
        //waitKey(0);
        img = execute();
        imshow("a", img);

        while (key != 'q') {
            key = cv::waitKey(1);
        }
        imwrite("new_exp/before2_par_cap.png", img);
        //imwrite("result/手_Parrots_前.png", img);
        break;

    }
    /*imwrite("mandrillbycam2.png", img);*/



}

