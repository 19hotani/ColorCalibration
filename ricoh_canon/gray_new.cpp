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

#include <fstream>

#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>  // checked at opencv 3.4.1
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/structured_light.hpp>


//#include "xiApiPlusOcv.hpp" 
//#include "xiApiPlusOcv.cpp" 


using namespace cv;
using namespace std;
using namespace aruco;
//
//
//#define NUMBER 2
//
//string filename = "6_Gamma_2_Pro" + to_string(NUMBER) + "_";
//
//#define X_MIN 384
//#define X_MAX 895
//#define Y_MIN 256
//#define Y_MAX 767
//
//#define WIDTH 1280
//#define HEIGHT 800
//#define CAMWIDTH 1280
//#define CAMHEIGHT 1024
//

#define WINDOWWIDTH 1280
#define WINDOWHEIGHT 800
#define GRAYCODEWIDTHSTEP 3
#define GRAYCODEHEIGHTSTEP 3
#define GRAYCODEWIDTH WINDOWWIDTH / GRAYCODEWIDTHSTEP
#define GRAYCODEHEIGHT WINDOWHEIGHT / GRAYCODEHEIGHTSTEP
#define WHITETHRESHOLD 10
#define BLACKTHRESHOLD 40

//#define DATE 309


#include <iostream>
#include <vector>

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

int initCamera(EdsCameraRef& camera);
int capture(EdsCameraRef camera, cv::Mat& img);
int terminateCamera(EdsCameraRef camera);


// Camera to Projector
struct C2P {
    int cx;
    int cy;
    int px;
    int py;
    C2P(int camera_x, int camera_y, int proj_x, int proj_y) {
        cx = camera_x;
        cy = camera_y;
        px = proj_x;
        py = proj_y;
    }
};




std::vector<C2P> c2pList;
cv::Mat viz;



