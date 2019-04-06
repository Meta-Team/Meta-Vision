#ifndef ICRA2018_NJUST_ARMOR_INTERFACE_HPP
#define ICRA2018_NJUST_ARMOR_INTERFACE_HPP

//OpenCV headers
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>

#include "settings.hpp"
using namespace std;
using namespace cv;

namespace ICRA2018_NJUST_Armor {
    class Armor_Interface {
    public:
        Settings* settings;
        ArmorDetector* armor_detector;
    public:
        Armor_Interface();
        Armor_Interface(const string base_dir, const YAML::Node& root);
        ~Armor_Interface();

        void setEnemyColor(int enemyColor);
        RotatedRect analyze(const Mat& src);
    };
}

#endif // ICRA2018_NJUST_ARMOR_INTERFACE_HPP
