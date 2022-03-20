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

using namespace cv;
using namespace std;
using namespace aruco;

#define NUMBER 1

// 0 Mandrill | 1 Parrot | 2 house | 3 boart | 4 airplane | 5 green 
#define NAM 1

#define Scale 10
#define WIDTH 1280
#define HEIGHT 800
#define CAMWIDTH 6960
#define CAMHEIGHT 4640
//
//#define X_MIN 3500
//#define Y_MIN 950
//#define X_MAX 4200
//#define Y_MAX 1400

int main() {

	string filename;
	string filename_Pro;
	string filename_Pro_gamma;

	Mat img, img2;

	/*Mat T_Pro = Mat(CAMHEIGHT, CAMWIDTH, CV_8UC3);
	Mat T_Pro_gamma = Mat(CAMHEIGHT, CAMWIDTH, CV_8UC3);*/

	Mat T_Pro = Mat(HEIGHT, WIDTH, CV_8UC3);
	Mat T_Pro_gamma = Mat(HEIGHT, WIDTH, CV_8UC3);

	std::vector<cv::Mat> Color;
	vector<cv::Mat> Splits;

	// ガンマ補正後のピクセル値
	double Pro_gamma_blue = 0;
	double Pro_gamma_red = 0;
	double Pro_gamma_green = 0;

	// ガンマ値
	//double Pro_gamma[3] = {
	//	2.629834686432756, 2.055648716795304 ,2.3887616929430098
	//};
	/*double Pro_gamma[3] = {
		 2.2939065256146187, 2.8331498411272342, 2.6692270393531428
	};*/


	//自分で測定
	double Pro_gamma[3] = {
	2.1423118282391296, 2.198967693021996, 2.1835081537225856
	};

	////やまけんさん
	//double Pro_gamma[3] = {
	//	2.7649459370657556, 2.672060594298085, 2.698057072061991
	//};


	//double Pro_gamma[3] = {
	//	2.1, 2.1, 2.1
	//};

	if (NAM == 9) {
		filename = "red.png";
		cv::Mat red_img(cv::Size(255, 255), CV_8UC3, cv::Scalar(0, 0, 255));
		imwrite(filename, red_img);
		filename_Pro = "red_Pro" + to_string(NUMBER) + ".jpg";
		filename_Pro_gamma = "中間発表projected/3mm_red.png";

	}
	if (NAM == 5) {
		filename = "blue.png";
		cv::Mat red_img(cv::Size(255, 255), CV_8UC3, cv::Scalar(255, 0, 0));
		imwrite(filename, red_img);
		filename_Pro = "red_Pro" + to_string(NUMBER) + ".jpg";
		filename_Pro_gamma = "中間発表projected/3mm_blue.png";

	}
	if (NAM == 6) {
		filename = "purple.png";
		cv::Mat red_img(cv::Size(255, 255), CV_8UC3, cv::Scalar(210, 55, 210));
		imwrite(filename, red_img);
		filename_Pro = "red_Pro" + to_string(NUMBER) + ".jpg";
		filename_Pro_gamma = "中間発表projected/3mm_purple.png";

	}
	if (NAM == 7) {
		filename = "green.png";
		cv::Mat red_img(cv::Size(255, 255), CV_8UC3, cv::Scalar(47, 105, 72));
		imwrite(filename, red_img);
		filename_Pro = "red_Pro" + to_string(NUMBER) + ".jpg";
		filename_Pro_gamma = "中間発表projected/0.6mm_green.png";

	}
	if (NAM == 8) {
		filename = "gray.jpg";
		cv::Mat red_img(cv::Size(1280, 800), CV_8UC3, cv::Scalar(125, 125, 125));
		imwrite(filename, red_img);
		filename_Pro = "gray_Pro" + to_string(NUMBER) + ".jpg";
		filename_Pro_gamma = "projected/2gray_Pro_gamma.jpg";
	}

	// 各手ファイル名入手
	if (NAM == 0) {
		filename = "Mandrill.png";
		filename_Pro = "projected/Mandrill_Pro" + to_string(NUMBER) + ".jpg";
		filename_Pro_gamma = "exp2_projected/or_2.4mm_Mandrill.png";
	}
	else if (NAM == 1) {
		filename = "Parrots.png";
		filename_Pro = "projected/Parrots_Pro.jpg";
		filename_Pro_gamma = "new_exp/parrots_for_projection.png";
	}

	else if (NAM == 2) {
		filename = "house.jpg";
		filename_Pro = "house_Pro.jpg";
		filename_Pro_gamma = "house_Pro_gamma.jpg";
	}
	else if (NAM == 3) {
		filename = "boart.jpg";
		filename_Pro = "boart_Pro.jpg";
		filename_Pro_gamma = "boart_Pro_gamma.jpg";
	}
	else if (NAM == 4) {
		filename = "airplane.jpg";
		filename_Pro = "airplane_Pro.jpg";
		filename_Pro_gamma = "airplane_Pro_gamma.jpg";
	}
	
	img2 = imread(filename);
	

	//for (int y = 0; y < img2.rows; y++) {
	//	for (int x = 0; x < img2.cols; x++) {
	//		if (y < 15 || y > 240 || x < 15 || x > 240) {
	//			img2.at<cv::Vec3b>(y, x)[0] = 0;//Blue
	//			img2.at<cv::Vec3b>(y, x)[1] = 0;//Redc
	//			img2.at<cv::Vec3b>(y, x)[2] = 0; //Blue
	//		}
	//	}
	//}
	cv::resize(img2, img2, Size(1280, 800));
	//cv::resize(img, img, Size(512, 512));
	//cv::resize(img, img, Size(1280, 800));
	
	//for (int y = 0; y < img2.rows; y++) {
	//	for (int x = 0; x < img2.cols; x++) {
	//		img2.at<cv::Vec3b>(y, x)[0] *= 0.7;//Blue
	//		img2.at<cv::Vec3b>(y, x)[1] *= 0.7;//Redc
	//		img2.at<cv::Vec3b>(y, x)[2] *= 0.7; //Blue
	//	}
	//}
	//

	imwrite("new_exp/_" + filename, img2);
	cv::waitKey(500);



	// 黒投影時の輝度値
	/*Mat E = (cv::Mat_<double>(3, 1) <<
		14.503784, 16.717602, 24.107334
		);*/
	Mat E = (cv::Mat_<double>(3, 1) <<
		0.002122, 0.004863, 0.003486
		);


	Mat I = (cv::Mat_<double>(3, 3) <<
		1.0, 0, 0,
		0, 1.0, 0,
		0, 0, 1.0
		);

	Mat T_keisan = (cv::Mat_<double>(3, 1) <<
		1, 1, 1
		);

	Mat T_keisan_Pro = (cv::Mat_<double>(3, 1) <<
		1, 1, 1
		);

	Mat K_Pro = (cv::Mat_<double>(3, 3) <<
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
		);

	Mat P1 = (cv::Mat_<double>(3, 1) <<
		1, 1, 1
		);

	cv::Mat K(9, 1, CV_64F);
	cv::Mat Cam(3 * 64, 1, CV_64F);
	cv::Mat Pro(3 * 64, 9, CV_64F);

	Pro = 0;

	int Count = 0;

	// キャリブレーション画像投影
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int m = 0; m < 4; m++) {

				for (int l = 0; l < 3; l++) {
					Pro.at<double>(3 * Count + l, 3 * l) = (double)(50 * i);
					Pro.at<double>(3 * Count + l, 3 * l + 1) = (double)(50 * j);
					Pro.at<double>(3 * Count + l, 3 * l + 2) = (double)(50 * m);
				}

				string filename_cap = "color/Color_2_" + to_string(Count) + ".jpg";
				img = imread(filename_cap);
				Color.push_back(img);
				Count++;
			}
		}
	}


	std::cout << Pro << std::endl;

	Mat PP = Pro.t() * Pro;
	Mat P = PP.inv(cv::DECOMP_SVD) * Pro.t();

	std::cout << P << std::endl;



	printf("\n---------- start ----------\n\n");

	Mat p2c = imread("P2C_finish.png");

	int cx = 0;
	int cy = 0;
	/*for (int i = X_MIN; i < X_MAX; i++) {
		for (int j = Y_MIN; j < Y_MAX; j++) {*/
	for (int i = 35; i < 1000; i++) {
		for (int j = 60; j < 770; j++) {
			for (int k = 0; k < 64; k++) {
				cy = p2c.at<cv::Vec3b>(j, i)[1] / 255.0 * 4640;//g
				cx = p2c.at<cv::Vec3b>(j, i)[2] / 255.0 * 6960;//r
				/*std::cout << "cx" << cx << std::endl;
				std::cout << "cy" << cy << std::endl;*/
				//std::cout << "ここまで" << std::endl;
				//プロジェクタの座標に対応するカメラ座標を（j，i）に入れる．
				Cam.at<double>(3 * k, 0) = (double)Color[k].at<cv::Vec3b>(cy, cx)[0] - E.at<double>(0);
				Cam.at<double>(3 * k + 1, 0) = (double)Color[k].at<cv::Vec3b>(cy, cx)[1] - E.at<double>(1);
				Cam.at<double>(3 * k + 2, 0) = (double)Color[k].at<cv::Vec3b>(cy, cx)[2] - E.at<double>(2);
			}

			K = P * Cam;

			K_Pro.at<double>(0) = K.at<double>(0);
			K_Pro.at<double>(1) = K.at<double>(1);
			K_Pro.at<double>(2) = K.at<double>(2);
			K_Pro.at<double>(3) = K.at<double>(3);
			K_Pro.at<double>(4) = K.at<double>(4);
			K_Pro.at<double>(5) = K.at<double>(5);
			K_Pro.at<double>(6) = K.at<double>(6);
			K_Pro.at<double>(7) = K.at<double>(7);
			K_Pro.at<double>(8) = K.at<double>(8);

			//*****img2から環境光の部分を引く
			T_keisan.at<double>(0) = img2.at<cv::Vec3b>(j, i)[0];
			T_keisan.at<double>(1) = img2.at<cv::Vec3b>(j, i)[1];
			T_keisan.at<double>(2) = img2.at<cv::Vec3b>(j, i)[2];
			//T_keisan.at<double>(0) = img2.at<cv::Vec3b>(cy, cx)[0];
			//T_keisan.at<double>(1) = img2.at<cv::Vec3b>(cy, cx)[1];
			//T_keisan.at<double>(2) = img2.at<cv::Vec3b>(cy, cx)[2];
			P1 = K_Pro.inv() * T_keisan;

			//std::cout << P1 << std::endl;

			if ((0 <= (int)P1.at<double>(0)) && ((int)P1.at<double>(0) <= 255)) {
				T_Pro.at<cv::Vec3b>(j, i)[0] = (int)P1.at<double>(0);
			}
			else if ((int)P1.at<double>(0) > 255) {
				T_Pro.at<cv::Vec3b>(j, i)[0] = 255;
			}
			else if ((int)P1.at<double>(0) < 0) {
				T_Pro.at<cv::Vec3b>(j, i)[0] = 0;
			}

			if ((0 <= (int)P1.at<double>(1)) && ((int)P1.at<double>(1) <= 255)) {
				T_Pro.at<cv::Vec3b>(j, i)[1] = (int)P1.at<double>(1);
			}
			else if ((int)P1.at<double>(1) > 255) {
				T_Pro.at<cv::Vec3b>(j, i)[1] = 255;
			}
			else if ((int)P1.at<double>(1) < 0) {
				T_Pro.at<cv::Vec3b>(j, i)[1] = 0;
			}

			if ((0 <= (int)P1.at<double>(2)) && ((int)P1.at<double>(2) <= 255)) {
				T_Pro.at<cv::Vec3b>(j, i)[2] = (int)P1.at<double>(2);
			}
			else if ((int)P1.at<double>(2) > 255) {
				T_Pro.at<cv::Vec3b>(j, i)[2] = 255;
			}
			else if ((int)P1.at<double>(2) < 0) {
				T_Pro.at<cv::Vec3b>(j, i)[2] = 0;
			}



			Pro_gamma_blue = pow((T_Pro.at<cv::Vec3b>(j, i)[0] / (double)255), ((double)1 / Pro_gamma[0])) * (double)255;
			Pro_gamma_green = pow((T_Pro.at<cv::Vec3b>(j, i)[1] / (double)255), ((double)1 / Pro_gamma[1])) * (double)255;
			Pro_gamma_red = pow((T_Pro.at<cv::Vec3b>(j, i)[2] / (double)255), ((double)1 / Pro_gamma[2])) * (double)255;

			if ((0 <= (int)Pro_gamma_blue) && ((int)Pro_gamma_blue <= 255)) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[0] = (int)Pro_gamma_blue;
			}
			else if ((int)Pro_gamma_blue > 255) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[0] = 255;
			}
			else if ((int)Pro_gamma_blue < 0) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[0] = 0;
			}

			if ((0 <= (int)Pro_gamma_green) && ((int)Pro_gamma_green <= 255)) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[1] = (int)Pro_gamma_green;
			}
			else if ((int)Pro_gamma_green > 255) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[1] = 255;
			}
			else if ((int)Pro_gamma_green < 0) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[1] = 0;
			}

			if ((0 <= (int)Pro_gamma_red) && ((int)Pro_gamma_red <= 255)) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[2] = (int)Pro_gamma_red;
			}
			else if ((int)Pro_gamma_red > 255) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[2] = 255;
			}
			else if ((int)Pro_gamma_red < 0) {
				T_Pro_gamma.at<cv::Vec3b>(j, i)[2] = 0;
			}



		}

		printf("x | %d\n", i);

	}

	//imwrite("down_" + filename_Pro, T_Pro);
	//imwrite(filename_Pro, T_Pro);
	//imwrite("down_" + filename_Pro_gamma, T_Pro_gamma);
	imwrite(filename_Pro_gamma, T_Pro_gamma);

	printf("\ncomplete!!!!!\n");

	//cv::imshow("Pro", T_Pro);
	//cv::imshow("Pro2", T_Pro2);
	//cv::waitKey(0);

}