void main() {

    std::string filename = "img_";
    // -----------------------------------
    // ----- Prepare graycode images -----
    // -----------------------------------
    cv::structured_light::GrayCodePattern::Params params;
    params.width = GRAYCODEWIDTH;
    params.height = GRAYCODEHEIGHT;
    //params.width = WINDOWWIDTH;
    //params.height = WINDOWHEIGHT;

    auto pattern = cv::structured_light::GrayCodePattern::create(params);

    // 用途:decode時にpositiveとnegativeの画素値の差が
    //      常にwhiteThreshold以上である画素のみdecodeする
    pattern->setWhiteThreshold(WHITETHRESHOLD);
    // 用途:ShadowMask計算時に white - black > blackThreshold
    //      ならば前景（グレイコードを認識した）と判別する
    // 今回はこれを設定しても参照されることはないが一応セットしておく
    pattern->setBlackThreshold(BLACKTHRESHOLD);

    std::vector<cv::Mat> graycodes;
    pattern->generate(graycodes);

    cv::Mat blackCode, whiteCode;
    pattern->getImagesForShadowMasks(blackCode, whiteCode);
    graycodes.push_back(blackCode), graycodes.push_back(whiteCode);

    // -----------------------------
    // ----- Prepare cv window -----
    // -----------------------------
    cv::namedWindow("Pattern", cv::WINDOW_NORMAL);
    cv::resizeWindow("Pattern", GRAYCODEWIDTH, GRAYCODEHEIGHT);
    // 2枚目のディスプレイにフルスクリーン表示
    cv::moveWindow("Pattern", 1920, 0);
    cv::setWindowProperty("Pattern", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    cv::namedWindow("camera", cv::WINDOW_NORMAL);
    //cv::resizeWindow("camera", 640, 480);
    // 2枚目のディスプレイにフルスクリーン表示
    //cv::moveWindow("camera", -1920, 0);
    cv::setWindowProperty("camera", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

    cv::resizeWindow("camera", 6960, 4640);
    cv::moveWindow("camera", 100, 100);


    // ----------------------------------
    // ----- Wait camera adjustment -----
    // ----------------------------------

    EdsError err;

    err = EdsInitializeSDK();
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to load SDK" << std::endl;
        return;
    }
    EdsCameraRef camera = NULL;
    initCamera(camera);

    cv::Mat image;
    // 撮影
    if (!capture(camera, image)) {
        std::cout << "An error ocurred" << std::endl;
    }

    //initializeCamera();
    ///*cv::moveWindow("camera", -1280, 0);*/
    int time = 16666;
    cv::imshow("Pattern", graycodes[graycodes.size() - 3]);
    while (true) {
        //cv::Mat img = getCameraImage();
        cv::imshow("camera", image);
        cv::waitKey(1);
        if (GetKeyState('C') < 0) {
            break;
        }

    }
    printf("time:%d\n\n", time);

    // --------------------------------
    // ----- Capture the graycode -----
    // --------------------------------
    std::vector<cv::Mat> captured;
    int cnt = 0;
    for (auto gimg : graycodes) {
        std::cout << "ここまで1" << std::endl;
        //flip(gimg, gimg, 0);
        cv::imshow("Pattern", gimg);
        // ディスプレイに表示->カメラバッファに反映されるまで待つ
        // 必要な待ち時間は使うカメラに依存
        //cv::waitKey(400);
        char key = 0;

        //手動にしたいとき
      /*  while (key != 'q')
        {
            key = cv::waitKey(1);
        }*/

        //cv::waitKey(1100);
        std::cout << "ここまで2" << std::endl;

        // グレイスケールで撮影
        cv::Mat img;

        if (!capture(camera, img)) {
            std::cout << "An error ocurred" << std::endl;
        }
        //flip(img, img, -1);
        cv::imshow("camera", img);
        //moveWindow("camera", 0, 0);
        /*cv::waitKey(1);*/
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << cnt++;
        cv::imwrite("cam_" + oss.str() + ".png", img);

        captured.push_back(img);
        std::cout << cnt << std::endl;
    }


    terminateCamera(camera);


    // -------------------------------
    // ----- Decode the graycode -----
    // -------------------------------
    // pattern->decode()は視差マップの解析に使う関数なので今回は使わない
    // pattern->getProjPixel()を使って各カメラ画素に写ったプロジェクタ画素の座標を計算
    cv::Mat white = captured.back();
    captured.pop_back();
    cv::Mat black = captured.back();
    captured.pop_back();

    int camHeight = 4640;
    int camWidth = 6960;

    cv::Mat c2pX = cv::Mat::zeros(camHeight, camWidth, CV_16U);
    cv::Mat c2pY = cv::Mat::zeros(camHeight, camWidth, CV_16U);
    std::vector<C2P> c2pList;
    for (int y = 0; y < camHeight; y++) {
        for (int x = 0; x < camWidth; x++) {
            cv::Point pixel;
            if (white.at<cv::uint8_t>(y, x) - black.at<cv::uint8_t>(y, x) > BLACKTHRESHOLD &&
                !pattern->getProjPixel(captured, x, y, pixel)) {
                c2pX.at<cv::uint16_t>(y, x) = pixel.x;
                c2pY.at<cv::uint16_t>(y, x) = pixel.y;
                c2pList.push_back(C2P(x, y, pixel.x * GRAYCODEWIDTHSTEP, pixel.y * GRAYCODEHEIGHTSTEP));
            }
        }
    }

    int countnum = 0;
    // ---------------------------
    // ----- Save C2P as csv -----
    // ---------------------------
    std::ofstream os("c2p.csv");
    for (auto elem : c2pList) {
        countnum++;
        os << elem.cx << ", " << elem.cy << ", " << elem.px << ", " << elem.py << std::endl;
        //std::cout << "roop" << std::endl;
    }
    os.close();

    // ----------------------------
    // ----- Visualize result -----
    // ----------------------------
    cv::Mat viz = cv::Mat::zeros(camHeight, camWidth, CV_8UC3);
    for (int y = 0; y < camHeight; y++) {
        for (int x = 0; x < camWidth; x++) {
            viz.at<cv::Vec3b>(y, x)[0] = (unsigned char)c2pX.at<cv::uint16_t>(y, x);
            viz.at<cv::Vec3b>(y, x)[1] = (unsigned char)c2pY.at<cv::uint16_t>(y, x);
        }
    }

    std::string aa;
    std::string str_buf;
    std::string str_conma_buf;
    std::ifstream ifs_csv_file("c2p.csv");

    // 変換前の画像での座標
    std::vector<cv::Point2f> src_pt(countnum);

    // 変換後の画像での座標
    std::vector<cv::Point2f> dst_pt(countnum);

    int count = 0;
    int len = 0;

    while (getline(ifs_csv_file, str_buf)) {
        // 「,」区切りごとにデータを読み込むためにistringstream型にする
        std::istringstream i_stream(str_buf);

        // 「,」区切りごとにデータを読み込む
        while (getline(i_stream, str_conma_buf, ',')) {
            // csvファイルに書き込む
            aa = str_conma_buf;

            if (len == 0) {
                src_pt[count].x = std::stoi(aa);
            }
            else if (len == 1) {
                src_pt[count].y = std::stoi(aa);
            }
            else if (len == 2) {
                dst_pt[count].x = std::stoi(aa);
            }
            else if (len == 3) {
                dst_pt[count].y = std::stoi(aa);
                len = -1;
                count++;
            }
            len++;

        }
        // 改行する
    }

    // homography行列を計算
    const cv::Mat homography_matrix = findHomography(src_pt, dst_pt);

    std::cout << homography_matrix << std::endl;

    /*cv::imshow("result", viz);
    cv::waitKey(0);*/
    viz.convertTo(viz, CV_8U, 255);
    //cv::imwrite("result2.png", viz);


    viz = cv::Mat::zeros(camHeight, camWidth, CV_8UC3);
    for (int y = 0; y < camHeight; y++) {
        for (int x = 0; x < camWidth; x++) {
            viz.at<cv::Vec3b>(y, x)[0] = (unsigned char)c2pX.at<cv::uint16_t>(y, x);
            viz.at<cv::Vec3b>(y, x)[1] = (unsigned char)c2pY.at<cv::uint16_t>(y, x);
        }
    }
    std::cout << "ここまで" << std::endl;
    cv::imshow("result", viz);
    cv::imwrite("result.png", viz);


    //////

    //Mat input_img = imread("result.png", IMREAD_UNCHANGED);
    //Mat gray_img = imread("result.png", IMREAD_GRAYSCALE);

    //Mat bin_img; //2値画像を入れておくためのMat
    //Mat rev_img;
    //cv::bitwise_not(gray_img, rev_img);

    //threshold(rev_img, bin_img, 254.999, 255, THRESH_BINARY); //閾値160で2値画像に変換
    ////threshold(rev_img, bin_img, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定

    ////imshow("binary", bin_img);

    //cv::imwrite("result_binary.png", bin_img);
    ////cv::waitKey(0);

    //Mat mask_arround = imread("cam_33.png", IMREAD_GRAYSCALE);
    //Mat bin_img2;
    //threshold(mask_arround, bin_img2, 90, 255, THRESH_BINARY); //閾値160で2値画像に変換
    //cv::imwrite("result_binary_arround.png", bin_img2);
    //imshow("binary", bin_img2);
    //cv::waitKey(1);
    //Mat result_or;
    //cv::bitwise_and(bin_img, bin_img2, result_or);

    ////最終的なマスク
    //cv::imwrite("result_mask.png", result_or);
    //imshow("result_mask", result_or);
    //cv::waitKey(1);

    //Mat result_inpaint;
    ////インペイント
    ////Mat viz = imread("result.png");//グレイコード部分消したときのみ使用
    //cv::inpaint(viz, result_or, result_inpaint, 5, cv::INPAINT_TELEA);
    //cv::imwrite("result_inpaint.png", result_inpaint);
    //imshow("result_inpaint", result_inpaint);

    cv::Mat cam_img = viz;
    camHeight = cam_img.rows;
    camWidth = cam_img.cols;
    cv::Mat proj_img(cv::Size(WINDOWWIDTH, WINDOWHEIGHT), CV_8UC3, cv::Scalar(0, 0, 0));

    for (auto elem : c2pList) {
        proj_img.at<cv::Vec3b>(elem.py, elem.px)[0] = 0;
        proj_img.at<cv::Vec3b>(elem.py, elem.px)[1] = 255 * elem.cy / camHeight;
        proj_img.at<cv::Vec3b>(elem.py, elem.px)[2] = 255 * elem.cx / camWidth;
    }

    cv::Mat proj_mini;
    imshow("projector_img", proj_img);
    cv::imwrite("projector_img.png", proj_img);
    cv::waitKey(0);

    EdsTerminateSDK();

    return;
}



int initCamera(EdsCameraRef& camera) {
    EdsError err;

    // -----------------------------------
    // ----- Get reference to camera -----
    // -----------------------------------
    EdsCameraListRef cameraList = NULL;
    err = EdsGetCameraList(&cameraList);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to obtain camera list" << std::endl;
        return false;
    }

    EdsUInt32 count = 0;
    err = EdsGetChildCount(cameraList, &count);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to obtain number of cameras" << std::endl;
        EdsTerminateSDK();
        return false;
    }
    if (count == 0) {
        std::cout << "Camera was not found" << std::endl;
        EdsTerminateSDK();
        return false;
    }

    err = EdsGetChildAtIndex(cameraList, 0, &camera);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to get first camera" << std::endl;
        EdsTerminateSDK();
        return false;
    }

    EdsDeviceInfo deviceInfo;
    err = EdsGetDeviceInfo(camera, &deviceInfo);
    if (err != EDS_ERR_OK || camera == NULL) {
        std::cout << "Failed to get device info" << std::endl;
        EdsTerminateSDK();
        return false;
    }
    bool isLegacy = deviceInfo.deviceSubType == 0;

    EdsRelease(cameraList);

    // ------------------------
    // ----- Open session -----
    // ------------------------
    err = EdsOpenSession(camera);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to open session" << std::endl;
        return false;
    }

    // -------------------------
    // ----- Configuration -----
    // -------------------------
    EdsUInt32 quality = EdsImageQuality_LJN;  // Save as normal jpeg
    err = EdsSetPropertyData(camera, kEdsPropID_ImageQuality, 0, sizeof(quality),
        &quality);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        return false;
    }

    EdsUInt32 saveTo = kEdsSaveTo_Host;
    err =
        EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(saveTo), &saveTo);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        return false;
    }

    EdsCapacity capacity = { 0x7FFFFFFF, 0x1000, 1 };
    err = EdsSendStatusCommand(camera, kEdsCameraStatusCommand_UILock, 0);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        return false;
    }
    err = EdsSetCapacity(camera, capacity);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        return false;
    }
    err = EdsSendStatusCommand(camera, kEdsCameraStatusCommand_UIUnLock, 0);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to configure" << std::endl;
        return false;
    }

    // ------------------------------------
    // ----- Set Object Event Handler -----
    // ------------------------------------
    err = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handler, NULL);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set event handler" << std::endl;
        return false;
    }

    // ---------------------------
    // ----- Camera settings -----
    // ---------------------------
    // Read "EDSDK API Programming Reference" to know property values
    // (ex. ISO 400 -> 0x58)
    // (ex. ISO 200 -> 0x50)
    EdsUInt32 iso = 0x53;  // ISO 400
    err = EdsSetPropertyData(camera, kEdsPropID_ISOSpeed, 0, sizeof(iso), &iso);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set ISO" << std::endl;
        //return;
    }

    EdsUInt32 av = 0x50;  // Av 20
    err = EdsSetPropertyData(camera, kEdsPropID_Av, 0, sizeof(av), &av);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set Av" << std::endl;
        //return;
    }

    EdsUInt32 tv = 0x4C;  // Tv 1/4
    err = EdsSetPropertyData(camera, kEdsPropID_Tv, 0, sizeof(tv), &tv);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to set Tv" << std::endl;
        //return;
    }
}


