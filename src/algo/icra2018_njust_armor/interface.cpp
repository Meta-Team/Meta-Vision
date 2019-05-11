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
        settings = new Settings();

        armor_detector = new ArmorDetector(settings->armor);
        Mat template_img = imread(settings->template_image_file);
        armor_detector->initTemplate(template_img);
        armor_detector->setPara(settings->armor);
    }

    Armor_Interface::Armor_Interface(const YAML::Node& root) {
        settings = new Settings(root);

        armor_detector = new ArmorDetector(settings->armor);
        Mat template_img = imread(settings->template_image_file);
        armor_detector->initTemplate(template_img);
        armor_detector->setPara(settings->armor);
    }

    Armor_Interface::~Armor_Interface() {
        delete settings;
        delete armor_detector;
    }

    void Armor_Interface::setEnemyColor(int enemyColor) {
        armor_detector->setEnemyColor(enemyColor);
    }

    RotatedRect Armor_Interface::analyze(const Mat& src){
        return armor_detector->getTargetAera(src);
    }
}