#ifndef _VIDEO_TARGET_HPP_
#define _VIDEO_TARGET_HPP_

#include "multithread.hpp"
#include "video_source.hpp"
#include <opencv2/opencv.hpp>

/**
 * @brief Video Target interface.
 * 
 */
class VideoTarget : public Thread {
public:
    /**
     * @brief Virtual destroyer, to make inherited class's destroyers work.
     */
    virtual ~VideoTarget() {}

    /**
     * @brief Returns whether the target is available.
     * 
     * @return true If the target is available.
     * @return false If the target is not ready.
     */
    virtual bool isAvailable() = 0;

    /**
     * @brief Writes a frame into the target.
     * 
     *        Note that some targets write the frame immediately, while others
     *        may enqueue it and process it afterwards, or even discard it.
     * 
     * @param mat Frame to be written
     */
    virtual void writeFrame(cv::Mat& mat) = 0;
};

#endif