/**
 * Robomaster Vision program of Autocar
 * Copyright (c) 2018, Xidian University Robotics Vision group.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files(the "Software"), to deal in the Software without restriction.
 */

#ifndef CAMERA_DRIVER_H
#define CAMERA_DRIVER_H

#include <opencv2/opencv.hpp>

namespace RM2018_Xidian_Armor {

    /**
     * @brief: 摄像头配置类型
     */
    class CameraInfo {

    public:

        /**
         * @brief: Read vision parameters
         */
        CameraInfo(std::string infoFile);

    public:

        // 相机参数
        cv::Mat camera_matrix;
        cv::Mat dist_coeffs;

        // 相机安装
        double ptz_camera_x;
        double ptz_camera_y;
        double ptz_camera_z;

        cv::Mat t_camera_ptz;
        cv::Mat r_camera_ptz;
        double barrel_ptz_offset_y;
        double z_scale;
    };

}

#endif // VISION_PARAM_H
