#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include <iostream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "global.hpp"
#include "logging/logging.hpp"
#include "algo/icra2018_njust_armor/interface.hpp"

#include "interfaces/video_source.hpp"
#include "interfaces/video_target.hpp"

using namespace std;
using namespace cv;

class Main {
public:
    Main() {}
    ~Main();
    int main(int argc, char** argv);

    // Value will change to false after a ctrl-c.
    // Used for clean shutdown.
    bool should_run = true;

private:
    YAML::Node _config;
    void _loadConfig(string filename);
    string _base_folder = "";

    void _prepareArmorDetect();

    VideoSource* _video_src = NULL;
    VideoTarget* _video_tgt = NULL;

    ICRA2018_NJUST_Armor::Armor_Interface* _armorDetect = NULL;
};

#endif