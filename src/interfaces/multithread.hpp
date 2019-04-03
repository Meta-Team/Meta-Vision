#ifndef _MULTITHREAD_HPP_
#define _MULTITHREAD_HPP_

#include <unistd.h>
#include <thread>
#include <iostream>

class Thread {
private:
    std::thread* t = NULL;
public:
    bool should_run = false;

    virtual void job() = 0;

    /* int run()
     * @description: create a new thread with assigned job.
     */
    int run() {
        if(t != NULL) return -1;
        should_run = true;
        t = new std::thread(&Thread::job, this);
        return 0;
    }

    /* int join()
     * @description: wait until the thread stops and remove it from memory.
     */
    int join() {
        if(t == NULL) return -1;
        t->join();
        delete t;
        t = NULL;
        return 0;
    }

    /* int stop()
     * @description: requests the thread to stop.
     * MUST use while(should_run) instead of while(1).
     */
    int stop() {
        should_run = false;
        join();
        return 0;
    }

    /* Destructor of Thread
     * @description: stops the thread if still running.
     */
    ~Thread() {
        if(t != NULL) stop();
    }
};

#endif