#ifndef _MULTITHREAD_HPP_
#define _MULTITHREAD_HPP_

#include <thread>
#include <iostream>

class Thread {
private:
    std::thread* _t = NULL;
public:
    bool thread_should_run = false;

    virtual void thread_job() = 0;

    /* int run()
     * @description: create a new thread with assigned job.
     */
    int thread_run() {
        if(_t != NULL) return -1;
        thread_should_run = true;
        _t = new std::thread(&Thread::thread_job, this);
        return 0;
    }

    /* int join()
     * @description: wait until the thread stops and remove it from memory.
     */
    int thread_join() {
        if(_t == NULL) return -1;
        _t->join();
        delete _t;
        _t = NULL;
        return 0;
    }

    /* int stop()
     * @description: requests the thread to stop.
     * MUST use while(thread_should_run) instead of while(1).
     */
    int thread_stop() {
        thread_should_run = false;
        thread_join();
        return 0;
    }

    /* Destructor of Thread
     * @description: stops the thread if still running.
     */
    ~Thread() {
        if(_t != NULL) thread_stop();
    }
};

#endif