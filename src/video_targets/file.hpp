#ifndef _VIDEO_TARGET_FILE_HPP_
#define _VIDEO_TARGET_FILE_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_target.hpp"
#include "../logging/timing.hpp"
#include <queue>
#include <mutex>
#include <ctime>

class VideoTargetFile : public VideoTarget {
private:
    cv::VideoWriter* _wri = NULL;
    std::queue<cv::Mat> _queue;
    std::mutex _queue_mutex;

    std::string _folder;
    int _width, _height, _fps, _segment_interval;

    Timing _timing;
    time_t _prev_file_time;

    void _createNewFile();
public:
    VideoTargetFile(std::string folder, int width, int height, int fps, int segment_interval);
    ~VideoTargetFile();

    void thread_job();

    bool isAvailable();
    void writeFrame(cv::Mat& mat);
};

#endif