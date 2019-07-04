/**
 * Robomaster Vision program of Autocar
 * Copyright (c) 2018, Xidian University Robotics Vision group.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files(the "Software"), to deal in the Software without restriction.
 */

#include "camera_info.hpp"

namespace RM2018_Xidian_Armor {

    CameraInfo::CameraInfo(std::string infoFile) {

        cv::FileStorage fs(infoFile, cv::FileStorage::READ);
        if (!fs.isOpened())
            std::cout << "Cannot open " << infoFile << ", please check if the file is exist." << std::endl;
        cv::FileNode root = fs.root();

        // driver_mul
        root["Camera_Matrix"] >> camera_matrix;
        root["Distortion_Coefficients"] >> dist_coeffs;
        root["ptz_camera_x"] >> ptz_camera_x;
        root["ptz_camera_y"] >> ptz_camera_y;
        root["ptz_camera_z"] >> ptz_camera_z;
        root["z_scale"] >> z_scale;

        root["barrel_ptz_offset_y"] >> barrel_ptz_offset_y;


        std::cout << "Camera_Matrix Size: " << camera_matrix.size() << std::endl;
        std::cout << "Distortion_Coefficients Size: " << dist_coeffs.size() << std::endl;

        // double barrel_ptz_offset_y = 0;
        const double overlap_dist = 100000.0;
        double theta = -atan((ptz_camera_y + barrel_ptz_offset_y) / overlap_dist);
        double r_data[] = {1, 0, 0, 0, cos(theta), -sin(theta), 0, sin(theta), cos(theta)};
        double t_data[] = {0, ptz_camera_y, ptz_camera_z}; // ptz org position in camera coodinate system

        cv::Mat t(3, 1, CV_64FC1, t_data);
        cv::Mat r(3, 3, CV_64FC1, r_data); // Mat::eye(3, 3, CV_64FC1);
        t.copyTo(t_camera_ptz);
        r.copyTo(r_camera_ptz);

    }

}

    

