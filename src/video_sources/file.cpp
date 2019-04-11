#include "file.hpp"
#include "../logging/logging.hpp"
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace cv;

/**
 * @brief Opens a video file as a video source. Used for debugging.
 * 
 * @param filename The video file to be opened.
 */
VideoSourceFile::VideoSourceFile(std::string filename) {
    // Try to use different backends to start capture
    do {
        _cap = new VideoCapture(filename, CAP_V4L2);
        if(_cap->isOpened()) break;

        _cap = new VideoCapture(filename, CAP_ANY);
        if(_cap->isOpened()) break;

        throw std::invalid_argument("Failed to open camera");
    } while(0);

    cwarning << "Video Source/File: " << getWidth() << "x" << getHeight() << " @" << getFPS() << endlog;

    _timing.set_name("Video Source/File");

    thread_run();
}

/**
 * @brief Destroy the Video Source File:: Video Source File object
 */
VideoSourceFile::~VideoSourceFile() {
    thread_stop();  // IMPORTANT: not stopping here will cause job() still running
                    // when _cap get deleted, causing segfault
    if(_cap != NULL) {
        _cap->release();
        delete _cap;
    }
}

/**
 * @brief Main thread that reads frames from the video file and waits,
 *        to simulate the behavior of a real camera.
 *
 *        Note that the actual FPS will be a bit lower than the source file due to waiting.
 */
void VideoSourceFile::thread_job() {
    Mat local_frame;
    chrono::high_resolution_clock::time_point op_timepoint = chrono::high_resolution_clock::now();
    chrono::duration<int, std::micro> interval((int) (1000000 / _cap->get(CAP_PROP_FPS)));

    // Source ready to serve images
    _available = true;

    while(thread_should_run) {
        if(!_cap->read(local_frame)) break;

        _frame_mutex.lock();
        _frame = local_frame;
        _id++;                  // Indicating it is a different frame now
        _frame_mutex.unlock();

        _timing.op_done();
        
        // Limit video reading speed to avoid excessive frame dropping
        op_timepoint += interval;
        std::this_thread::sleep_until(op_timepoint);
    }

    // When file is closed, mark this source as unavailable
    _available = false;
    _timing.job_end();
    thread_should_run = false;
}

/**
 * @brief Returns whether the file is available.
 * 
 * @return true When the file is available.
 * @return false When the file is unavailable or done (reached the end).
 */
bool VideoSourceFile::isAvailable() { return _available; }

/**
 * @brief Checks if a new frame is available by comparing the
 *        **previous id** with the **current id**.
 *        When it is, copies the new frame.
 * 
 * @param target Target of the frame to be copied.
 * @param prev_id ID of the previous frame.
 * @return int 
 */
int VideoSourceFile::getFrame(Mat& target, int prev_id = 0) {
    unique_lock<mutex> lock(_frame_mutex);

    if(prev_id == _id) return prev_id;  // No new frame available yet

    target = _frame;                    // New frame available, do the copy
    return _id;
}

/**
 * @brief Returns the actual frame width of the video.
 * 
 * @return int actual frame width of the video
 */
int VideoSourceFile::getWidth() {
    return _cap->get(CAP_PROP_FRAME_WIDTH);
}

/**
 * @brief Returns the actual frame height of the video.
 * 
 * @return int actual frame height of the video
 */
int VideoSourceFile::getHeight() {
    return _cap->get(CAP_PROP_FRAME_HEIGHT);
}

/**
 * @brief Returns the actual FPS of the video.
 * 
 * @return int actual FPS of the video
 */
int VideoSourceFile::getFPS() {
    return _cap->get(CAP_PROP_FPS);
}
