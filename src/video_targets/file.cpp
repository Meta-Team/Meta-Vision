#include "file.hpp"
#include "../logging/logging.hpp"
#include "../global.hpp"
#include <stdexcept>
#include <ctime>

using namespace std;
using namespace cv;

VideoTargetFile::VideoTargetFile(string folder, int width, int height, int fps) {
    _width = width;
    _height = height;

    // Create filename based on current datetime
    time_t rawtime;
    struct tm * timeinfo;
    char filename[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filename,sizeof(filename),"record-%Y-%m-%d-%H-%M-%S.mp4",timeinfo);
    string filepath = folder + "/" + string(filename);

    // Open a MP4 writer
    _wri = new VideoWriter(filepath, VideoWriter::fourcc('m', 'p', '4', 'v'), fps, Size(width, height));
    if(!_wri->isOpened()) throw std::invalid_argument("Invalid output file");

    cwarning << "Video Target/File: " << filepath << endlog;

    _timing.set_name("Video Target/File");

    // Start processing thread immediately
    thread_run();
}

VideoTargetFile::~VideoTargetFile() {
    thread_stop();  // IMPORTANT: not stopping here will cause job() still running
                    // when _wri get deleted, causing segfault
    if(_wri != NULL) {
        _wri->release();
        delete _wri;
    }
}

void VideoTargetFile::thread_job() {
    Mat local_frame, local_frame_resized;
    while(thread_should_run) {
        if(_queue.empty()) continue;
        _queue_mutex.lock();
        local_frame = _queue.front();
        _queue.pop();
        _queue_mutex.unlock();

        resize(local_frame, local_frame_resized, Size(_width, _height));
        _wri->write(local_frame_resized);

        _timing.op_done();
    }
    _timing.job_end();
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
