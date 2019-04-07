#ifndef _VIDEO_SOURCE_CAMERA_HPP_
#define _VIDEO_SOURCE_CAMERA_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_source.hpp"
#include "../logging/timing.hpp"
#include <mutex>

class VideoSourceCamera : public VideoSource {
private:
    cv::VideoCapture* _cap = NULL;
    cv::Mat _frame;
    std::mutex _frame_mutex;
    bool _available = false;
    int _id = 0;
    Timing _timing;
public:
    VideoSourceCamera(int id, int width, int height, int fps);
    ~VideoSourceCamera();

    void thread_job();

    bool isAvailable();
    int getFrame(cv::Mat& target, int prev_id);

    int getWidth();
    int getHeight();
    int getFPS();
};

#endif