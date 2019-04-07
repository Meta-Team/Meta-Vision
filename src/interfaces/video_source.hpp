#ifndef _VIDEO_SOURCE_HPP_
#define _VIDEO_SOURCE_HPP_

#include "multithread.hpp"
#include <opencv2/opencv.hpp>

class VideoSource : public Thread {
public:
    virtual ~VideoSource() {}

    virtual bool isAvailable() = 0;

    virtual int getFrame(cv::Mat& target, int prev_id) = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual int getFPS() = 0;
};

#endif