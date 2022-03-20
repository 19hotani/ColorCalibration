#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "FlyCap2CVWrapper.h"
#include <opencv2/opencv.hpp>

#include <GL/glut.h>
#include <GL/freeglut.h>


using namespace std;
using namespace cv;

cv::Mat proj_img;
std::vector<cv::Vec6f> triangles;

int display_num = 0;



// @brief .csvファイルからプロジェクタ座標を読み込み、Subdiv2D型の変数に格納する
// @param file_name 読み込む.csvファイルの名前
// @param subdiv 座標格納用の空のSubdiv2D型変数
void csvreader(std::string file_name, cv::Subdiv2D& subdiv) {
    std::string str_buf;
    std::string str_conma_buf;
    std::string input_csv_file_path = file_name;

    // 読み込むcsvファイルを開く(std::ifstreamのコンストラクタで開く)
    std::ifstream ifs_csv_file(input_csv_file_path);

    // getline関数で1行ずつ読み込む(読み込んだ内容はstr_bufに格納)
    while (getline(ifs_csv_file, str_buf)) {
        int px, py;

        // 「,」区切りごとにデータを読み込むためにistringstream型にする
        std::istringstream i_stream(str_buf);

        getline(i_stream, str_conma_buf, ',');
        getline(i_stream, str_conma_buf, ',');
        getline(i_stream, str_conma_buf, ',');
        px = stoi(str_conma_buf);
        getline(i_stream, str_conma_buf, ',');
        py = stoi(str_conma_buf);
        cv::Point2f pt(px, py);

        subdiv.insert(pt);
    }
}


//template<typename Num_type>
//std::vector<std::vector<Num_type> >
//Getdata(std::string filename, std::size_t ignore_line_num = 0) {
//    std::ifstream reading_file;
//    reading_file.open(filename, std::ios::in);
//    if (!reading_file)
//        throw std::invalid_argument(filename + std::string("could not be opened."));
//
//    std::string reading_line_buffer;
//
//    //最初のignore_line_num行を空読みする
//    for (std::size_t line = 0; line < ignore_line_num; line++) {
//        getline(reading_file, reading_line_buffer);
//        if (reading_file.eof()) break;
//    }
//
//    Num_type num;
//    char comma;
//
//    std::vector<std::vector<Num_type> > data;
//
//    while (std::getline(reading_file, reading_line_buffer)) {
//        if (reading_line_buffer.size() == 0) break;
//        std::vector<Num_type> temp_data;
//        std::istringstream is(reading_line_buffer);
//        while (is >> num) {
//            temp_data.push_back(num);
//            is >> comma;
//        }
//        data.push_back(temp_data);
//    }
//
//    return data;
//}

// @breif OpenGLで表示しているウィンドウをcv::Matに変換する
cv::Mat getScreen() {
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    int bits = glutGet(GLUT_WINDOW_BUFFER_SIZE);

    int type = CV_8UC4;		// またはCV_8UC3
    int format = GL_BGRA_EXT;	// またはGL_RGB, GL_RGBA, GL_BGR_EXT

    glReadBuffer(GL_FRONT);

    cv::Mat out_img(800, 1280, type);
    glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, out_img.data);

    return out_img;
}

