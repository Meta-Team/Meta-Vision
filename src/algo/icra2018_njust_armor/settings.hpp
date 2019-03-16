#ifndef ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP
#define ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP

//#include <opencv2/core/core.hpp>
#include "opencv2/core/core.hpp"
#include <string>
#include "armor_detector.hpp"
using namespace cv;

#define ARMOR_MODE 0
#define RUNE_MODE 1

namespace ICRA2018_NJUST_Armor {
    class Settings {
    public:
        Settings(const string & filename) {
            FileStorage setting_fs(filename, FileStorage::READ);
            read(setting_fs);
    //        cout<<"show_image="<<show_image<<endl;
            setting_fs.release();
        }
        void read(const FileStorage & fs);
        void check();
        void write(FileStorage& fs);
    public:
        int show_image;
        int save_result;
    //    RuneParam rune;
        ArmorParam armor;
        int mode;
    //    string intrinsic_file_480;
    //    string intrinsic_file_720;
    //    int exposure_time;
        string template_image_file;
        string small_template_image_file = "/Users/lantian/Projects/robomaster/armor_detect/src/algo/icra2018_njust_armor/template/small_template.bmp";
    //    double min_detect_distance;
    //    double max_detect_distance;
    //    double bullet_speed;
    //    double scale_z;
    //    double scale_z_480;
    };
}

#endif // ALGO_ICRA2018_NJUST_ARMOR_SETTINGS_HPP
