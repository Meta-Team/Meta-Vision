#include "settings.hpp"

namespace ICRA2018_NJUST_Armor {
    void Settings::read(const YAML::Node& root){
        armor.min_light_gray = root["min_light_gray"].as<int>();
        armor.min_light_height = root["min_light_height"].as<int>();
        armor.avg_contrast_threshold = root["avg_contrast_threshold"].as<int>();
        armor.light_slope_offset = root["light_slope_offset"].as<int>();
        armor.max_light_delta_h = root["max_light_delta_h"].as<int>();
        armor.min_light_delta_h = root["min_light_delta_h"].as<int>();
        armor.max_light_delta_v = root["max_light_delta_v"].as<int>();
        armor.max_light_delta_angle = root["max_light_delta_angle"].as<int>();
        armor.avg_board_gray_threshold = root["avg_board_gray_threshold"].as<int>();
        armor.avg_board_grad_threshold = root["avg_board_grad_threshold"].as<int>();
        armor.grad_threshold = root["grad_threshold"].as<int>();
        armor.br_threshold = root["br_threshold"].as<int>();

        template_image_file = root["template_image_file"].as<string>();
        
        check();
    }

    void Settings::check(){
        ArmorParam default_armor;
        if (armor.min_light_gray < 5) {}
                armor.min_light_gray = default_armor.min_light_gray;
        if (armor.min_light_height < 5)
                armor.min_light_height = default_armor.min_light_height;
        if (armor.avg_contrast_threshold < 5)
                armor.avg_contrast_threshold = default_armor.avg_contrast_threshold;
        if (armor.light_slope_offset < 5)
                armor.light_slope_offset = default_armor.light_slope_offset;
        if (armor.max_light_delta_h < 5)
                armor.max_light_delta_h = default_armor.max_light_delta_h;
        if (armor.min_light_delta_h < 5)
                armor.min_light_delta_h = default_armor.min_light_delta_h;
        if (armor.max_light_delta_v < 5)
                armor.max_light_delta_v = default_armor.max_light_delta_v;
        if (armor.max_light_delta_angle < 5)
                armor.max_light_delta_angle = default_armor.max_light_delta_angle;
        if (armor.avg_board_gray_threshold < 5)
                armor.avg_board_gray_threshold = default_armor.avg_board_gray_threshold;
        if (armor.avg_board_grad_threshold < 5)
                armor.avg_board_grad_threshold = default_armor.avg_board_grad_threshold;
        if (armor.grad_threshold < 5)
                armor.grad_threshold = default_armor.grad_threshold;
        if (armor.br_threshold < 5)
                armor.br_threshold = default_armor.br_threshold;
    }
}