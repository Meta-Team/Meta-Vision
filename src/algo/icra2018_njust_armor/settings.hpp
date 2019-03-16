#ifndef ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP
#define ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP

//#include <opencv2/core/core.hpp>
#include "opencv2/core/core.hpp"
#include <string>
#include <yaml-cpp/yaml.h>
#include "armor_detector.hpp"
using namespace cv;

namespace ICRA2018_NJUST_Armor {
    class Settings {
    public:
        Settings() {}
        Settings(const string & filename) {
            YAML::Node config = YAML::LoadFile(filename);
            read(config);
        }
        Settings(const YAML::Node& root) {
            read(root);
        }
        void read(const YAML::Node& root);
        void check();
    public:
        ArmorParam armor;

        string template_image_file;
        string small_template_image_file;
    };
}

#endif // ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP
