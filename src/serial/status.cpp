#include "status.hpp"
#include "../logging/logging.hpp"

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

SerialStatus::SerialStatus(string serial_device, int baudrate) {
    _serial_fd = open(serial_device.c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY | O_NDELAY);

    if(-1 == _serial_fd) {
        cerror << "Open serial device failed" << endl;
        return;
    }

    if(!isatty(_serial_fd)) {
        cerror << "Not a serial device" << endl;
        return;
    }

    struct termios config;
    if(tcgetattr(_serial_fd, &config) < 0) {
        cerror << "Failed to get serial configuration" << endl;
        return;
    }

    config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    config.c_oflag = 0;
    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    config.c_cflag &= ~(CSIZE | PARENB);
    config.c_cflag |= CS8;
    config.c_cc[VMIN]  = 1;
    config.c_cc[VTIME] = 0;

    if(cfsetispeed(&config, baudrate) < 0 || cfsetospeed(&config, baudrate) < 0) {
        cerror << "Failed to set port baudrate" << endl;
        return;
    }

    if(tcsetattr(_serial_fd, TCSAFLUSH, &config) < 0) {
        cerror << "Failed to apply serial settings" << endl;
        return;
    }

    _timing.set_name("Serial");

    thread_run();
}

void SerialStatus::thread_job() {
    unsigned char buf[257];
    unsigned int buf_pos = 0;
    unsigned int len = 0;

    while(thread_should_run) {
        // Look for packet header, 0xA5
        buf_pos = 0;
        while(thread_should_run) {
            if(0 == read(_serial_fd, buf + buf_pos, 1)) continue;
            if(0xA5 != buf[0]) continue;
            buf_pos = 1;
            break;
        }

        // Read the rest of packet header
        for(int i = 0; i < 4 && thread_should_run; i++) {
            while(0 == read(_serial_fd, buf + buf_pos, 1) && thread_should_run) {}
            buf_pos++;
        }
        if(!thread_should_run) break;

        // Verify header
        _crc8.reset();
        _crc8.process_bytes(buf, 4);
        if(_crc8.checksum() != buf[4]) {
            cerror << "Serial: Received invalid header of length " << buf_pos << endlog;
            continue;
        }

        // Read the cmd_id & data & frame_tail
        len = buf[1] + (buf[2] << 8) + 4;
        for(int i = 0; i < len && thread_should_run; i++) {
            while(0 == read(_serial_fd, buf + buf_pos, 1) && thread_should_run) {}
            buf_pos++;
        }
        if(!thread_should_run) break;

        // Verify whole packet
        _crc16.reset();
        _crc16.process_bytes(buf, 7 + len);
        if(_crc16.checksum() != (buf[7 + len] + (buf[8 + len] << 8))) {
            cerror << "Serial: Received invalid data of length " << buf_pos << endlog;
            continue;
        }

        parse(buf, buf_pos);
        _timing.op_done();
    }
    _timing.job_end();
}

void SerialStatus::send_gimbal(int d_yaw, int d_pitch) {
    // TODO: prepare packet and send it
}

bool SerialStatus::parse(unsigned char* data, unsigned int len) {
    // TODO: Understand the data
    return true;
}

SerialStatus::~SerialStatus() {
    if(-1 != _serial_fd) close(_serial_fd);
}