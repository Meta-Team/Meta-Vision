#ifndef _VIDEO_TARGET_HPP_
#define _VIDEO_TARGET_HPP_

#include "multithread.hpp"
#include "video_source.hpp"
#include <opencv2/opencv.hpp>

class VideoTarget : public Thread {
public:
    virtual ~VideoTarget() {}

    virtual bool isAvailable() = 0;

    virtual void writeFrame(cv::Mat& mat) = 0;
};

#endif