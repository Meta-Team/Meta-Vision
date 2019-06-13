#ifndef _TIMING_HPP_
#define _TIMING_HPP_

#include <chrono>
#include <string>

/**
 * @brief Calculates and prints the average op/s.
 */
class Timing {
private:
    /**
     * @brief Time point of previous printing onto console.
     */
    std::chrono::high_resolution_clock::time_point _prev_notify;

    /**
     * @brief Name of the job being counted.
     */
    std::string _name = "(unset)";

    /**
     * @brief Operations since last printing onto console.
     */
    int _ops_passed = 0;

    /**
     * @brief The interval in seconds to print onto console.
     */
    int _timing_interval;

public:
    Timing(const std::string& function_name);
    void set_name(std::string name);
    void set_name(const char* name);

    void op_done();
    ~Timing();
};

#define TIME_THIS Timing _timing(__PRETTY_FUNCTION__)
#define TIME_DONE _timing.op_done()

#endif