#include <opencv2/opencv.hpp>

using namespace cv;
int main(void)
{

	cv::namedWindow("Pro", cv::WINDOW_NORMAL);
	cv::resizeWindow("Pro", 800, 1280);
	cv::moveWindow("Pro", 1920, 0);
	cv::setWindowProperty("Pro", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);


	// �v�f�����ׂ� 0 �� width160, height100 �̉摜
	//Mat image = Mat::zeros(cv::Size(1280, 800), CV_8UC3);

	

	// �摜���i�[����I�u�W�F�N�g��錾����
	// # CV_8UC3 : 8bit 3channel �t���J���[�摜
	//cv::Mat	image(400, 400, CV_8UC3);
	cv::Mat	image(800, 1280, CV_8UC3);


	// �摜�𔒐F�œh��Ԃ�
	//image = cv::Scalar(255, 255, 255);
	image = cv::Scalar(0, 0, 0);


	//// �l�p�`��`�� 
	//// # rectangle(�摜, ���_1, ���_2, �F, ����, �A��)
	//// # ���_1,2��Ίp�Ƃ��A�X���̂Ȃ��l�p�`��`��
	//cv::Point p1(10, 10), p2(190, 90);
	//cv::rectangle(image, p1, p2, cv::Scalar(255, 0, 0), 1, cv::LINE_4);


	//// �l�p�`��`�� 2
	//// # rectangle(�摜, �l�p�`, �F, ����, �A��)
	//// # cv::Rect(�n�_x, �n�_y, ��, ����) ����ɂ��āA�X���̂Ȃ��l�p�`��`��
	//cv::Rect rect(210, 10, 180, 80);
	//cv::rectangle(image, rect, cv::Scalar(0, 0, 255), 1, cv::LINE_4);


	//for (int i = 0; i < 13; i++) {
	//	cv::Rect rect(i * 30 + 10, 100, 20, 80);
	//	// # ���� < 0 �̂Ƃ��͎l�p�`����h��Ԃ�
	//	cv::rectangle(image, rect, cv::Scalar(255 - i * 20, 0, i * 20 + 15), -1, cv::LINE_4);
	//}


	//for (int i = 1; i < 7; i++) {
	//	// ���~��`��
	//	//// # circle(�摜, ���S���W, ���a, �F, ����, �A��)
	//	//cv::circle(image, cv::Point(i * i * 9 + 10, 250), i * 9 - 1, cv::Scalar(0, 200, 0), 1, cv::LINE_AA);
	//	// # ���� < 0 �̂Ƃ��͉~����h��Ԃ�
	//	cv::circle(image, cv::Point(340, 340), 60, cv::Scalar(285, 80, 33), -1, cv::LINE_AA);
	//}
	
	cv::circle(image, cv::Point(640, 545), 70, cv::Scalar(285, 80, 33), -1, cv::LINE_AA);
	//circle(image, Point(640, 400), 100, cv::Scalar(0, 0, 0), -1, cv::LINE_AA);
	imwrite("thesis_pro.jpg",image);
	cv::imshow("Pro", image);
	cv::waitKey();

	return 0;
}