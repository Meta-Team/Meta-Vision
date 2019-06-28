//created by Anbang 2019/6/25//
//see https://github.com/ObitoLee/robomasters_base/blob/maste for original code//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "myHeader.hpp"

using namespace std;
using namespace cv;
#define ZOOM_FACTOR 1.0f
#define t 206 //threshold
#define minContour 15		//最小轮廓尺寸，用于控制检测目标的远近

#define maxContour 600		//最大轮廓尺寸，用于滤掉过大的错误目标
#define DARK 1
#define BLUE 1
#define BLUE_OFFSET 70  //TODO: choose a appropriate number for BLUE_OFFSET

int main() {
    Mat img = imread("C:\\Users\\86134\\CLionProjects\\armorDetector\\test.jpg");
    img = img(Rect(0,(int)img.rows/3,img.cols,(int)img.rows*2/3));
    cout<<"img.dimgs  = "<<img.dims<<endl;
    cout<<"img.rows  = "<<img.rows<<endl;
    cout<<"img.cols  = "<<img.cols<<endl;
    cout<<"img.channels = "<<img.channels()<<endl;
    cout<<"img.step[0] = " <<img.step[0]<<endl;
    cout<<"img.step[1] = " <<img.step[1]<<endl;
    cout<<"img.Elemsize = "<<img.elemSize()<<endl;    //一个像素点的大小  CV_8U3C=1*3
    cout<<"img.Elemsize1 = "<<img.elemSize1()<<endl;  //数据类型的大小 UCHAR = 1 （ElemSize / Channel）
    cvNamedWindow("test.jpg");
    imshow("test.jpg", img);
    waitKey(0);

    //TestProgram
    Mat imgThresholded_b, imgThresholded;
    Mat imgOriginal = img;//由于图像上半部分无有效信息，故取src图像的下面2/3
    Rect roiImg  = Rect(0, 0, img.cols, img.rows);
    vector<RotatedRect> vEllipse;//符合条件的椭圆

    Armors armors(imgOriginal.cols*ZOOM_FACTOR, imgOriginal.rows*ZOOM_FACTOR);//实例化Armors类

    vector<vector<Point> > contours;//检测到的轮廓



    int sendFilter = 0;
    bool sendBool = false;
    String noTargetReason;//检测不到目标输出的信息
    int64 t0 = getTickCount();
    GetDiffImage(imgOriginal, imgThresholded, BLUE_OFFSET, blue, roiImg);//蓝色
    //GetDiffImage(imgOriginal, imgThresholded,t , red, roiImg);//红色
    cvNamedWindow("testThreshold_blue.jpg");
    imshow("testThreshold_blue.jpg",imgThresholded);
    waitKey(0);
/*
    GetBrightImage(imgOriginal, imgThresholded, t, roiImg);
    cvNamedWindow("testThreshold_bright.jpg");
    imshow("testThreshold_bright.jpg",imgThresholded);
    waitKey(0);
    addWeighted(imgThresholded, 1, imgThresholded_b, 1,0.0, imgThresholded);
    cvNamedWindow("testThreshold_bright_and_blue.jpg");
    imshow("testThreshold_bright_and_blue.jpg",imgThresholded);
    waitKey(0);
*/
    //inRange(imgThresholded, 254, 255, imgThresholded);
    preProcess(imgThresholded);//预处理
    findContours(imgThresholded, contours, RETR_CCOMP, CHAIN_APPROX_NONE);

    drawContours(imgThresholded, contours, -1, Scalar(255, 255, 255));
    cvNamedWindow("contour.jpg");
    imshow("contour.jpg", imgThresholded);
    waitKey(0);
    for (auto itContours = contours.begin(); itContours != contours.end(); ++itContours)
    {

        vector<Point> points = *itContours;//将一个轮廓的所有点存入points

        if (itContours->size() > minContour && itContours->size() < maxContour)//筛选轮廓上的点大于100的轮廓

        {
            cout<<"possible contour detected!!!"<<endl;
            RotatedRect s = fitEllipse(Mat(points));//拟合椭圆



            if ((s.size.height < s.size.width) || (s.size.height * s.size.width > 2000))//|| (s.size.height / s.size.width > 12))

            {

                //cout<<s.size<<endl;

                cout<< "size don't cater to standard.\n"<<endl;

                continue;

            }

#ifdef DARK

            Rect colorRect = (s.boundingRect() - Point(7, 7) + Size(14, 14)) & roiImg;

            int color = colorJudge(imgOriginal(colorRect));

#ifdef BLUE

            if (color == blue)

                vEllipse.push_back(s);

            else

                continue;

#endif // BLUE

#ifdef RED

            if (color == red || color == purple)

                        vEllipse.push_back(s);

                    else

                        continue;



#endif // RED

#else//Bright

            vEllipse.push_back(s);

#endif // DARK

#ifdef DEBUG

            ellipse(imgOriginal, s, Scalar(255,255, 66),2);

#endif // DEBUG

        }

        else

        {

            noTargetReason += "size of contours is too small or too big.\n";

        }



        points.clear();//points.swap(vector<Point>());

    }


    cout<<"num_of_armor: "<<vEllipse.size()<<endl;
    armors.inputEllipse(vEllipse);//输入将测到的椭圆，寻找装甲

    Point2f target = armors.getTarget();//求目标坐标
    //cout<<"rt h: "<<rt.size.height<<" w: "<<rt.size.width<<endl;
    //cout<<"rt_center_x: "<<rt.center.x<<" rt_center_y: "<<rt.center.y<<endl;
    //Point2f vertices[4];      //定义4个点的数组
    //rt.points(vertices);   //将四个点存储到vertices数组中
    cout<<target.x<<" "<<target.y<<endl;
    line(img, Point2f(target.x-15,target.y),Point2f(target.x+15,target.y), Scalar(0,255,0));
    line(img, Point2f(target.x,target.y-15),Point2f(target.x,target.y+15), Scalar(0,255,0));
    cvNamedWindow("detect.jpg");
    imshow("detect.jpg", img);
    waitKey(0);



}