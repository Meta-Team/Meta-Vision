#ifndef _TIMING_HPP_
#define _TIMING_HPP_

#include <chrono>
#include <string>

class Timing {
private:
    std::chrono::high_resolution_clock::time_point _prev_notify;
    std::string _name = "???";
    int _ops_passed = 0;

public:
    Timing();
    void set_name(std::string name);
    void set_name(const char* name);

    void op_done();
    void job_end();
};

#endif