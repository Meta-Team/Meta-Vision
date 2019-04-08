#ifndef _VIDEO_TARGET_FILE_HPP_
#define _VIDEO_TARGET_FILE_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_target.hpp"
#include "../logging/timing.hpp"
#include <queue>
#include <mutex>

class VideoTargetFile : public VideoTarget {
private:
    cv::VideoWriter* _wri = NULL;
    std::queue<cv::Mat> _queue;
    std::mutex _queue_mutex;
    int _width, _height;
    Timing _timing;
public:
    VideoTargetFile(std::string folder, int width = 640, int height = 480, int fps = 30);
    ~VideoTargetFile();

    void thread_job();

    bool isAvailable();
    void writeFrame(cv::Mat& mat);
};

#endif