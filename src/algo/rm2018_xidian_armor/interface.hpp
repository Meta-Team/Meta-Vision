#ifndef RM2018_XIDIAN_ARMOR_INTERFACE_HPP
#define RM2018_XIDIAN_ARMOR_INTERFACE_HPP

//OpenCV headers
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>

#include "armor_param.hpp"
#include "armor_detect.hpp"
#include "armor_recorder.hpp"
#include "camera_info.hpp"
#include "angle_solver.hpp"

#include "settings.hpp"

using namespace std;
using namespace cv;

namespace RM2018_Xidian_Armor {
    
    /** Interface for integration with main vision program */
    
    class MetaInterface {
        
    public:

        Settings *settings;

        ArmorDetector *armorDetector;
        ArmorRecorder *armorRecorder;

        CameraInfo *cameraInfo;
        AngleSolver *angleSolver;
        AngleSolverFactory *angleSolverFactory;

    public:

        /**
         * @brief Construct a new Armor_Interface object, read settings from YAML node
         * @param root Node containing config to algorithm
         */
        MetaInterface(const YAML::Node& root);

        /**
         * @brief Destroy the Armor_Interface object
         */
        ~MetaInterface();

        /**
         * @brief Set color of enemy
         * 
         * @param enemyColor 0: red, 1: blue
         */
        void setEnemyColor(int enemyColor);

        /**
         * @brief Given an image, find position of enemy armor
         * 
         * @param src Source image to perform search on
         * @return RotatedRect Rectangle enclosing enemy armor
         */
        RotatedRect analyze(const Mat &src);
    };
}

#endif // RM2018_XIDIAN_ARMOR_INTERFACE_HPP
