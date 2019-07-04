/*******************************************************************************************************************
Copyright 2017 Dajiang Innovations Technology Co., Ltd (DJI)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files(the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and 
to permit persons to whom the Software is furnished to do so, subject to the following conditions : 

The above copyright notice and this permission notice shall be included in all copies or substantial portions of
the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*******************************************************************************************************************/

/**
 * Robomaster Vision program of Autocar
 * Copyright (c) 2018, Xidian University Robotics Vision group.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files(the "Software"), to deal in the Software without restriction.
 */

#pragma once

#include <list>
#include <chrono>
#include "armor_param.hpp"
#include "angle_solver.hpp"

namespace RM2018_Xidian_Armor {

    /**
     * @brief 记录历史信息,然后进行弹道预测.
     */
    class ArmorRecorder {

    public:

        ArmorRecorder(int _history_size = 5) : history_size(_history_size) {
            recorder_time = std::chrono::steady_clock::now();
        }

        double point_dist(const cv::Point2f &p1, const cv::Point2f &p2) {
            return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
        }

        ArmorPos SelectFinalArmor(std::vector<ArmorInfo> &armors, AngleSolver &angle_solver,
                                  AngleSolverFactory &angle_solver_factory, const cv::Mat &src,
                                  ArmorInfo &final_armor);

        void setLastResult(/*ArmorInfo last_armor,*/ ArmorPos last_pos, double time) {
            if (history_pos.size() < history_size) {
                // history_armor.push_back(last_armor);
                history_pos.push_back(last_pos);
                history_time.push_back(time);
            } else {
                // history_armor.push_back(last_armor);
                history_pos.push_back(last_pos);
                history_time.push_back(time);

                // history_armor.pop_front();
                history_pos.pop_front();
                history_time.pop_front();
            }
            recorder_time = std::chrono::steady_clock::now();
        }

        void clear() {
            //history_armor.clear();
            //history_time.clear();
            if (/*history_armor.size() &&*/ history_pos.size() && history_time.size()) {
                // history_armor.pop_front();
                history_pos.pop_front();
                history_time.pop_front();
            }
        }

        std::chrono::steady_clock::time_point recorder_time;

        // std::list<ArmorInfo> history_armor;
        std::list<ArmorPos> history_pos;
        std::list<double> history_time;
        int history_size;
        int miss_detection_cnt;

        double predict(double time);
    };

} // namespace RM2018_Xidian_Armor
