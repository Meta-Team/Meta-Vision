#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include <iostream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "global.hpp"
#include "logging/logging.hpp"
//#include "algo/icra2018_njust_armor/interface.hpp"
#include "algo/interface.hpp"

#include "interfaces/multithread.hpp"
#include "interfaces/video_source.hpp"
#include "interfaces/video_target.hpp"
#include "serial/status.hpp"

using namespace std;
using namespace cv;

/**
 * @brief Main job of the program. Used to avoid global variables/simplify management.
 */
class Main {
public:
    Main() {}
    ~Main();
    int main(int argc, char** argv);

    /**
     * @brief Signal for clean shutdown. Will change to false after a Ctrl-C by signal handler.
     */
    bool should_run = true;

    /**
     * @brief Root node of the configuration. May be directly read by other classes
     */
    YAML::Node config;
private:
    void _loadConfig(const string& filename);

    void _prepareArmorDetect();

    double _fraction2angle(double fraction, double maxAngle);

    /**
     * @brief Video Source for this session. Specified by config file.
     */
    VideoSource* _video_src = NULL;

    /**
     * @brief Video Target for this session. Specified by config file.
     */
    VideoTarget* _video_tgt = NULL;

    /**
     * @brief Armor Detect interface for this session. Currently only ICRA2018_NJUST_Armor is available.
     */
//    ICRA2018_NJUST_Armor::MetaInterface* _armorDetect = NULL;
    RM2018_Xidian_Armor::MetaInterface* _armorDetect = NULL;

    /**
     * @brief Serial interface to communicate with RMBoard and judge system.
     */
    SerialStatus* _serial = NULL;
};

#endif