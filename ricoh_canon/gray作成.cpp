#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>

int main()
{
    const cv::Size im_size(1280, 800);

    std::vector<cv::Mat> im_vec;
    std::vector<int> graycode(im_size.width);

    const int bit_num = static_cast<int>(std::ceil(std::log2(im_size.width))) - 1;
    const int offset = static_cast<int>(std::pow(2, bit_num) - im_size.width) / 2;

    // generate gray code
    for (int px = 0; px < im_size.width; ++px)
    {
        // shift to center
        const int bit = px + offset;

        // gray code
        graycode[px] = bit ^ (bit >> 1);
    }

    // make images
    for (int i = 0; i < bit_num; ++i)
    {
        cv::Mat im = cv::Mat::zeros(im_size, CV_8U);

        for (int py = 0; py < im_size.height; ++py)
        {
            uchar* const im_ptr = im.ptr<uchar>(py);

            for (int px = 0; px < im_size.width; ++px)
            {
                // extract i-th bit
                const uchar val = ((graycode[px] >> (bit_num - i - 1)) & 0x01);
                im_ptr[px] = val * 255;
            }
        }
        im_vec.push_back(im.clone());
    }

    // show
    int count = 0;
    for (const auto& im : im_vec)
    {
        cv::imshow("im", im);
        cv::imwrite("pattern/gray" + std::to_string(count++) + ".jpg", im);
        cv::waitKey(-1);
    }
}