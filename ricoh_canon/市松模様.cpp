//OpenCV2 image handling
/*#include <opencv2/cv.h>
#include <highgui.h>  */  
#include <opencv2/opencv.hpp>
#include <iostream>
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

#define image_path_string "madoka.png"
//#define W 1280
#define W 1920
//#define H 720
#define H 1080


#define IMAGESHOW(X) do{  \
  cvNamedWindow(#X);      \
  imshow(#X, X);          \
  }while(0)

using namespace cv;

int main()
{
    using namespace std;
    using namespace cv;


    

    //�J���[�摜�̍쐬
    Mat img6 = Mat::zeros(Size(W, H), CV_8UC3);
    Mat img7 = Mat::zeros(Size(W, H), CV_8UC3);


   
   



    
    //����́u�[���v�R�s�[�ɂȂ�B
    //���������āAimg6 �ɑ΂��鑀��� img2 �ɂ͑S�����f����Ȃ��B


    ////��f�ɑ΂��钼�ڑ���i�O���[�X�P�[���摜�̏ꍇ�j
    ////�s���͗l�ɂ��Ă݂�
    //for (int y = 0; y < img6.rows; ++y)
    //{
    //    for (int x = 0; x < img6.cols; ++x)
    //    {
    //        img6.at<unsigned char>(y, x)
    //            = 255 * ((x / 10 + y / 10) % 2);
    //        // (y,x) �Ƃ������ł��邱�Ƃɒ��ӂ���B
    //    }
    //}


    //��f�ɑ΂��钼�ڑ���i�J���[�摜�̏ꍇ�j
    //��񖈂ɐF���ς��s���͗l�ɂ��Ă݂�
    
    for (int y = 0; y < img7.rows; ++y)
    {
        for (int x = 0; x < img7.cols; ++x)
        {
           /* if ((y / 10) % 3 == 0) {
                img7.at<Vec3b>(y, x)[2] = 255 * ((x / 10 + y / 10) % 2);
            }
            if ((y / 10) % 3 == 1) {
                img7.at<Vec3b>(y, x)[2] = 255 * ((x / 10 + y / 10) % 2);
            }
            if ((y / 10) % 3 == 2) {
                img7.at<Vec3b>(y, x)[2] = 255 * ((x / 10 + y / 10) % 2);
            }*/
            if ((x / 40 + y / 40) % 2) {
                img7.at<Vec3b>(y, x)[0] = 205;
                img7.at<Vec3b>(y, x)[1] = 225;
                img7.at<Vec3b>(y, x)[2] = 250;
            }
            else {
                img7.at<Vec3b>(y, x)[0] = 213;
                img7.at<Vec3b>(y, x)[1] = 203;
                img7.at<Vec3b>(y, x)[2] = 250;
            }
            //img7.at<Vec3b>(y, x)[0] = 255 * ();
            //img7.at<Vec3b>(y, x)[1] = 255 * ((x / 80 + y / 80) % 2);
            //img7.at<Vec3b>(y, x)[2] = 230 * ((x / 80 + y / 80) % 2);

            //img7.at<Vec3b>(y, x)[0] = 255 * ((x / 80 + y / 80) % 2);
            //img7.at<Vec3b>(y, x)[1] = 255 * ((x / 80 + y / 80) % 2);
            //img7.at<Vec3b>(y, x)[2] = 230 * ((x / 80 + y / 80) % 2);
            ////img7.at<Vec3b>(y, x)[0] = 130 * ((x / 10 + y / 10 + 1) % 2);


            //img7.at<Vec3b>(y, x)[0] = ((y / 10) % 3 == 0) ? 255 * ((x / 10 + y / 10) % 2) : 0;//B
            //img7.at<Vec3b>(y, x)[2] = ((y / 10) % 3 == 1) ? 255 * ((x / 10 + y / 10) % 2) : 0;//G
            //img7.at<Vec3b>(y, x)[2] = ((y / 10) % 3 == 2) ? 255 * ((x / 10 + y / 10) % 2) : 0;//R
            //img7.at<Vec3b>(y, x)[2] = ((y / 100) % 3 == 0) ? 255 * ((x / 100 + y / 100) % 2) : 0;//B
            //img7.at<Vec3b>(y, x)[2] = ((y / 100) % 3 == 1) ? 255 * ((x / 100 + y / 100) % 2) : 0;//G
            //img7.at<Vec3b>(y, x)[2] = ((y / 100) % 3 == 2) ? 255 * ((x / 100 + y / 100) % 2) : 0;//
              // (y,x) �Ƃ������ł��邱�Ƃɒ��ӂ���B
        }
    }
    cv::imshow("�`�F�b�J�[", img7);
    

    ////�摜�́u���_�v�͍���ł���G
    ////�܂�x���͍�����E�ցAy���͏ォ�牺�ɑ����Ă���B
    //circle(img6, Point(0, 0), 100
    //    , Scalar(0, 250, 250), 5, CV_AA);
    //circle(img7, Point(0, 0), 100
    //    , Scalar(0, 250, 250), 5, CV_AA);


    //�摜�̕ۑ��F�g���q�ɉ������`���ŕۑ������B
    //imwrite("img6.png", img6);
    imwrite("pattern/�`�F�b�Nr.png", img7);
    cv::waitKey(0);
    //imwrite("img7.jpg", img7);
    //imwrite("img7.bmp", img7);

    //������ւ̏����o��
    //(���؂��₷�����邽�ߏ����߂̉摜�ł��)
    /*Mat img8 = Mat::zeros(Size(20, 15), CV_8UC1);
    for (int y = 0; y < img8.rows; ++y)
    {
        for (int x = 0; x < img8.cols; ++x)
        {
            img8.at<unsigned char>(y, x) = 255 * ((x + y) % 2);
        }
    }*/

    
    ////C����̃f�[�^�Ƃ��Ďg����`���ŏ����o��
    //ostringstream C_out;
    //C_out << cv::format(img8, "C") << flush;
    //cout << "C:" << C_out.str() << endl;

    ////CSV�`���ŏ����o��
    //ostringstream CSV_out;
    //CSV_out << cv::format(img8, "csv") << flush;
    //cout << "CSV:" << CSV_out.str() << endl;

    ////Python�̃f�[�^�Ƃ��Ďg����`���ŏ����o��
    //ostringstream Python_out;
    //Python_out << cv::format(img8, "python") << flush;
    //cout << "Python:" << Python_out.str() << endl;

    //IMAGESHOW(img1);
    //IMAGESHOW(img2);
    //IMAGESHOW(img2_color);
    //IMAGESHOW(img3);
    //IMAGESHOW(img4);
    //IMAGESHOW(img5);
    //IMAGESHOW(img6);
    //IMAGESHOW(img7);

    cout << "hit any key on some image window" << endl;
    cv::waitKey(0);

    return 0;
}