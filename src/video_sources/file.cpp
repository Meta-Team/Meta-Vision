#include "file.hpp"
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace cv;

VideoSourceFile::VideoSourceFile(std::string filename) {
    _cap = new VideoCapture(filename);
    if(!_cap->isOpened()) throw std::invalid_argument("Invalid input file");

    run();
}

VideoSourceFile::~VideoSourceFile() {
    stop();     // IMPORTANT: not stopping here will cause job() still running
                // when _cap get deleted, causing segfault
    if(_cap != NULL) {
        _cap->release();
        delete _cap;
    }
}

void VideoSourceFile::job() {
    Mat local_frame;
    double fps = _cap->get(CAP_PROP_FPS);

    // Source ready to serve images
    _available = true;

    while(should_run) {
        if(!_cap->read(local_frame)) break;

        _frame_mutex.lock();
        _frame = local_frame;
        _id++;                  // Indicating it is a different frame now
        _frame_mutex.unlock();

        // Limit video reading speed to avoid excessive frame dropping
        // TODO: Take video reading time into account, instead of simple 1/fps
        std::this_thread::sleep_for(std::chrono::microseconds((long long) (1000000 / fps)));
    }

    // When file is closed, mark this source as unavailable
    _available = false;
}

bool VideoSourceFile::isAvailable() { return _available; }

int VideoSourceFile::getFrame(Mat& target, int prev_id = 0) {
    shared_lock lock(_frame_mutex);

    if(prev_id == _id) return prev_id;  // No new frame available yet

    target = _frame;                    // New frame available, do the copy
    return _id;
}