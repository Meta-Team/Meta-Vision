//OpenCV headers
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

#include<iostream>
#include<stdlib.h>
#include<thread>

#include "settings.hpp"
#include "armor_detector.hpp"
#include "interface.hpp"
using namespace std;
using namespace cv;

namespace ICRA2018_NJUST_Armor {
    Armor_Interface::Armor_Interface() {
        settings = new Settings("/Users/lantian/Projects/robomaster/armor_detect/src/algo/icra2018_njust_armor/template/param_config.xml");

        armor_detector = new ArmorDetector(settings->armor);
        
        Mat template_img = imread("/Users/lantian/Projects/robomaster/armor_detect/src/algo/icra2018_njust_armor/template/template.bmp");
        Mat small_template_img = imread("/Users/lantian/Projects/robomaster/armor_detect/src/algo/icra2018_njust_armor/template/small_template.bmp");
        armor_detector->initTemplate(template_img, small_template_img);
        armor_detector->setPara(settings->armor);
    }

    Armor_Interface::~Armor_Interface() {
        delete settings;
        delete armor_detector;
    }

    RotatedRect Armor_Interface::analyze(const Mat& src){
        return armor_detector->getTargetAera(src);
    }
}