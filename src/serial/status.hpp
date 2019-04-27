#ifndef _SERIAL_STATUS_HPP_
#define _SERIAL_STATUS_HPP_

#include "serial.hpp"
#include "../interfaces/multithread.hpp"
#include "../logging/timing.hpp"

#include <boost/crc.hpp>

class SerialStatus : public Thread {
public:
    SerialStatus(std::string serial_device, int baudrate);
    void thread_job();
    void send_gimbal(int d_yaw, int d_pitch);
    bool parse(unsigned char* data, unsigned int len);
    ~SerialStatus();

private:
    boost::crc_optimal<8, 0x31, 0, 0, true, true> _crc8;
    boost::crc_16_type _crc16;

    int _serial_fd = -1;

    /**
     * @brief Timing object to show operation per second statistics.
     */
    Timing _timing;
};

#endif