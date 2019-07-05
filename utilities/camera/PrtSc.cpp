//
// Created by Kerui Zhu on 7/5/2019.
//

#include <iostream>
#include <unistd.h>
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;

VideoCapture* openCamera(int id, int width, int height, int fps) {

    VideoCapture* _cap;

    // Try to use different backends to start capture
    do {
        _cap = new VideoCapture(id + CAP_V4L2);
        if(_cap->isOpened()) break;

        _cap = new VideoCapture(id + CAP_ANY);
        if(_cap->isOpened()) break;

        throw std::invalid_argument("Failed to open camera");
    } while(0);

    _cap->set(CAP_PROP_FRAME_WIDTH, width);
    _cap->set(CAP_PROP_FRAME_HEIGHT, height);
    _cap->set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    _cap->set(CAP_PROP_FPS, fps);

    return _cap;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "Parameter needed: directory with data." << endl;
        return 1;
    }

    if (0 > chdir(argv[1])) {
        cout << "Failed to change to directory: " << argv[1];
        return 1;
    } else {
        char buf[1024];
        if (getcwd(buf, 1024)) {
            cout << "Changed to directory: " << string(buf) << endl;
        } else {
            cout << "Changed to directory: " << argv[1] << endl;
        }
    }

    Mat frame;
    VideoCapture *cap = openCamera(1, 640, 360, 330);
    char fileName[100];
    char key;
    int count = 0;

    while(1)
    {
        if(!cap->read(frame)) continue;
//        cvFlip(pImg, NULL, 1); //水平翻转图像，像照镜子一样，不想要这个效果可以去掉此句
        key = waitKey(50);
        if(key == 27) break; //按ESC键退出程序
        if(key == 'c')       //按c键拍照
        {
            sprintf(fileName, "Picture%d.jpg", ++count); //生成文件名
            imwrite(fileName, frame);
            imshow("Camera", frame);
            waitKey(200); //反色图像显示ms
        }
        imshow("Camera", frame);
    }
    delete cap;
    return 0;
}