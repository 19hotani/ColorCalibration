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


using namespace cv;
using namespace std;
using namespace aruco;

#define NUMBER 2


#define WIDTH 1280
#define HEIGHT 800
#define CAMWIDTH 6960
#define CAMHEIGHT 4640


#define X_MIN 3500
#define Y_MIN 950
#define X_MAX 4200
#define Y_MAX 1400
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

    Mat img, img_copy;
    Mat Color_Pro = Mat(HEIGHT, WIDTH, CV_8UC3);
    Mat haikei = Mat(CAMHEIGHT, CAMWIDTH, CV_8UC3);
    Mat Cap_E = Mat(CAMHEIGHT, CAMWIDTH, CV_8UC3);
    Mat Pro = Mat(HEIGHT, WIDTH, CV_8UC3);
    vector<cv::Mat> Splits;

    string filename;

    // ガンマ補正後のピクセル値
    double Pro_gamma_blue = 0;
    double Pro_gamma_red = 0;
    double Pro_gamma_green = 0;

   /* double Pro_gamma[3] = {
            2.629834686432756, 2.055648716795304 ,2.3887616929430098
    };*/
 

    //やまけんさん
    double Pro_gamma[3] = {
        2.1423118282391296, 2.198967693021996, 2.1835081537225856
    };

    //Mat homography = (Mat_<double>(3, 3) <<
    //    0.4504646534929118, -0.2102947107650047, -155.5028144886838,
    //    0.2204709804305548, 0.4041117709456818, -925.0566285047865,
    //    5.817536125382487e-05, 2.519207151871143e-05, 0.9999999999999999
    //    );
    // 黒投影画像作成
    /*split(Cap_E, Splits);
    Splits[0] = 6;
    Splits[1] = 6;
    Splits[2] = 6;*/
    split(Cap_E, Splits);
    Splits[0] = 0.002122;
    Splits[1] = 0.004863;
    Splits[2] = 0.003486;
    merge(Splits, Cap_E);

    int Count = 0;
    float wb_b = 1.5;
    float wb_g = 0.9;
    float wb_r = 1.1;


    // カメラ初期設定
    //int cam_time = 17228;
    //xiAPIplusCameraOcv cam;
    //cam.OpenFirst();
    //cam.SetExposureTime(cam_time); //10000 us = 10 ms
    //cam.SetImageDataFormat(XI_RGB24);

    //cam.SetWhiteBalanceBlue(wb_b);
    //cam.SetWhiteBalanceGreen(wb_g);
    //cam.SetWhiteBalanceRed(wb_r);
    ////cam.EnableWhiteBalanceAuto();
    //cam.SetGammaLuminosity(1.0);
    //cam.SetGammaChromaticity(1.0);
    //cam.StartAcquisition();

    // Pro用のウィンドウ生成
    cv::namedWindow("Pro", cv::WINDOW_NORMAL);
    cv::resizeWindow("Pro", WIDTH, HEIGHT);
    cv::moveWindow("Pro", 1920, 0);
    cv::setWindowProperty("Pro", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    while (true) {
        //cv::Mat img = getCameraImage();
        //cv::imshow("camera", image);
        cv::waitKey(1);
        if (GetKeyState('C') < 0) {
            break;
        }

    }
    EdsError err;

    err = EdsInitializeSDK();
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to load SDK" << std::endl;
        return 0;
    }

    // キャリブレーション画像投影
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int m = 0; m < 4; m++) {
                // 色変更
                split(Color_Pro, Splits);

                //自分で投影したい色を決めよう→T_Pro.at<cv::Vec3b>(j, i)[0]入れる

                Pro_gamma_blue = pow(((double)(50 * i) / (double)255), ((double)1 / Pro_gamma[0])) * (double)255;
                Pro_gamma_green = pow(((double)(50 * j) / (double)255), ((double)1 / Pro_gamma[1])) * (double)255;
                Pro_gamma_red = pow(((double)(50 * m) / (double)255), ((double)1 / Pro_gamma[2])) * (double)255;

                if ((0 <= (int)Pro_gamma_blue) && ((int)Pro_gamma_blue <= 255)) {
                    Splits[0] = (int)Pro_gamma_blue;
                }
                else if ((int)Pro_gamma_blue > 255) {
                    Splits[0] = 255;
                }
                else if ((int)Pro_gamma_blue < 0) {
                    Splits[0] = 0;
                }
                if ((0 <= (int)Pro_gamma_green) && ((int)Pro_gamma_green <= 255)) {
                    Splits[1] = (int)Pro_gamma_green;
                }
                else if ((int)Pro_gamma_green > 255) {
                    Splits[1] = 255;
                }
                else if ((int)Pro_gamma_green < 0) {
                    Splits[1] = 0;
                }
                if ((0 <= (int)Pro_gamma_red) && ((int)Pro_gamma_red <= 255)) {
                    Splits[2] = (int)Pro_gamma_red;
                }
                else if ((int)Pro_gamma_red > 255) {
                    Splits[2] = 255;
                }
                else if ((int)Pro_gamma_red < 0) {
                    Splits[2] = 0;
                }

                merge(Splits, Color_Pro);
                //warpPerspective(Color_Pro, Pro, homography, Pro.size());


                while (1) {

                    // 投影
                    cv::imshow("Pro", Color_Pro);
                    //cv:waitKey(500);
                    char key = 0;
                    //std::cout << "push:q" << std::endl;
                    while (key != 'q')
                    {
                        key = cv::waitKey(1);


                    }

                    //cv::waitKey(1000);

                    // 撮影
                    img = execute();
                    //flip(img, img, -1);
                    img.copyTo(img_copy);
                    resize(img, img, cv::Size(), 0.5, 0.5);
                    cv::imshow("cap", img);



                    // 保存
                    filename = "color/Color_" + to_string(NUMBER) + "_" + to_string(Count) + ".jpg";
                    imwrite(filename, img_copy - Cap_E);
                    std::cout << NUMBER << "saved" << std::endl;
                    cv::waitKey(10);

                    break;

                }

                Count++;

            }
        }
    }

    EdsTerminateSDK();
    printf("\ncomplete!!!!!\n");

}