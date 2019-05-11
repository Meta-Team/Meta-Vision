#ifndef _SERIAL_STATUS_HPP_
#define _SERIAL_STATUS_HPP_

#include "../global.hpp"
#include "serial.hpp"
#include "rm_protocol.hpp"
#include "../interfaces/multithread.hpp"
#include "../logging/timing.hpp"

#include <termios.h>
#include <boost/crc.hpp>

typedef boost::crc_optimal<8, 0x31, 0xFF, 0, true, true> rm_crc8_t;
typedef boost::crc_16_type rm_crc16_t;

class SerialStatus : public Thread {
public:
    SerialStatus(std::string serial_device, int baudrate = B115200);
    void thread_job();
    void send_gimbal(int yaw, int pitch);
    bool parse(unsigned char* data, unsigned int len);
    ~SerialStatus();

    rm_state_t rm_state;

private:
    int _packet_get_length(rm_protocol_t* packet);
    int _packet_generate_crc(rm_protocol_t* packet, int packet_len);
    int _packet_generate_header(rm_protocol_t* packet, int packet_len);
    int _packet_send(rm_protocol_t* packet);

    rm_crc8_t _crc8;
    rm_crc16_t _crc16;

    int _serial_fd = -1;

    /**
     * @brief Timing object to show operation per second statistics.
     */
    Timing _timing;
};

#endif