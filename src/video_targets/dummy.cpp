#include "dummy.hpp"
#include "../global.hpp"
#include <stdexcept>

using namespace std;
using namespace cv;

/**
 * @brief A dummy video target, which does almost nothing.
 *        Used only for debugging and benchmarking.
 */
VideoTargetDummy::VideoTargetDummy() {
    _timing.set_name("Video Target/Dummy");

    // Start processing thread immediately
    thread_run();
}

/**
 * @brief Destroy the Video Target Dummy:: Video Target Dummy object
 */
VideoTargetDummy::~VideoTargetDummy() {
    thread_stop();
}

/**
 * @brief Main routine, only decreases a counter for statistical purposes, does nothing otherwise
 */
void VideoTargetDummy::thread_job() {
    while(thread_should_run) {
        if(_counter == 0) continue;
        _counter_mutex.lock();
        _counter--;
        _counter_mutex.unlock();
        
        _timing.op_done();
    }
    _timing.job_end();
}

/**
 * @brief Returns whether the target is ready. Returns true all the time obviously.
 * 
 * @return true Always
 * @return false Never
 */
bool VideoTargetDummy::isAvailable() { return true; }

/**
 * @brief Sends a frame to be written to nowhere.
 *        Practically only increases a counter for statistical purposes.
 * 
 * @param mat Frame to be sent
 */
void VideoTargetDummy::writeFrame(Mat& mat) {
    _counter_mutex.lock();
    _counter++;
    _counter_mutex.unlock();
}
