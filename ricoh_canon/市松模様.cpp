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


    

    //カラー画像の作成
    Mat img6 = Mat::zeros(Size(W, H), CV_8UC3);
    Mat img7 = Mat::zeros(Size(W, H), CV_8UC3);


   
   



    
    //これは「深い」コピーになる。
    //したがって、img6 に対する操作は img2 には全く反映されない。


    ////画素に対する直接操作（グレースケール画像の場合）
    ////市松模様にしてみる
    //for (int y = 0; y < img6.rows; ++y)
    //{
    //    for (int x = 0; x < img6.cols; ++x)
    //    {
    //        img6.at<unsigned char>(y, x)
    //            = 255 * ((x / 10 + y / 10) % 2);
    //        // (y,x) という順であることに注意する。
    //    }
    //}


    //画素に対する直接操作（カラー画像の場合）
    //一列毎に色が変わる市松模様にしてみる
    
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
              // (y,x) という順であることに注意する。
        }
    }
    cv::imshow("チェッカー", img7);
    

    ////画像の「原点」は左上である；
    ////つまりx軸は左から右へ、y軸は上から下に走っている。
    //circle(img6, Point(0, 0), 100
    //    , Scalar(0, 250, 250), 5, CV_AA);
    //circle(img7, Point(0, 0), 100
    //    , Scalar(0, 250, 250), 5, CV_AA);


    //画像の保存：拡張子に応じた形式で保存される。
    //imwrite("img6.png", img6);
    imwrite("pattern/チェックr.png", img7);
    cv::waitKey(0);
    //imwrite("img7.jpg", img7);
    //imwrite("img7.bmp", img7);

    //文字列への書き出し
    //(検証しやすくするため小さめの画像でやる)
    /*Mat img8 = Mat::zeros(Size(20, 15), CV_8UC1);
    for (int y = 0; y < img8.rows; ++y)
    {
        for (int x = 0; x < img8.cols; ++x)
        {
            img8.at<unsigned char>(y, x) = 255 * ((x + y) % 2);
        }
    }*/

    
    ////C言語のデータとして使える形式で書き出し
    //ostringstream C_out;
    //C_out << cv::format(img8, "C") << flush;
    //cout << "C:" << C_out.str() << endl;

    ////CSV形式で書き出し
    //ostringstream CSV_out;
    //CSV_out << cv::format(img8, "csv") << flush;
    //cout << "CSV:" << CSV_out.str() << endl;

    ////Pythonのデータとして使える形式で書き出し
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