#ifndef _VIDEO_TARGET_DUMMY_HPP_
#define _VIDEO_TARGET_DUMMY_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_target.hpp"
#include "../logging/timing.hpp"
#include <queue>
#include <mutex>

class VideoTargetDummy : public VideoTarget {
private:
    int _counter = 0;
    std::mutex _counter_mutex;
    Timing _timing;
public:
    VideoTargetDummy();
    ~VideoTargetDummy();

    void thread_job();

    bool isAvailable();
    void writeFrame(cv::Mat& mat);
};

#endif