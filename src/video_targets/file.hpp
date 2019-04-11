#ifndef _VIDEO_TARGET_FILE_HPP_
#define _VIDEO_TARGET_FILE_HPP_

#include <opencv2/opencv.hpp>
#include "../interfaces/multithread.hpp"
#include "../interfaces/video_target.hpp"
#include "../logging/timing.hpp"
#include <queue>
#include <mutex>
#include <ctime>

/**
 * @brief Video target that writes frames into a video file.
 */
class VideoTargetFile : public VideoTarget {
private:
    /**
     * @brief OpenCV video writer.
     */
    cv::VideoWriter* _wri = NULL;

    /**
     * @brief Queue for video frames waiting to be written to file.
     */
    std::queue<cv::Mat> _queue;

    /**
     * @brief Mutex to ensure thread safety of the queue.
     */
    std::mutex _queue_mutex;

    /**
     * @brief Folder to store video files.
     */
    std::string _folder;

    /**
     * @brief Width of the saved video.
     */
    int _width;

    /**
     * @brief Height of the saved video.
     */
    int _height;

    /**
     * @brief FPS of the saved video.
     */
    int _fps;

    /**
     * @brief Time in seconds to start writing to a new video file.
     */
    int _segment_interval;

    /**
     * @brief The time when the previous video file was created.
     */
    time_t _prev_file_time;

    /**
     * @brief Timing object to show operation per second statistics.
     */
    Timing _timing;

    void _createNewFile();
public:
    VideoTargetFile(std::string folder, int width, int height, int fps, int segment_interval);
    ~VideoTargetFile();

    void thread_job();

    bool isAvailable();
    void writeFrame(cv::Mat& mat);
};

#endif