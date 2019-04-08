#include "file.hpp"
#include "../logging/logging.hpp"
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace cv;

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

VideoSourceFile::~VideoSourceFile() {
    thread_stop();  // IMPORTANT: not stopping here will cause job() still running
                    // when _cap get deleted, causing segfault
    if(_cap != NULL) {
        _cap->release();
        delete _cap;
    }
}

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

bool VideoSourceFile::isAvailable() { return _available; }

int VideoSourceFile::getFrame(Mat& target, int prev_id = 0) {
    unique_lock<mutex> lock(_frame_mutex);

    if(prev_id == _id) return prev_id;  // No new frame available yet

    target = _frame;                    // New frame available, do the copy
    return _id;
}
int VideoSourceFile::getWidth() {
    return _cap->get(CAP_PROP_FRAME_WIDTH);
}

int VideoSourceFile::getHeight() {
    return _cap->get(CAP_PROP_FRAME_HEIGHT);
}

int VideoSourceFile::getFPS() {
    return _cap->get(CAP_PROP_FPS);
}
