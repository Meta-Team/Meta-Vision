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
        Settings(const string& base_dir, const string& filename) {
            YAML::Node config = YAML::LoadFile(filename);
            read(base_dir, config);
        }
        Settings(const string& base_dir, const YAML::Node& root) {
            read(base_dir, root);
        }
        void read(const string& base_dir, const YAML::Node& root);
        void check();
    public:
        ArmorParam armor;

        string template_image_file;
        string small_template_image_file;
    };
}

#endif // ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP
