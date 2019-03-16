#include "settings.hpp"

namespace ICRA2018_NJUST_Armor {
    void Settings::read(const FileStorage & fs){
        // for debug image
        fs["show_image"] >> show_image;
        fs["save_result"] >> save_result;

        // for armor system
        fs["min_light_gray"] >> armor.min_light_gray;
        fs["min_light_height"] >> armor.min_light_height;
        fs["avg_contrast_threshold"] >> armor.avg_contrast_threshold;
        fs["light_slope_offset"] >> armor.light_slope_offset;
        fs["max_light_delta_h"] >> armor.max_light_delta_h;
        fs["min_light_delta_h"] >> armor.min_light_delta_h;
        fs["max_light_delta_v"] >> armor.max_light_delta_v;
        fs["max_light_delta_angle"] >> armor.max_light_delta_angle;
        fs["avg_board_gray_threshold"] >> armor.avg_board_gray_threshold;
        fs["avg_board_grad_threshold"] >> armor.avg_board_grad_threshold;
        fs["grad_threshold"] >> armor.grad_threshold;
        fs["br_threshold"] >> armor.br_threshold;
        fs["enemy_color"] >> armor.enemy_color;

        // for armor template
        fs["template_image_file"] >> template_image_file;
        fs["small_template_image_file"] >> small_template_image_file;

        // for system mode
        fs["mode"] >> mode;

        check();
    }

    void Settings::check(){
        ArmorParam default_armor;
        if (armor.min_light_gray < 5)
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

    void Settings::write(FileStorage& fs) {
        // for debug image
        cvWriteComment(*fs, "\nFor Debug Image", 0);
        fs << "show_image" << show_image;
        fs << "save_result" << save_result;

        // for armor system
        cvWriteComment(*fs, "\nParameter for Armor Detection System", 0);
        fs << "min_light_gray" << armor.min_light_gray
            << "min_light_height" << armor.min_light_height
            << "avg_contrast_threshold" << armor.avg_contrast_threshold
            << "light_slope_offset" << armor.light_slope_offset
            << "max_light_delta_h" << armor.max_light_delta_h
            << "min_light_delta_h" << armor.min_light_delta_h
            << "max_light_delta_v" << armor.max_light_delta_v
            << "max_light_delta_angle" << armor.max_light_delta_angle
            << "avg_board_gray_threshold" << armor.avg_board_gray_threshold
            << "avg_board_grad_threshold" << armor.avg_board_grad_threshold
            << "grad_threshold" << armor.grad_threshold
            << "br_threshold" << armor.br_threshold;

        // for enemy color
        cvWriteComment(*fs, "\nParameter for Enemy Color, 0(default) means for red, otherwise blue", 0);
        fs << "enemy_color" << armor.enemy_color;


        // for armor template
        cvWriteComment(*fs, "\nParameter for Template", 0);
        fs << "template_image_file" << template_image_file;
        fs << "small_template_image_file" << string("small_template_image_file");

        // for system mode
        cvWriteComment(*fs, "\nParameter for Vision System Mode, 0(default) means for armor detection mode, 1 means for rune system mode", 0);
        fs << "mode" << mode;
    }
}