#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include <iostream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "global.hpp"
#include "logging/logging.hpp"
#include "algo/icra2018_njust_armor/interface.hpp"

using namespace std;
using namespace cv;

class Main {
public:
    Main() {}
    ~Main();
    int main(int argc, char** argv);
private:
    YAML::Node _config;
    void _loadConfig(string filename);
    void _prepareArmorDetect();
    void _prepareVideoCapture();
    void _prepareVideoWriter();
    VideoCapture* _vCapture;
    VideoWriter* _vWriter;

    ICRA2018_NJUST_Armor::Armor_Interface* _armorDetect;
};

#endif