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
#define NAM 7

#define Scale 10
#define WIDTH 1280
#define HEIGHT 800
#define CAMWIDTH 5184
#define CAMHEIGHT 3456

#define X_MIN 3500
#define Y_MIN 950
#define X_MAX 4200
#define Y_MAX 1400

int main() {

	double Pro_gamma[3] = {
		2.7649459370657556, 2.672060594298085, 2.698057072061991
	};

	cv::namedWindow("Pro", cv::WINDOW_NORMAL);
	cv::resizeWindow("Pro", WIDTH, HEIGHT);
	cv::moveWindow("Pro", 1920, 0);
	cv::setWindowProperty("Pro", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

	cv::Mat color(cv::Size(1280, 800), CV_8UC3, cv::Scalar(235, 80, 33));
	Mat color_gamma = Mat(HEIGHT, WIDTH, CV_8UC3);
	double Pro_gamma_blue = 0;
	double Pro_gamma_red = 0;
	double Pro_gamma_green = 0;

	for (int i = 0; i < 1280; i++) {
		for (int j = 0; j < 800; j++) {





			Pro_gamma_blue = pow((color.at<cv::Vec3b>(j, i)[0] / (double)255.0), ((double)1 / Pro_gamma[0])) * (double)255;
			Pro_gamma_green = pow((color.at<cv::Vec3b>(j, i)[1] / (double)255.0), ((double)1 / Pro_gamma[1])) * (double)255;
			Pro_gamma_red = pow((color.at<cv::Vec3b>(j, i)[2] / (double)255), ((double)1 / Pro_gamma[2])) * (double)255;

			if ((0 <= (int)Pro_gamma_blue) && ((int)Pro_gamma_blue <= 255)) {
				color_gamma.at<cv::Vec3b>(j, i)[0] = (int)Pro_gamma_blue;
			}
			else if ((int)Pro_gamma_blue > 255) {
				color_gamma.at<cv::Vec3b>(j, i)[0] = 255;
			}
			else if ((int)Pro_gamma_blue < 0) {
				color_gamma.at<cv::Vec3b>(j, i)[0] = 0;
			}

			if ((0 <= (int)Pro_gamma_green) && ((int)Pro_gamma_green <= 255)) {
				color_gamma.at<cv::Vec3b>(j, i)[1] = (int)Pro_gamma_green;
			}
			else if ((int)Pro_gamma_green > 255) {
				color_gamma.at<cv::Vec3b>(j, i)[1] = 255;
			}
			else if ((int)Pro_gamma_green < 0) {
				color_gamma.at<cv::Vec3b>(j, i)[1] = 0;
			}

			if ((0 <= (int)Pro_gamma_red) && ((int)Pro_gamma_red <= 255)) {
				color_gamma.at<cv::Vec3b>(j, i)[2] = (int)Pro_gamma_red;
			}
			else if ((int)Pro_gamma_red > 255) {
				color_gamma.at<cv::Vec3b>(j, i)[2] = 255;
			}
			else if ((int)Pro_gamma_red < 0) {
				color_gamma.at<cv::Vec3b>(j, i)[2] = 0;
			}



		}

	}



	//Mat src1 = Mat::ones(100, 160, CV_8U) * 255;
	imshow("Pro", color);
	cv::waitKey(0);

}