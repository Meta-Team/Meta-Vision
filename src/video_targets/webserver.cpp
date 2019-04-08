#include "webserver.hpp"
#include "../logging/logging.hpp"
#include "../global.hpp"
#include <stdexcept>

using namespace std;
using namespace cv;

VideoTargetWebserver::VideoTargetWebserver(int port) {
    _mjpegWriter = new MJPEGWriter(port);

    _timing.set_name("Video Target/Web Server");

    cwarning << "Video Target/Web Server: Port " << port << endlog;

    // Start processing thread immediately
    thread_run();
}

VideoTargetWebserver::~VideoTargetWebserver() {
    thread_stop();  // IMPORTANT: not stopping here will cause job() still running
                    // when object get deleted, causing segfault
    if(_mjpegWriter != NULL) {
        _mjpegWriter->stop();
        delete _mjpegWriter;
    }
}

void VideoTargetWebserver::thread_job() {
    Mat local_frame;

    // Don't start webserver until first frame arrives
    // Otherwise
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

bool VideoTargetWebserver::isAvailable() { return true; }

void VideoTargetWebserver::writeFrame(Mat& mat) {
    // Limit queue size to prevent memory exhaustion
    if(_queue.size() > VIDEO_TARGET_QUEUE_SIZE) return;
    // Lock not needed here, although queue may exceed max size,
    // not a lot extra memory will be used, but performance can be better

    _queue_mutex.lock();
    _queue.push(mat);
    _queue_mutex.unlock();
}