void display(void) {

    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto elem : triangles) {
        glBegin(GL_TRIANGLES);
        glColor3f(proj_img.at<cv::Vec3b>(elem[1], elem[0])[2] / 255.0, proj_img.at<cv::Vec3b>(elem[1], elem[0])[1] / 255.0, proj_img.at<cv::Vec3b>(elem[1], elem[0])[0] / 255.0); //r,g,b
        glVertex2f(elem[0], elem[1]);

        glColor3f(proj_img.at<cv::Vec3b>(elem[3], elem[2])[2] / 255.0, proj_img.at<cv::Vec3b>(elem[3], elem[2])[1] / 255.0, proj_img.at<cv::Vec3b>(elem[3], elem[2])[0] / 255.0); //r,g,b
        glVertex2f(elem[2], elem[3]);

        glColor3f(proj_img.at<cv::Vec3b>(elem[5], elem[4])[2] / 255.0, proj_img.at<cv::Vec3b>(elem[5], elem[4])[1] / 255.0, proj_img.at<cv::Vec3b>(elem[5], elem[4])[0] / 255.0); //r,g,b
        glVertex2f(elem[4], elem[5]);

        glEnd();

    }
    ////読み込みデータ格納先2次元vectorを用意する、今回の数値型はなんでも良い
    //std::vector<std::vector<double> > data;//ドロネー読み込み用

    //std::string filename("delaunay_test_p2c_2保存.csv");

    ////Getdata関数を使う    
    ////テンプレート引数として読み込みデータの型名を指定(今回はdouble)。
    ////第１引数に読み込みたいファイルのカレントディレクトリからのパスを指定する
    //std::cout << "reading " << filename << "..." << std::endl;
    //data = Getdata<double>(filename);
    //int m = 0;
    //for (std::size_t line = 0; line < data.size(); line++) {
    //    for (std::size_t column = 0; column < data[line].size(); column++) {
    //        //std::cout << data[line][column] << " ";
    //    }
    //    //std::cout << std::endl;
    //    m++;
    //}

    //for (int p = 0; p < m; p++) {
    //    glBegin(GL_TRIANGLES);
    //    glColor3f(0.0, data[p][7] / 1080.0, data[p][6] / 1920.0); //r,g,b
    //    glVertex2f(data[p][0], data[p][1]);
    //    /*if (data[p][1] > 700) {
    //        std::cout << data[p][1] << std::endl;
    //    }*/

    //    glColor3f(0.0, data[p][9] / 1080.0, data[p][8] / 1920.0); //r,g,b
    //    glVertex2f(data[p][2], data[p][3]);

    //    glColor3f(0.0, data[p][11] / 1080.0, data[p][10] / 1920.0); //r,g,b
    //    glVertex2f(data[p][4], data[p][5]);


    //    glEnd();

    //}
    glFlush();
    //std::cout << "書き込み開始" << std::endl;
    ////Mat p2c = cv::Mat::zeros(800, 1280, CV_16U);
    ////p2c = getScreen();
    //Mat p2c = getScreen();
    //imwrite("p2cマスク_2.png", p2c);
    //std::string filename2 = "p2cpfm";
    ////WritePFMImage(filename2, p2c);
    ////imwrite("delaunayp2c_2.pfm", p2c);
    //std::cout << "書き込み終了" << std::endl;

    cv::Mat p2c_img = getScreen();
    cv::imwrite("P2C_finish.png", p2c_img);
}

//void resize(int w, int h) {
//    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
//    std::cout << "リサイズ" << std::endl;
//    std::cout << w << ", " << h << std::endl;
//    /* ウィンドウ全体をビューポートにする */
//    glMatrixMode(GL_PROJECTION);  /* マトリックスモードを投影変換にする */
//    glLoadIdentity();             /* 変換行列の初期化 */
//    glOrtho(0, 1280, 0, 800, -1.0, 1.0);
//}


void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    //h = 800;
    //glViewport(0, 0, (GLsizei)1280, (GLsizei)800);
    std::cout << "リサイズ" << std::endl;
    std::cout << w << ", " << h << std::endl;
    /* ウィンドウ全体をビューポートにする */
    glMatrixMode(GL_PROJECTION);  /* マトリックスモードを投影変換にする */
    glLoadIdentity();             /* 変換行列の初期化 */
    glOrtho(0, 1280,
        0, 800, -1.0, 1.0);
    /* 平行射影or正射影 */
    /* ワールド座標系の表示領域を，ビューポートの大きさに比例させる */

}

int main(int argc, char** argv) {

    proj_img = cv::imread("projector_img.png");
    cv::Rect rect(0, 0, proj_img.cols, proj_img.rows);
    cv::Subdiv2D c2pList(rect);

    csvreader("c2p.csv", c2pList);
    c2pList.getTriangleList(triangles);

    glutInit(&argc, argv);
    glutInitWindowSize(proj_img.cols, proj_img.rows);
    glutInitWindowPosition(1280, 0);
    glutCreateWindow("プロジェクター画像");

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMainLoop();

    return 0;
}