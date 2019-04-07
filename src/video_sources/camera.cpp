#include "camera.hpp"
#include "../logging/logging.hpp"
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace cv;

VideoSourceCamera::VideoSourceCamera(int id, int width, int height, int fps) {
    // Try to use different backends to start capture
    do {
        _cap = new VideoCapture(id + CAP_V4L2);
        if(_cap->isOpened()) break;

        _cap = new VideoCapture(id + CAP_ANY);
        if(_cap->isOpened()) break;

        throw std::invalid_argument("Failed to open camera");
    } while(0);

    _cap->set(CAP_PROP_FRAME_WIDTH, width);
    _cap->set(CAP_PROP_FRAME_HEIGHT, height);
    _cap->set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    _cap->set(CAP_PROP_FPS, fps);

    cwarning << "Video Source/Camera: " << getWidth() << "x" << getHeight() << " @" << getFPS() << endlog;

    _timing.set_name("Video Source/Camera");

    thread_run();
}

VideoSourceCamera::~VideoSourceCamera() {
    thread_stop();  // IMPORTANT: not stopping here will cause job() still running
                    // when _cap get deleted, causing segfault
    if(_cap != NULL) {
        _cap->release();
        delete _cap;
    }
}

void VideoSourceCamera::thread_job() {
    Mat local_frame;

    // Mark source ready to serve images
    _available = true;

    while(thread_should_run) {
        if(!_cap->read(local_frame)) continue;

        _frame_mutex.lock();
        _frame = local_frame;
        _id++;                  // Indicating it is a different frame now
        _frame_mutex.unlock();

        _timing.op_done();
    }

    // When file is closed, mark this source as unavailable
    _available = false;
    _timing.job_end();
    thread_should_run = false;
}

bool VideoSourceCamera::isAvailable() { return _available; }

int VideoSourceCamera::getFrame(Mat& target, int prev_id = 0) {
    unique_lock<mutex> lock(_frame_mutex);

    if(prev_id == _id) return prev_id;  // No new frame available yet

    target = _frame;                    // New frame available, do the copy
    return _id;
}

int VideoSourceCamera::getWidth() {
    return _cap->get(CAP_PROP_FRAME_WIDTH);
}

int VideoSourceCamera::getHeight() {
    return _cap->get(CAP_PROP_FRAME_HEIGHT);
}

int VideoSourceCamera::getFPS() {
    return _cap->get(CAP_PROP_FPS);
}
