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
#include "EDSDK.h"             // Checked at EDSDK 3.8
#include <opencv2/opencv.hpp>  // Checked at OpenCV 4.0.0-pre

using namespace cv;
using namespace std;
using namespace aruco;

string filename = "White_Pro_";

//#define R1 0
//#define G1 0
//#define B1 0
//
//#define R2 0
//#define G2 0
//#define B2 0


#define WIDTH 1280
#define HEIGHT 800
#define CAMWIDTH 5184
#define CAMHEIGHT 3456

#define X_MIN 2233
#define X_MAX 3365
#define Y_MIN 1581
#define Y_MAX 2569


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
    EdsUInt32 iso = 0x0000005d;  // ISO 400
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

    EdsUInt32 tv = 0x68;  // Tv 1/4
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
    EdsError err;

    err = EdsInitializeSDK();
    if (err != EDS_ERR_OK) {
        std::cout << "Failed to load SDK" << std::endl;
        return 0;
    }

	Mat img;
	Mat haikei2 = Mat(800, 1280, CV_8UC3);
	Mat img2 = Mat(HEIGHT, WIDTH, CV_8UC3);

	vector<cv::Mat> Splits2;

	//Mat homography2 = (Mat_<double>(3, 3) <<
	//	2.414292407924759, -0.01805056408768506, -135.0440503207785,
	//	-0.01018302623782823, 2.42647682545644, -150.1289603544682,
	//	-3.673621261807999e-05, -2.670135083004852e-06, 1
	//	);

	for (int h = 0; h < 800; h++) {
		for (int w = 0; w < 1280; w++) {
			haikei2.at<cv::Vec3b>(h, w)[0] = 0;
			haikei2.at<cv::Vec3b>(h, w)[1] = 0;
			haikei2.at<cv::Vec3b>(h, w)[2] = 255;
		}
	}


	cv::namedWindow("haikei2", cv::WINDOW_NORMAL);
	cv::resizeWindow("haikei2", WIDTH, HEIGHT);
	cv::moveWindow("haikei2", 1920, 0);
	cv::setWindowProperty("haikei2", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
	//warpPerspective(haikei2, img2, homography2, img2.size());
    
	double ave_B = 0;
	double ave_G = 0;
	double ave_R = 0;
	double ave_count = 0;

	//float wb_r = 0.8;
	//float wb_g = 0.8;
	//float wb_b = 0.9;
	////float wb_r = 1.1;
	//float wb_g = 0.9;
	//float wb_b = 0.9;


	//float wb_r = 4.1;
	//float wb_g = 2.5;
	//float wb_b = 2.6;

	//int cam_time = 33333;
	int cam_time = 16666;
	//xiAPIplusCameraOcv cam;
	//cam.OpenFirst();
	//cam.SetExposureTime(cam_time); //10000 us = 10 ms
	//cam.SetImageDataFormat(XI_RGB24);
	//cam.SetWhiteBalanceBlue(wb_b);
	//cam.SetWhiteBalanceGreen(wb_g);
	//cam.SetWhiteBalanceRed(wb_r);
	//cam.SetGammaLuminosity(1.0);
	//cam.SetGammaChromaticity(1.0);

	//cam.EnableWhiteBalanceAuto();
	//cam.StartAcquisition();


	int Count = 0;
	Mat img_copy;

	while (1) {

		cv::imshow("haikei2", haikei2);

		img = execute();
        imwrite("test.png", img);
		//flip(img, img, -1);
		img.copyTo(img_copy);
		resize(img, img, cv::Size(), 0.5, 0.5);
		cv::imshow("cap", img);
		cv::waitKey(-1);
        //char key = 0;
        //std::cout << "ここまで1" << std::endl;

        ////std::cout << "push:q" << std::endl;
        //while (key != 'C')
        //{
        //    key = cv::waitKey(1);


        //}
        //std::cout << "ここまで2" << std::endl;
		if (GetKeyState('C') < 0) {
			string filename2 = filename + to_string(Count) + ".jpg";
			Count++;

			ave_B = 0;
			ave_G = 0;
			ave_R = 0;
			ave_count = 0;

			for (int i = X_MIN; i <= X_MAX; i++) {
				for (int j = Y_MIN; j <= Y_MAX; j++) {
					ave_count++;
					ave_B += (double)img_copy.at<cv::Vec3b>(j, i)[0];
					ave_G += (double)img_copy.at<cv::Vec3b>(j, i)[1];
					ave_R += (double)img_copy.at<cv::Vec3b>(j, i)[2];
				}
			}
			ave_B /= ave_count;
			ave_G /= ave_count;
			ave_R /= ave_count;

			printf(" B | %lf  G | %lf  R | %lf\n", ave_B, ave_G, ave_R);
			cv::waitKey(1000);
		}


		/*if (GetKeyState('A') < 0) {
			cv::waitKey(500);
			wb_b += 0.1;
			printf(" B | %lf  G | %lf  R | %lf\n", wb_b, wb_g, wb_r);
			cam.SetWhiteBalanceBlue(wb_b);
			cam.SetWhiteBalanceGreen(wb_g);
			cam.SetWhiteBalanceRed(wb_r);
		}

		if (GetKeyState('Z') < 0) {
			cv::waitKey(500);
			wb_b -= 0.1;
			printf(" B | %lf  G | %lf  R | %lf\n", wb_b, wb_g, wb_r);
			cam.SetWhiteBalanceBlue(wb_b);
			cam.SetWhiteBalanceGreen(wb_g);
			cam.SetWhiteBalanceRed(wb_r);
		}

		if (GetKeyState('S') < 0) {
			cv::waitKey(500);
			wb_g += 0.1;
			printf(" B | %lf  G | %lf  R | %lf\n", wb_b, wb_g, wb_r);
			cam.SetWhiteBalanceBlue(wb_b);
			cam.SetWhiteBalanceGreen(wb_g);
			cam.SetWhiteBalanceRed(wb_r);
		}

		if (GetKeyState('X') < 0) {
			cv::waitKey(500);
			wb_g -= 0.1;
			printf(" B | %lf  G | %lf  R | %lf\n", wb_b, wb_g, wb_r);
			cam.SetWhiteBalanceBlue(wb_b);
			cam.SetWhiteBalanceGreen(wb_g);
			cam.SetWhiteBalanceRed(wb_r);
		}

		if (GetKeyState('G') < 0) {
			cv::waitKey(500);
			wb_r += 0.1;
			printf(" B | %lf  G | %lf  R | %lf\n", wb_b, wb_g, wb_r);
			cam.SetWhiteBalanceBlue(wb_b);
			cam.SetWhiteBalanceGreen(wb_g);
			cam.SetWhiteBalanceRed(wb_r);
		}

		if (GetKeyState('B') < 0) {
			cv::waitKey(500);
			wb_r -= 0.1;
			printf(" B | %lf  G | %lf  R | %lf\n", wb_b, wb_g, wb_r);
			cam.SetWhiteBalanceBlue(wb_b);
			cam.SetWhiteBalanceGreen(wb_g);
			cam.SetWhiteBalanceRed(wb_r);
		}*/

		if (GetKeyState('Q') < 0) {

            for (int h = 0; h < 800; h++) {
                for (int w = 0; w < 1280; w++) {
					haikei2.at<cv::Vec3b>(h, w)[0] = 150;
					haikei2.at<cv::Vec3b>(h, w)[1] = 150;
					haikei2.at<cv::Vec3b>(h, w)[2] = 150;
				}
			}

			//warpPerspective(haikei1, img1, homography1, img1.size());
			//warpPerspective(haikei2, img2, homography2, img2.size());
			//cv::imshow("haikei", img1);
			cv::imshow("haikei2", haikei2);

		}

		if (GetKeyState('W') < 0) {



            for (int h = 0; h < 800; h++) {
                for (int w = 0; w < 1280; w++) {
					haikei2.at<cv::Vec3b>(h, w)[0] = 255;
					haikei2.at<cv::Vec3b>(h, w)[1] = 0;
					haikei2.at<cv::Vec3b>(h, w)[2] = 0;
				}
			}

			//warpPerspective(haikei1, img1, homography1, img1.size());
			//warpPerspective(haikei2, img2, homography2, img2.size());
			//cv::imshow("haikei", img1);
			cv::imshow("haikei2", haikei2);

		}

		if (GetKeyState('E') < 0) {

			/*for (int h = Y_MIN; h < Y_MAX; h++) {
				for (int w = X_MIN; w < X_MAX; w++) {
					haikei1.at<cv::Vec3b>(h, w)[0] = 0;
					haikei1.at<cv::Vec3b>(h, w)[1] = 255;
					haikei1.at<cv::Vec3b>(h, w)[2] = 0;
				}
			}*/

            for (int h = 0; h < 800; h++) {
                for (int w = 0; w < 1280; w++) {
					haikei2.at<cv::Vec3b>(h, w)[0] = 0;
					haikei2.at<cv::Vec3b>(h, w)[1] = 255;
					haikei2.at<cv::Vec3b>(h, w)[2] = 0;
				}
			}

			//warpPerspective(haikei1, img1, homography1, img1.size());
			//warpPerspective(haikei2, img2, homography2, img2.size());
			//cv::imshow("haikei", img1);
			cv::imshow("haikei2", haikei2);

		}

		if (GetKeyState('R') < 0) {

			//for (int h = Y_MIN; h < Y_MAX; h++) {
			//	for (int w = X_MIN; w < X_MAX; w++) {
			//		haikei1.at<cv::Vec3b>(h, w)[0] = 0;
			//		haikei1.at<cv::Vec3b>(h, w)[1] = 0;
			//		haikei1.at<cv::Vec3b>(h, w)[2] = 255;
			//	}
			//}

            for (int h = 0; h < 800; h++) {
                for (int w = 0; w < 1280; w++) {
					haikei2.at<cv::Vec3b>(h, w)[0] = 0;
					haikei2.at<cv::Vec3b>(h, w)[1] = 0;
					haikei2.at<cv::Vec3b>(h, w)[2] = 255;
				}
			}

			//warpPerspective(haikei2, img2, homography2, img2.size());
			cv::imshow("haikei2", haikei2);

		}

		if (GetKeyState('T') < 0) {

			/*for (int h = Y_MIN; h < Y_MAX; h++) {
				for (int w = X_MIN; w < X_MAX; w++) {
					haikei1.at<cv::Vec3b>(h, w)[0] = 0;
					haikei1.at<cv::Vec3b>(h, w)[1] = 0;
					haikei1.at<cv::Vec3b>(h, w)[2] = 0;
				}
			}*/

			for(int h = 0; h < 800; h++) {
                for (int w = 0; w < 1280; w++) {
					haikei2.at<cv::Vec3b>(h, w)[0] = 0;
					haikei2.at<cv::Vec3b>(h, w)[1] = 0;
					haikei2.at<cv::Vec3b>(h, w)[2] = 0;
				}
			}

			//warpPerspective(haikei1, img1, homography1, img1.size());
			//warpPerspective(haikei2, img2, homography2, img2.size());
			//cv::imshow("haikei", img1);
			cv::imshow("haikei2", haikei2);

		}



	}
    EdsTerminateSDK();

}