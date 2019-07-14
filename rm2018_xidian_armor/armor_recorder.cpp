/**
 * Robomaster Vision program of Autocar
 * Copyright (c) 2018, Xidian University Robotics Vision group.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files(the "Software"), to deal in the Software without restriction.
 */
#include "armor_recorder.hpp"
#include "angle_solver.hpp"

namespace RM2018_Xidian_Armor {


//#define SHOW_SRC_RECT

    const int bullet_speed = 18;
    const cv::Point ptoffset = cv::Point(22, 0); // 子弹的偏移量 offset x → y ↓

    /**
     * @brief 定义一个距离函数，计算装甲片之间姿态的“距离”
     */
    inline bool pos_distance(const ArmorPos &pos1, const ArmorPos &last_pos) {
        return std::sqrt((pos1.angle_x - last_pos.angle_x) * (pos1.angle_x - last_pos.angle_x) +
                         (pos1.angle_y - last_pos.angle_y) * (pos1.angle_y - last_pos.angle_y) +
                         std::abs(pos1.angle_z - last_pos.angle_z)) / 10;
    }

    ArmorPos ArmorRecorder::SelectFinalArmor(std::vector<ArmorInfo> &armors, AngleSolver &angle_solver,
                                             AngleSolverFactory &angle_solver_factory, const cv::Mat &src,
                                             ArmorInfo &final_armor) {
        std::vector<ArmorPos> pos_vect;
        std::vector<ArmorInfo> selected_vect;
#ifdef SHOW_SRC_RECT
        std::vector<ArmorInfo> armor_vect;
#endif
        ArmorPos ArmorPos_;
        for (auto armor : armors) {
            double armor_ratio = std::max(armor.rect.size.width, armor.rect.size.height) /
                                 std::min(armor.rect.size.width, armor.rect.size.height);
            cv::RotatedRect rect = armor.rect;
            if (armor_ratio < 4) {
                if (angle_solver_factory.getAngle(rect, AngleSolverFactory::TARGET_SMALL_ARMOR, ArmorPos_.angle_x,
                                                  ArmorPos_.angle_y, bullet_speed, ptoffset) == true) {
                    this->miss_detection_cnt = 0;
                    ArmorPos_.Flag = armor.state; // [1 2 3 4]
                    ArmorPos_.angle_z = angle_solver._distance;
                    pos_vect.push_back(ArmorPos_);
                    selected_vect.push_back(armor);
#ifdef SHOW_SRC_RECT
                    armor_vect.push_back(armor);
#endif
                } else {
                    ArmorPos_.Flag = 0;
                    ArmorPos_.angle_z = angle_solver._distance;
                }
            } else {
                if (angle_solver_factory.getAngle(rect, AngleSolverFactory::TARGET_ARMOR, ArmorPos_.angle_x,
                                                  ArmorPos_.angle_y, bullet_speed, ptoffset) == true) {
                    this->miss_detection_cnt = 0;
                    ArmorPos_.Flag = armor.state; // [1 2 3 4]
                    ArmorPos_.angle_z = angle_solver._distance;
                    pos_vect.push_back(ArmorPos_);
                    selected_vect.push_back(armor);
#ifdef SHOW_SRC_RECT
                    armor_vect.push_back(armor);
#endif
                } else {
                    ArmorPos_.Flag = 0;
                    ArmorPos_.angle_z = angle_solver._distance;
                }
            } // if infantry or hero
        } // for

        ArmorPos last_pos;

        if (history_pos.size()) {
            last_pos = history_pos.back();
        }

        if (pos_vect.size()) {
            double dis_min = 100000000;
            unsigned idx = 0;
            for (unsigned i = 0; i != pos_vect.size(); ++i) {
                double dis = pos_distance(pos_vect[i], last_pos);
                if (dis < dis_min) {
                    dis_min = dis;
                    idx = i;
                }
            }
#ifdef SHOW_SRC_RECT
            draw_rotated_rect(src, armor_vect[idx].rect, cv::Scalar(0, 255, 255), 2);
#endif
            final_armor = selected_vect[idx];
            return pos_vect[idx];
        }
        return ArmorPos();
    }

} // namespace RM2018_Xidian_Armor
