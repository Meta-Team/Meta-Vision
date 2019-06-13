#include "file.hpp"
#include "../logging/logging.hpp"
#include "../global.hpp"
#include <stdexcept>

using namespace std;
using namespace cv;

/**
 * @brief Starts writing video into a folder, with specified parameters.
 *        To handle crashes/power cuts better, the video file is split
 *        every **segment_interval** seconds, so only the last file will be lost
 *        if something bad ever happens.
 * 
 * @param folder Folder to store the saved videos.
 * @param width Frame width of the saved video file.
 * @param height Frame height of the saved video file.
 * @param fps Frames per second of the saved video file.
 * @param segment_interval Interval in seconds when the video file will be split.
 */
VideoTargetFile::VideoTargetFile(string folder, int width, int height, int fps, int segment_interval)
: _folder(folder), _width(width), _height(height), _fps(fps), _segment_interval(segment_interval) {
    // Create filename based on current datetime
    time_t rawtime;
    struct tm * timeinfo;
    char filename[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filename,sizeof(filename),"record-%Y-%m-%d-%H-%M-%S.mp4",timeinfo);
    string filepath = _folder + "/" + string(filename);

    _createNewFile();

    _timing.set_name("Video Target/File");

    // Start processing thread immediately
    thread_run();
}

/**
 * @brief Destroy the Video Target File:: Video Target File object
 */
VideoTargetFile::~VideoTargetFile() {
    thread_stop();  // IMPORTANT: not stopping here will cause job() still running
                    // when _wri get deleted, causing segfault
    if(_wri != NULL) {
        _wri->release();
        delete _wri;
    }
}

/**
 * @brief Create a new file in the folder, based on the current datetime.
 */
void VideoTargetFile::_createNewFile() {
    if(_wri != NULL) {
        _wri->release();
        delete _wri;
    }
    
    struct tm * timeinfo;
    char filename[80];
    time(&_prev_file_time);
    timeinfo = localtime(&_prev_file_time);
    strftime(filename,sizeof(filename),"record-%Y-%m-%d-%H-%M-%S.mp4",timeinfo);
    string filepath = _folder + "/" + string(filename);

    // Open a MP4 writer
    _wri = new VideoWriter(filepath, VideoWriter::fourcc('m', 'p', '4', 'v'), _fps, Size(_width, _height));
    if(!_wri->isOpened()) throw std::invalid_argument("Invalid output file");

    cwarning << "Video Target/File: " << filepath;
}

/**
 * @brief Main routine of the thread.
 *        Pops image from queue and writes it to disk.
 */
void VideoTargetFile::thread_job() {
    Mat local_frame, local_frame_resized;
    time_t current_time;
    while(thread_should_run) {
        time(&current_time);
        if(difftime(current_time, _prev_file_time) >= _segment_interval) {
            _createNewFile();
        }

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

/**
 * @brief Checks if the writer is available. Always return true,
 *        because the writer is always ready, thanks to the queue.
 * 
 * @return true Always
 * @return false Never
 */
bool VideoTargetFile::isAvailable() { return true; }

/**
 * @brief Enqueues a frame to be written to disk.
 * 
 * @param mat The frame to be writte to disk
 */
void VideoTargetFile::writeFrame(Mat& mat) {
    // Limit queue size to prevent memory exhaustion
    if(_queue.size() > VIDEO_TARGET_QUEUE_SIZE) return;
    // Lock not needed here, although queue may exceed max size,
    // not a lot extra memory will be used, but performance can be better

    _queue_mutex.lock();
    _queue.push(mat);
    _queue_mutex.unlock();
}
