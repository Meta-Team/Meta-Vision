#ifndef ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP
#define ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP

#include <opencv2/core/core.hpp>
#include <string>
#include <yaml-cpp/yaml.h>
#include "armor_detect.hpp"

using namespace cv;

namespace RM2018_XiDian_Armor {
    /**
     * @brief Settings parser & converter from YAML to internal struct
     *        of ICRA2018_NJUST_Armor algorithm.
     */
    class Settings {

    public:

//        /**
//         * @brief Construct a new Settings object
//         */
//        Settings() {}

        /**
         * @brief Construct a new Settings object from a YAML config file
         * 
         * @param filename path to YAML config for the algorithm
         */
        Settings(const string& filename) {
            YAML::Node config = YAML::LoadFile(filename);
            read(config);
        }

        /**
         * @brief Construct a new Settings object from a parsed YAML node
         * 
         * @param root YAML node of the config for the algorithm
         */
        Settings(const YAML::Node& root) {
            read(root);
        }

        /**
         * @brief Convert config from YAML node to struct ArmorParam used by the algorithm
         * 
         * @param root YAML node of the config for the algorithm
         */
        void read(const YAML::Node& root);

    public:

        /** ArmorParam object to store config, for use with main algorithm */
        ArmorParam armor;

        /** Path to template image file */
        string big_armor_pic_file;
        string small_armor_pic_file;
    };
}

#endif // ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP
