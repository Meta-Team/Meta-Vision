#ifndef _VIDEO_SOURCE_HPP_
#define _VIDEO_SOURCE_HPP_

#include "multithread.hpp"
#include <opencv2/opencv.hpp>

/**
 * @brief Video Source interface.
 */
class VideoSource : public Thread {
public:
    /**
     * @brief Virtual destroyer, to make inherited class's destroyers work.
     */
    virtual ~VideoSource() {}

    /**
     * @brief Returns whether the source is available.
     * 
     * @return true If the source is available.
     * @return false If the source is not ready.
     */
    virtual bool isAvailable() = 0;

    /**
     * @brief Checks if a new frame is available by comparing the
     *        **previous id** with the **current id**.
     *        When it is, copies the new frame.
     * 
     * @param target Target of the frame to be copied.
     * @param prev_id ID of the previous frame.
     * @return int 
     */
    virtual int getFrame(cv::Mat& target, int prev_id) = 0;

    /**
     * @brief Returns the actual frame width.
     * 
     * @return int Actual frame width
     */
    virtual int getWidth() = 0;

    /**
     * @brief Returns the actual frame height.
     * 
     * @return int Actual frame height
     */
    virtual int getHeight() = 0;

    /**
     * @brief Returns the actual FPS.
     * 
     * @return int Actual FPS
     */
    virtual int getFPS() = 0;
};

#endif