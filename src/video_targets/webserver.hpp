#ifndef _VIDEO_TARGET_WEBSERVER_HPP_
#define _VIDEO_TARGET_WEBSERVER_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_target.hpp"
#include "../logging/timing.hpp"
#include <MJPEGWriter.h>
#include <queue>
#include <mutex>

/**
 * @brief Video target that runs a webserver to serve the frames.
 *        Uses the **MJPEGWriter** library in **lib** folder.
 */
class VideoTargetWebserver : public VideoTarget {
private:
    /**
     * @brief Webserver interface provided by **MJPEGWriter** library.
     */
    MJPEGWriter* _mjpegWriter = NULL;

    /**
     * @brief Queue for video frames waiting to be written to webserver.
     */
    std::queue<cv::Mat> _queue;

    /**
     * @brief Mutex to ensure thread safety of the queue.
     */
    std::mutex _queue_mutex;

    /**
     * @brief Timing object to show operation per second statistics.
     */
    Timing _timing;
public:
    VideoTargetWebserver(int port = 2019);
    ~VideoTargetWebserver();

    void thread_job();

    bool isAvailable();
    void writeFrame(cv::Mat& mat);
};

#endif