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
	target = imread("exp2_kido_after/edit_trim_Parrots.png");
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
	result1 = imread("exp2_kido_after/edit_1.8mm_Parrots_1.png");
	result2 = imread("exp2_kido_after/edit_1.8mm_Parrots_2.png");

	
	double psnr1, psnr2;
	psnr1 = PSNR(target, result1);
	psnr2 = PSNR(target, result2);

	std::cout << "補正前psnr:" << psnr1 << std::endl;
	std::cout << "補正後psnr:" << psnr2 << std::endl;

	Scalar ssim1 = getMSSIM(target, result1);
	double mssim1 = (ssim1[0] + ssim1[1] + ssim1[2]) / 3;

	Scalar ssim2 = getMSSIM(target, result2);
	double mssim2 = (ssim2[0] + ssim2[1] + ssim2[2]) / 3;

	std::cout << "補正前ssim:" << mssim1 << std::endl;
	std::cout << "補正後ssim:" << mssim2 << std::endl;
}