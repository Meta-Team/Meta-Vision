#include "settings.hpp"

namespace RM2018_Xidian_Armor {

    void Settings::read(const YAML::Node &root) {

        // Converted from armor_info.read()

        armor.light_threshold_val = root["light_threshold_val"].as<float>();

        armor.light_min_aspect_ratio = root["light_min_aspect_ratio"].as<float>();
        armor.light_max_aspect_ratio = root["light_max_aspect_ratio"].as<float>();

        armor.light_min_area = root["light_min_area"].as<float>();
        armor.light_max_area = root["light_max_area"].as<float>();

        armor.light_max_angle = root["light_max_angle"].as<float>();

        armor.light_max_angle_diff = root["light_max_angle_diff"].as<float>();
        armor.light_max_height_diff = root["light_max_height_diff"].as<float>();
        armor.light_max_width_diff = root["light_max_width_diff"].as<float>();

        armor.armor_min_ratio = root["armor_min_ratio"].as<float>();
        armor.armor_max_ratio = root["armor_max_ratio"].as<float>();

        armor.armor_light_angle_diff = root["armor_light_angle_diff"].as<float>();

        armor.filter_armor_area = root["filter_armor_area"].as<float>();

        armor.armor_max_angle = root["armor_max_angle"].as<float>();
        armor.armor_min_area = root["armor_min_area"].as<float>();
        armor.armor_max_area = root["armor_max_area"].as<float>();

        armor.armor_max_aspect_ratio = root["armor_max_aspect_ratio"].as<float>();
        armor.armor_max_pixel_val = root["armor_max_pixel_val"].as<float>();
        armor.armor_max_stddev = root["armor_max_stddev"].as<float>();
        armor.armor_width = root["width"].as<float>();
        armor.armor_height = root["height"].as<float>();
//        armor.enemy_color = root["enemy_color"].as<bool>();

        armor.blue_color_diff = root["blue_color_diff"].as<int>();
        armor.red_color_diff = root["red_color_diff"].as<int>();


        big_armor_pic_file = root["big_armor_pic_file"].as<string>();
        small_armor_pic_file = root["small_armor_pic_file"].as<string>();

        camera_info_file = root["camera_info_file"].as<string>();

        big_armor_svm_file = root["big_armor_svm_file"].as<string>();
        small_armor_svm_file = root["small_armor_svm_file"].as<string>();
    }
}