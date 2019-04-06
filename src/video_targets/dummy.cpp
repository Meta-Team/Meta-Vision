#include "dummy.hpp"
#include "../global.hpp"
#include <stdexcept>

using namespace std;
using namespace cv;

VideoTargetDummy::VideoTargetDummy() {
    _timing.set_name("Video Target/Dummy");

    // Start processing thread immediately
    thread_run();
}

VideoTargetDummy::~VideoTargetDummy() {
    thread_stop();
}

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

bool VideoTargetDummy::isAvailable() { return true; }

void VideoTargetDummy::writeFrame(Mat& mat) {
    _counter_mutex.lock();
    _counter++;
    _counter_mutex.unlock();
}
