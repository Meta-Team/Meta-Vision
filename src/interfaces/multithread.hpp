#ifndef _MULTITHREAD_HPP_
#define _MULTITHREAD_HPP_

#include <thread>
#include <iostream>

/**
 * @brief Multithread interface.
 */
class Thread {
private:
    /**
     * @brief The current thread object.
     */
    std::thread* _t = NULL;
public:
    /**
     * @brief Whether the loop should continue running.
     */
    bool thread_should_run = false;

    /**
     * @brief The actual code of the thread, to be started as a separate thread.
     */
    virtual void thread_job() = 0;

    /**
     * @brief Create a new thread with assigned job.
     * 
     * @return int 0 if success, -1 if not.
     */
    int thread_run() {
        if(_t != NULL) return -1;
        thread_should_run = true;
        _t = new std::thread(&Thread::thread_job, this);
        return 0;
    }

    /**
     * @brief Wait until the thread stops and remove it from memory.
     * 
     * @return int 0 if success, -1 if not.
     */
    int thread_join() {
        if(_t == NULL) return -1;
        _t->join();
        delete _t;
        _t = NULL;
        return 0;
    }

    /**
     * @brief Requests the thread to stop.
     *        MUST use while(thread_should_run) instead of while(1) in the job loop.
     * 
     * @return int 0 if success, -1 if not.
     */
    int thread_stop() {
        thread_should_run = false;
        thread_join();
        return 0;
    }

    /**
     * @brief Stops the thread if still running.
     */
    virtual ~Thread() {
        if(_t != NULL) thread_stop();
    }
};

#endif