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
#define CAMHEIGHT 3904

#define X_MIN 3725
#define Y_MIN 2065
#define X_MAX 5053
#define Y_MAX 2607

int main() {
    float wb_b = 1.5;
    float wb_g = 0.9;
    float wb_r = 1.1;

    Mat T_Pro, img;
    /*double Pro_gamma[3] = {
    2.1893678412980857, 2.930377913390212, 2.5357228292199276
    };*/
    //Ž©•ª‚Å‘ª’è
    double Pro_gamma[3] = {
        1.6792987955471081, 2.130850355920784, 1.7984286725215164
    };

    double Pro_gamma_blue = 0;
    double Pro_gamma_red = 0;
    double Pro_gamma_green = 0;

    Mat resize_gamma = Mat(HEIGHT, WIDTH, CV_8UC3);
  //  T_Pro = imread("Parrots_pro_gamma.jpg");
  //  //T_Pro = imread("resizegamma.jpg");

  //  cv::resize(T_Pro, T_Pro, Size(1280, 800));
  ///*  EdsError err;

 
    Mat resize = imread("’†ŠÔ”­•\projected/_green.png");
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
    imwrite("’†ŠÔ”­•\projected/gamma_green.png", resize_gamma);




}
