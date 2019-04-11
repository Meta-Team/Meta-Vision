#ifndef _VIDEO_TARGET_DUMMY_HPP_
#define _VIDEO_TARGET_DUMMY_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_target.hpp"
#include "../logging/timing.hpp"
#include <queue>
#include <mutex>

/**
 * @brief Dummy video target, silently discards all frame.
 */
class VideoTargetDummy : public VideoTarget {
private:
    /**
     * @brief Counter of "unprocessed frames", used for statistical purposes.
     */
    int _counter = 0;

    /**
     * @brief Mutex to ensure thread safety of the counter.
     */
    std::mutex _counter_mutex;

    /**
     * @brief Timing object to show operation per second statistics.
     */
    Timing _timing;
public:
    VideoTargetDummy();
    ~VideoTargetDummy();

    void thread_job();

    bool isAvailable();
    void writeFrame(cv::Mat& mat);
};

#endif