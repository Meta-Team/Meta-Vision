/**
 * Robomaster Vision program of Autocar
 * Copyright (c) 2018, Xidian University Robotics Vision group.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files(the "Software"), to deal in the Software without restriction.
 */

#include "info.h"
#include "detect.hpp"

#include "config.hpp"

int main()
{
    std::cout << "MAKE SURE THE OUTPUT DIRECTORY EXISTS AND CLEAN." << CONFIG_SAMPLE_VIDEO_FILE << std::endl;

    cv::VideoCapture capture_camera_forward(CONFIG_SAMPLE_VIDEO_FILE);
    if (!capture_camera_forward.isOpened()) {
        std::cout << "Fail to open file " << CONFIG_SAMPLE_VIDEO_FILE << std::endl;
        return 1;
    }

    int index = 0;
    cv::Mat frame_forward;
    armor_sample armor_detector;
   
    while (true) {
        capture_camera_forward >> frame_forward;
        if (frame_forward.empty()) break;
		armor_detector.detect(frame_forward, index);
        try {
		    imshow("result", frame_forward);
            cv::waitKey(1);
		}
		catch (cv::Exception e) {
			std::cout << "Show image error" << std::endl;
		}
//        cv::waitKey(0);
	}

    std::cout << "PLEASE PROCESS OUTPUT IMAGES ASAP." << CONFIG_SAMPLE_VIDEO_FILE << std::endl;

    return 0;
}