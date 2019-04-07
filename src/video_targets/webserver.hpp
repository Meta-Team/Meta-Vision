#ifndef _VIDEO_TARGET_WEBSERVER_HPP_
#define _VIDEO_TARGET_WEBSERVER_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_target.hpp"
#include "../logging/timing.hpp"
#include <MJPEGWriter.h>
#include <queue>
#include <mutex>

class VideoTargetWebserver : public VideoTarget {
private:
    MJPEGWriter* _mjpegWriter = NULL;
    std::queue<cv::Mat> _queue;
    std::mutex _queue_mutex;
    Timing _timing;
public:
    VideoTargetWebserver(int port = 2019);
    ~VideoTargetWebserver();

    void thread_job();

    bool isAvailable();
    void writeFrame(cv::Mat& mat);
};

#endif