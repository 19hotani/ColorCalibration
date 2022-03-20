#include <opencv2/opencv.hpp>

using namespace cv;
int main(void)
{

	cv::namedWindow("Pro", cv::WINDOW_NORMAL);
	cv::resizeWindow("Pro", 800, 1280);
	cv::moveWindow("Pro", 1920, 0);
	cv::setWindowProperty("Pro", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);


	// 要素がすべて 0 の width160, height100 の画像
	//Mat image = Mat::zeros(cv::Size(1280, 800), CV_8UC3);

	

	// 画像を格納するオブジェクトを宣言する
	// # CV_8UC3 : 8bit 3channel フルカラー画像
	//cv::Mat	image(400, 400, CV_8UC3);
	cv::Mat	image(800, 1280, CV_8UC3);


	// 画像を白色で塗りつぶす
	//image = cv::Scalar(255, 255, 255);
	image = cv::Scalar(0, 0, 0);


	//// 四角形を描く 
	//// # rectangle(画像, 頂点1, 頂点2, 色, 線幅, 連結)
	//// # 頂点1,2を対角とし、傾きのない四角形を描く
	//cv::Point p1(10, 10), p2(190, 90);
	//cv::rectangle(image, p1, p2, cv::Scalar(255, 0, 0), 1, cv::LINE_4);


	//// 四角形を描く 2
	//// # rectangle(画像, 四角形, 色, 線幅, 連結)
	//// # cv::Rect(始点x, 始点y, 幅, 高さ) を基にして、傾きのない四角形を描く
	//cv::Rect rect(210, 10, 180, 80);
	//cv::rectangle(image, rect, cv::Scalar(0, 0, 255), 1, cv::LINE_4);


	//for (int i = 0; i < 13; i++) {
	//	cv::Rect rect(i * 30 + 10, 100, 20, 80);
	//	// # 線幅 < 0 のときは四角形内を塗りつぶす
	//	cv::rectangle(image, rect, cv::Scalar(255 - i * 20, 0, i * 20 + 15), -1, cv::LINE_4);
	//}


	//for (int i = 1; i < 7; i++) {
	//	// 正円を描く
	//	//// # circle(画像, 中心座標, 半径, 色, 線幅, 連結)
	//	//cv::circle(image, cv::Point(i * i * 9 + 10, 250), i * 9 - 1, cv::Scalar(0, 200, 0), 1, cv::LINE_AA);
	//	// # 線幅 < 0 のときは円内を塗りつぶす
	//	cv::circle(image, cv::Point(340, 340), 60, cv::Scalar(285, 80, 33), -1, cv::LINE_AA);
	//}
	
	cv::circle(image, cv::Point(640, 545), 70, cv::Scalar(285, 80, 33), -1, cv::LINE_AA);
	//circle(image, Point(640, 400), 100, cv::Scalar(0, 0, 0), -1, cv::LINE_AA);
	imwrite("thesis_pro.jpg",image);
	cv::imshow("Pro", image);
	cv::waitKey();

	return 0;
}