int capture(EdsCameraRef camera, cv::Mat& img) {

    EdsError err;

    // --------------------------
    // ----- Take a picture -----
    // --------------------------
    err = EdsSendCommand(camera, kEdsCameraCommand_TakePicture, 0);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to take a picture" << std::endl;
        return false;
    }
    //std::cout << "set shutter" << std::endl;

    while (!flag) EdsGetEvent();  // wait until finish
    flag = false;
    if (eventRef == NULL) {
        std::cout << "Failed to handle event" << std::endl;
        return false;
    }

    // -------------------------------------------
    // ----- Send the image to host computer -----
    // -------------------------------------------
    EdsDirectoryItemInfo dirItemInfo;
    err = EdsGetDirectoryItemInfo(eventRef, &dirItemInfo);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to obtain directory" << std::endl;
        return false;
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
        return false;
    }

    err = EdsDownload(eventRef, dirItemInfo.size, stream);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to download" << std::endl;
        return false;
    }

    err = EdsDownloadComplete(eventRef);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to download" << std::endl;
        return false;
    }

    unsigned char* data = NULL;
    err = EdsGetPointer(stream, (EdsVoid**)&data);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to get pointer" << std::endl;
        return false;
    }

    EdsUInt64 size = 0;
    err = EdsGetLength(stream, &size);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to get image size" << std::endl;
        return false;
    }

    std::vector<unsigned char> buffer(data, data + size);
    img = cv::imdecode(buffer, cv::ImreadModes::IMREAD_COLOR);
    // `img` should be used after release `eventRef` and `stream`
    // -----------------------------------------

    err = EdsRelease(eventRef);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to release" << std::endl;
        return false;
    }
    eventRef = NULL;

    err = EdsRelease(stream);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to release" << std::endl;
        return false;
    }

    //resize(img, img, cv::Size(), 640 / img.cols , 480 / img.rows );
    //namedWindow("result", WINDOW_NORMAL);
    //imshow("result", img);
    cout << img.cols << 'f' << img.rows << endl;
}


int terminateCamera(EdsCameraRef camera) {

    EdsError err;

    //---------------------
    //----- Terminate -----
    //---------------------
    err = EdsRelease(camera);
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to release" << std::endl;
        return false;
    }

    return true;
}