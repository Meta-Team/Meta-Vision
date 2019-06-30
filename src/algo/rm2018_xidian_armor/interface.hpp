#ifndef RM2018_XIDIAN_ARMOR_INTERFACE_HPP
#define RM2018_XIDIAN_ARMOR_INTERFACE_HPP

//OpenCV headers
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>

#include "armor_info.hpp"
#include "armor_detect.hpp"
#include "settings.hpp"

using namespace std;
using namespace cv;

namespace RM2018_XiDian_Armor {
    
    /** Interface for integration with main vision program */
    
    class Armor_Interface {
        
    public:

        /** RM2018_XiDian_Armor Settings object, read xml file */
        Settings *settings;

        /** Main Armor Detector object */
        ArmorDetector *armor_detector;

    public:

//        /**
//         * @brief Construct a new Armor_Interface object
//         */
//        Armor_Interface();

        /**
         * @brief Construct a new Armor_Interface object, read settings from YAML node
         * @param root Node containing config to algorithm
         */
        Armor_Interface(const YAML::Node& root);

        /**
         * @brief Destroy the Armor_Interface object
         */
        ~Armor_Interface();

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
