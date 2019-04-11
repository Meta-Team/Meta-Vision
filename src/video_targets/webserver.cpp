#include "webserver.hpp"
#include "../logging/logging.hpp"
#include "../global.hpp"
#include <stdexcept>

using namespace std;
using namespace cv;

/**
 * @brief Sets up a webserver serving video images.
 * 
 * @param port The port webserver is listening on.
 */
VideoTargetWebserver::VideoTargetWebserver(int port) {
    _mjpegWriter = new MJPEGWriter(port);

    _timing.set_name("Video Target/Web Server");

    cwarning << "Video Target/Web Server: Port " << port << endlog;

    // Start processing thread immediately
    thread_run();
}

/**
 * @brief Destroy the Video Target Webserver:: Video Target Webserver object
 */
VideoTargetWebserver::~VideoTargetWebserver() {
    thread_stop();  // IMPORTANT: not stopping here will cause job() still running
                    // when object get deleted, causing segfault
    if(_mjpegWriter != NULL) {
        _mjpegWriter->stop();
        delete _mjpegWriter;
    }
}

/**
 * @brief Main routine of the webserver thread.
 *        Pops image from queue and sends it to webserver.
 */
void VideoTargetWebserver::thread_job() {
    Mat local_frame;

    // Don't start webserver until first frame arrives
    // Otherwise webserver will crash
    do {
        while(_queue.empty()) {
            if(!thread_should_run) return;
        }

        _queue_mutex.lock();
        local_frame = _queue.front();
        _queue.pop();
        _queue_mutex.unlock();
        
        _mjpegWriter->write(local_frame);
        _mjpegWriter->start();

        _timing.op_done();
    } while(0);

    while(thread_should_run) {
        if(_queue.empty()) continue;
        _queue_mutex.lock();
        local_frame = _queue.front();
        _queue.pop();
        _queue_mutex.unlock();

        _mjpegWriter->write(local_frame);

        _timing.op_done();
    }
    _timing.job_end();
    _mjpegWriter->stop();
}

/**
 * @brief Checks if the webserver is available. Always return true,
 *        because the webserver is always ready, thanks to the queue.
 * 
 * @return true Always
 * @return false Never
 */
bool VideoTargetWebserver::isAvailable() { return true; }

/**
 * @brief Enqueues a frame to be sent to the webserver.
 * 
 * @param mat The frame to be sent to webserver
 */
void VideoTargetWebserver::writeFrame(Mat& mat) {
    // Limit queue size to prevent memory exhaustion
    if(_queue.size() > VIDEO_TARGET_QUEUE_SIZE) return;
    // Lock not needed here, although queue may exceed max size,
    // not a lot extra memory will be used, but performance can be better

    _queue_mutex.lock();
    _queue.push(mat);
    _queue_mutex.unlock();
}
