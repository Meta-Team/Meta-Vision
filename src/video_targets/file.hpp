#ifndef _VIDEO_TARGET_FILE_HPP_
#define _VIDEO_TARGET_FILE_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_target.hpp"
#include <queue>
#include <mutex>

class VideoTargetFile : public VideoTarget {
private:
    cv::VideoWriter* _wri = NULL;
    std::queue<cv::Mat> _queue;
    std::mutex _queue_mutex;
    int _width, _height;
public:
    VideoTargetFile(std::string filename, int width = 640, int height = 480);
    ~VideoTargetFile();

    void job();

    bool isAvailable();
    void writeFrame(cv::Mat& mat);
};

#endif