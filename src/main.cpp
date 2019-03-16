#include <iostream>
#include <opencv2/opencv.hpp>
#include "algo/icra2018_njust_armor/interface.hpp"
using namespace std;
using namespace cv;

ICRA2018_NJUST_Armor::Armor_Interface armor_detect;

int main(int argc, char** argv){
    VideoCapture cap("/Users/lantian/Projects/robomaster/test_clips/步兵素材蓝车侧面-ev-0.MOV");
    if(!cap.isOpened()) {
        cout << "Input open failed" << endl;
        return -1;
    }

    VideoWriter wri("test.mp4", CV_FOURCC('M', 'P', '4', 'V'), 30, Size(640, 480));
    if(!wri.isOpened()) {
        cout << "Output open failed" << endl;
        return -1;
    }

    Mat frame;
    int i = 0;
    while(cap.read(frame)) {
        Mat resized;
        resize(frame, resized, Size(640, 480));
        cout << "Read frame #" << i++ << endl;

        RotatedRect rect = armor_detect.analyze(resized);
        Point2f vertices[4];
        rect.points(vertices);
        for (int i = 0; i < 4; i++) {
            line(resized, vertices[i], vertices[(i+1)%4], Scalar(0,0,255), 2);
        }
        wri.write(resized);
        // cout << vertices[0] << vertices[1] << vertices[2] << vertices[3] << endl;
    }
    cap.release();
    wri.release();
    return 0;
}