#include "file.hpp"
#include "../global.hpp"
#include <stdexcept>

using namespace std;
using namespace cv;

VideoTargetFile::VideoTargetFile(string filename, int width, int height) {
    _width = width;
    _height = height;

    // Open a 30FPS MP4 writer
    _wri = new VideoWriter(filename, CV_FOURCC('m', 'p', '4', 'v'), 30, Size(width, height));
    if(!_wri->isOpened()) throw std::invalid_argument("Invalid output file");

    // Start processing thread immediately
    run();
}

VideoTargetFile::~VideoTargetFile() {
    stop();     // IMPORTANT: not stopping here will cause job() still running
                // when _wri get deleted, causing segfault
    if(_wri != NULL) {
        _wri->release();
        delete _wri;
    }
}

void VideoTargetFile::job() {
    Mat local_frame, local_frame_resized;
    while(should_run) {
        if(_queue.empty()) continue;
        _queue_mutex.lock();
        local_frame = _queue.front();
        _queue.pop();
        _queue_mutex.unlock();

        resize(local_frame, local_frame_resized, Size(_width, _height));
        _wri->write(local_frame_resized);
    }
}

bool VideoTargetFile::isAvailable() { return true; }

void VideoTargetFile::writeFrame(Mat& mat) {
    // Limit queue size to prevent memory exhaustion
    if(_queue.size() > VIDEO_TARGET_QUEUE_SIZE) return;
    // Lock not needed here, although queue may exceed max size,
    // not a lot extra memory will be used, but performance can be better

    _queue_mutex.lock();
    _queue.push(mat);
    _queue_mutex.unlock();
}
