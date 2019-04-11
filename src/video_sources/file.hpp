#ifndef _VIDEO_SOURCE_FILE_HPP_
#define _VIDEO_SOURCE_FILE_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_source.hpp"
#include "../logging/timing.hpp"
#include <mutex>

/**
 * @brief Video source that reads from a file.
 */
class VideoSourceFile : public VideoSource {
private:
    /**
     * @brief OpenCV video capture device.
     */
    cv::VideoCapture* _cap = NULL;

    /**
     * @brief Frame captured from the camera.
     */
    cv::Mat _frame;

    /**
     * @brief Mutex to ensure thread safety of the frame object.
     */
    std::mutex _frame_mutex;

    /**
     * @brief Boolean of whether video capture is ready.
     */
    bool _available = false;

    /**
     * @brief ID of the current frame, used to prevent repetition of frames.
     */
    int _id = 0;

    /**
     * @brief Timing object to show operation per second statistics.
     */
    Timing _timing;
public:
    VideoSourceFile(std::string filename);
    ~VideoSourceFile();

    void thread_job();

    bool isAvailable();
    int getFrame(cv::Mat& target, int prev_id);

    int getWidth();
    int getHeight();
    int getFPS();
};

#endif