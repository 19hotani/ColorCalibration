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
double getPSNR(const Mat& I1, const Mat& I2)
{
	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2

	Scalar s = sum(s1);         // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if (sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double  mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0 * log10((255 * 255) / mse);
		return psnr;
	}
}

Scalar getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = (0.01 * 255) * (0.01 * 255), C2 = (0.03 * 255) * (0.03 * 255);
	/***************************** INITS **********************************/
	int d = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d);           // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	Mat I2_2 = I2.mul(I2);        // I2^2
	Mat I1_2 = I1.mul(I1);        // I1^2
	Mat I1_I2 = I1.mul(I2);        // I1 * I2

	/***********************PRELIMINARY COMPUTING ******************************/

	Mat mu1, mu2;   //
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);

	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// FORMULA ////////////////////////////////
	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	return mssim;
}


int main(void) {
	cv::Mat result1, result2;
	cv::Mat target;
	target = imread("exp2_kido_after/edit_trim_Mandrill.png");
	resize(target, target, cv::Size(1280, 800));

	//for (int y = 0; y < target.rows; y++) {
	//	for (int x = 0; x < target.cols; x++) {
	//		target.at<cv::Vec3b>(y, x)[0] *= 0.7;//Blue
	//		target.at<cv::Vec3b>(y, x)[1] *= 0.7;//Redc
	//		target.at<cv::Vec3b>(y, x)[2] *= 0.7; //Blue
	//	}
	//}
	//imwrite("ホモグラフィ/trim_Mandrill.png", target);

	//target = imread("中間発表projected/down_resize_Mandrill.png");
	result1 = imread("exp2_kido_after/edit_0.6mm_Mandrill_1.png");
	result2 = imread("exp2_kido_after/edit_0.6mm_Mandrill_2.png");

	//capture
	std::vector<cv::Point2f> src_pt1(4);//補正前
	std::vector<cv::Point2f> src_pt2(4);//補正後

	//target
	std::vector<cv::Point2f> dst_pt(4);

	//カメラ座標
	src_pt1[0].x = 2075;
	src_pt1[0].y = 1296;
	src_pt1[1].x = 3248;
	src_pt1[1].y = 1259;
	src_pt1[2].x = 2080;
	src_pt1[2].y = 2059;
	src_pt1[3].x = 3248;
	src_pt1[3].y = 2049;


	//src_pt1[0].x = src_pt2[0].x;
	//src_pt1[0].y = src_pt2[0].y;
	//src_pt1[1].x = src_pt2[1].x;
	//src_pt1[1].y = src_pt2[1].y;
	//src_pt1[2].x = src_pt2[2].x;
	//src_pt1[2].y = src_pt2[2].y;
	//src_pt1[3].x = src_pt2[3].x;
	//src_pt1[3].y = src_pt2[3].y;


	//プロジェクタ座標
	/*dst_pt[0].x = 74;
	dst_pt[0].y = 46;
	dst_pt[1].x = 1205;
	dst_pt[1].y = 46;
	dst_pt[2].x = 74;
	dst_pt[2].y = 753;
	dst_pt[3].x = 1295;
	dst_pt[3].y = 753;*/

	/*dst_pt[0].x = 75;
	dst_pt[0].y = 42;
	dst_pt[1].x = 1205;
	dst_pt[1].y = 42;
	dst_pt[2].x = 75;
	dst_pt[2].y = 677;
	dst_pt[3].x = 1205;
	dst_pt[3].y = 677;*/

	dst_pt[0].x = 0;
	dst_pt[0].y = 0;
	dst_pt[1].x = 1280;
	dst_pt[1].y = 0;
	dst_pt[2].x = 0;
	dst_pt[2].y = 800;
	dst_pt[3].x = 1280;
	dst_pt[3].y = 800;

	const cv::Mat homography_matrix1 = findHomography(src_pt1, dst_pt);

	std::cout << homography_matrix1 << std::endl;

	//const cv::Mat homography_matrix2 = findHomography(src_pt2, dst_pt);

	//std::cout << homography_matrix2 << std::endl;

	/*cv::Mat pro1 = Mat::zeros(cv::Size(1280, 800), CV_8U);
	cv::Mat pro2 = Mat::zeros(cv::Size(1280, 800), CV_8U);*/
	cv::Mat pro1 = Mat::zeros(cv::Size(1280, 800), CV_8U);
	cv::Mat pro2 = Mat::zeros(cv::Size(1280, 800), CV_8U);
	warpPerspective(result1, pro1, homography_matrix1, pro1.size());
	warpPerspective(result2, pro2, homography_matrix1, pro2.size());

	//imshow("before", pro1);
	imwrite("ホモグラフィ/1.8mm_Mandrill_前_homography.png", pro1);
	//imshow("after", pro2);
	imwrite("ホモグラフィ/1.8mm_Mandrill_後_homography.png", pro2);
	//imwrite("target.png", target);
	//waitKey(0);
	double psnr1, psnr2;
	psnr1 = PSNR(target, pro1);
	psnr2 = PSNR(target, pro2);

	std::cout << "補正前psnr:" << psnr1 << std::endl;
	std::cout << "補正後psnr:" << psnr2 << std::endl;

	Scalar ssim1 = getMSSIM(target, pro1);
	double mssim1 = (ssim1[0] + ssim1[1] + ssim1[2]) / 3;

	Scalar ssim2 = getMSSIM(target, pro2);
	double mssim2 = (ssim2[0] + ssim2[1] + ssim2[2]) / 3;

	std::cout << "補正前ssim:" << mssim1 << std::endl;
	std::cout << "補正後ssim:" << mssim2 << std::endl;
}