#include "file.hpp"
#include <stdexcept>

using namespace std;
using namespace cv;

VideoSourceFile::VideoSourceFile(std::string filename) {
    _cap = new VideoCapture(filename);
    if(!_cap->isOpened()) {
        throw std::invalid_argument("Invalid input file");
    }

    // Immediately read the first frame to prevent exception
    if(!_cap->read(_frame)) {
        throw std::runtime_error("Input file ended");
    }

    run();
}

VideoSourceFile::~VideoSourceFile() {
    stop();
    if(_cap != NULL) {
        _cap->release();
        delete _cap;
    }
}

void VideoSourceFile::job() {
    Mat local_frame;
    _available = true;
    while(should_run) {
        if(!_cap->read(local_frame)) break;

        _frame_mutex.lock();
        _frame = local_frame;
        _id++;
        _frame_mutex.unlock();
    }
    _available = false;
}

bool VideoSourceFile::isAvailable() { return _available; }

int VideoSourceFile::getFrame(Mat& target, int prev_id = 0) {
    shared_lock lock(_frame_mutex);
    if(prev_id == _id) return prev_id;

    target = _frame;
    return _id;
}