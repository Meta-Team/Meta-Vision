#include "timing.hpp"
#include "logging.hpp"
#include "../global.hpp"
#include "../entrypoint.hpp"

using namespace std;

/**
 * @brief Construct a new Timing:: Timing object, and starts counting.
 */
Timing::Timing(const std::string& function_name) {
    _prev_notify = chrono::high_resolution_clock::now();
    _timing_interval = mainFunction.config["system"]["timing_interval"].as<int>();
    _name = function_name;
}

/**
 * @brief Sets the name fo the current counter. Used when printing to console.
 * 
 * @param name New name for the current counter.
 */
void Timing::set_name(std::string name) {
    _name = name;
}

/**
 * @brief Sets the name fo the current counter. Used when printing to console.
 * 
 * @param name New name for the current counter.
 */
void Timing::set_name(const char* name) {
    _name = string(name);
}

/**
 * @brief Called at the end of every operation, used for counting.
 *        When a specified interval passed, the average operations per second
 *        is printed onto the console.
 */
void Timing::op_done() {
    chrono::high_resolution_clock::time_point now = chrono::high_resolution_clock::now();
    double time_passed = chrono::duration_cast<chrono::duration<double>>(now - _prev_notify).count();
    _ops_passed++;

    if(time_passed >= _timing_interval) {
        _prev_notify = now;
        Logging(_name) << color(FG_GREEN) << (1.0 * _ops_passed / time_passed) << color(FG_DEFAULT) << " op/s (" << color(FG_GREEN) << _ops_passed << color(FG_DEFAULT) << " ops in " << color(FG_GREEN) << time_passed << color(FG_DEFAULT) << "s)";
        _ops_passed = 0;
    }
}

/**
 * @brief Immediately prints the rest operations per second, regardless of interval.
 */
Timing::~Timing() {
    chrono::high_resolution_clock::time_point now = chrono::high_resolution_clock::now();
    double time_passed = chrono::duration_cast<chrono::duration<double>>(now - _prev_notify).count();

    _prev_notify = now;
    Logging(_name) << color(FG_GREEN) << (1.0 * _ops_passed / time_passed) << color(FG_DEFAULT) << " op/s (" << color(FG_GREEN) << _ops_passed << color(FG_DEFAULT) << " ops in " << color(FG_GREEN) << time_passed << color(FG_DEFAULT) << "s, " << color(FG_RED) << "end" << color(FG_DEFAULT) << ")";
}