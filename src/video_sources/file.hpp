#ifndef _VIDEO_SOURCE_FILE_HPP_
#define _VIDEO_SOURCE_FILE_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_source.hpp"
#include <shared_mutex>

class VideoSourceFile : public VideoSource {
private:
    cv::VideoCapture* _cap = NULL;
    cv::Mat _frame;
    std::shared_mutex _frame_mutex;
    bool _available = false;
    int _id = 0;
public:
    VideoSourceFile(std::string filename);
    ~VideoSourceFile();

    void job();

    bool isAvailable();
    int getFrame(cv::Mat& target, int prev_id);
};

#endif