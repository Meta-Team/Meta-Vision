/**
 * Robomaster Vision program of Autocar
 * Copyright (c) 2018, Xidian University Robotics Vision group.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files(the "Software"), to deal in the Software without restriction.
 */

#ifndef ARMOR_DETECT_H_
#define ARMOR_DETECT_H_

#include <iostream>
#include <chrono>
#include <ctype.h>
#include <opencv2/ml.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "armor_param.hpp"
//#include "slover/armor_recorder.hpp"
//#include "common/common_serial.h"

// FIXME: for version adaption
#define CV_RETR_EXTERNAL RETR_EXTERNAL
#define CV_CHAIN_APPROX_SIMPLE CHAIN_APPROX_SIMPLE
#define CV_THRESH_BINARY THRESH_BINARY

using namespace cv;
using namespace cv::ml;

#define POINT_DIST(p1, p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y))

namespace RM2018_Xidian_Armor {

    enum EnemyColor {
        RED = 0, BLUE = 1
    };

    class ArmorDetector {

    public:

        ArmorDetector(const ArmorParam &para, const std::string &smallArmorPic, const std::string &bigArmorPic,
                      const std::string &smallArmorSVMFile, const std::string &bigArmorSVMFile)
                      : para_(para) {
            initArmorHist(smallArmorPic, bigArmorPic); // load big armor
            svm_big = StatModel::load<SVM>(bigArmorSVMFile);
            svm_small = StatModel::load<SVM>(smallArmorSVMFile);
        };

        void setEnemyColor(int enemyColor) {
            para_.enemy_color = enemyColor;
        }

        void initArmorHist(const std::string &smallArmorPic, const std::string &bigArmorPic);


        /**
         * 检测API
         * @param src
         * @param armorsCandidate
         * @return
         */
        bool detect(const cv::Mat &src, std::vector<ArmorInfo> &armorsCandidate);

    private:

        int armorToArmorTest(const cv::RotatedRect &_rect1, const cv::RotatedRect &_rect2);
        bool makeRectSafe(cv::Rect &rect, cv::Size size);
        void adjustRect(cv::RotatedRect &rect);

        void drawRotatedRect(cv::Mat &img, const cv::RotatedRect &rect, const cv::Scalar &color, int thickness);

        std::vector<std::vector<cv::Point>> findContours(const cv::Mat &binary_img);

        cv::Mat distillationColor(const cv::Mat &src_img, unsigned int color);

        void chooseTargetFromLights(std::vector<cv::RotatedRect> &lights, std::vector<ArmorInfo> &armor_vector);


        void filterArmors(std::vector<ArmorInfo> &armors);

        void detectLights(const cv::Mat &src, std::vector<cv::RotatedRect> &lights);  //, double yaw_diff = 0);

        void filterLights(std::vector<cv::RotatedRect> &lights);   //, double yaw_diff = 0);

        /**
         * @brief: 根据装甲板不同的移动情况框选装甲板
         */
        cv::RotatedRect boundingRRect(const cv::RotatedRect &left, const cv::RotatedRect &right);

        cv::RotatedRect boundingRRectFast(const cv::RotatedRect &left, const cv::RotatedRect &right);

        cv::RotatedRect boundingRRectSlow(const cv::RotatedRect &left, const cv::RotatedRect &right);

        /**
         * @brief: filterArmors 方法1: 计算灰度和均值
         * @param: cv::Mat& mask
         * @param: const cv::RotatedRect& rect
         * @param: double& mean
         * @param: double& stddev
         * no-return;
         */
        void calcMeanStdDev(cv::Mat &mask, const cv::RotatedRect &rect, double &m, double &stddev);

        /**
         * @brief: filterArmors 方法2: 直方图匹配
         * @param: cv::Mat mask
         * @param: cv::RotateRect rect
         * @param: bool visual
         * @return: 相关度 0 ~ 1
         */
        double calcHistDiff(cv::Mat &mask, const cv::RotatedRect &rect, bool Visual);

        /**
         * @brief: support vector machine for armor
         * @param: 英雄 wsize(100,25)
         * @param: 步兵 wsize(60,25)
         * @return: 预测值 -1 ~ +1
         */
        float calcBigArmorSVM(Mat &img_roi);

        float calcSmallArmorSVM(Mat &img_roi);

        float calcArmorROI(cv::RotatedRect &rect, bool visual = 0);

    private:

        ArmorParam para_;    // 装甲板的参数
        cv::Mat src_img_;
        cv::Mat gray_img_;
        cv::Mat show_lights_before_filter_;
        cv::Mat show_lights_after_filter_;
        cv::Mat show_armors_befor_filter_;
        cv::Mat show_armors_after_filter_;
        std::chrono::steady_clock::time_point speed_test_start_begin_time;

        bool last_detect;

        std::vector<cv::RotatedRect> light_rects;
        std::vector<ArmorInfo> filte_rects;

        // 尝试过对1号步兵和2号英雄进行Hist的匹配分类
        cv::Mat armor_1_hist;  // 1号步兵的 Hist
        cv::Mat armor_2_hist;  // 2号英雄的 Hist

        // SVM效果会好很多，将来可以按照数字贴纸更细化的分类
        Ptr<SVM> svm_big;
        Ptr<SVM> svm_small;
    };

    int armorToArmorTest(const cv::RotatedRect &_rect1, const cv::RotatedRect &_rect2);


} // namespace RM2018_Xidian_Armor

#endif
