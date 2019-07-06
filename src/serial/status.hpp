#ifndef _SERIAL_STATUS_HPP_
#define _SERIAL_STATUS_HPP_

#include "../global.hpp"
#include "protocol.hpp"
#include "../interfaces/multithread.hpp"

#include <termios.h>
#include <boost/crc.hpp>

/** CRC8 algorithm used by RoboMaster referee system */
typedef boost::crc_optimal<8, 0x31, 0xFF, 0, true, true> rm_crc8_t;
/** CRC16 algorithm used by RoboMaster referee system */
typedef boost::crc_16_type rm_crc16_t;

/** Thread that receives & parses data from RoboMaster referee system, and sends instructions back.*/
class SerialStatus : public Thread {
public:
    /**
     * @brief Construct a new Serial Status object
     * 
     * @param serial_device Path to the serial port, usually `/dev/ttyS0`
     * @param baudrate Baudrate, usually B115200 (note that this is different from integer 115200!)
     */
    SerialStatus(std::string serial_device, int baudrate = B115200);

    /**
     * @brief Main thread that receives data from serial port
     */
    void thread_job();

    /**
     * @brief Send a command to RM board, asking it to aim to specified yaw/pitch angle
     * 
     * @param yaw Target yaw angle of the gimbal
     * @param pitch Target pitch angle of the gimbal
     */
    void send_gimbal(float yaw, float pitch, float distance);

    /**
     * @brief Parse packet from referee system and store the received data
     * 
     * @param data Pointer to first byte of packet
     * @param len Length of the packet
     * @return true if parse succeeds
     * @return false if parse fails, ex. cmd_id is unknown
     */
    bool parse(unsigned char* data, unsigned int len);
    ~SerialStatus();

    /** Storage of information received from referee system */
    RM_Protocol::rm_state_t rm_state;

private:
    /**
     * @brief Given a packet, return its expected length based on cmd_id
     * 
     * @param packet Pointer to first byte of packet
     * @return int Expected length of the packet
     */
    int _packet_get_length(RM_Protocol::rm_protocol_t* packet);

    /**
     * @brief Given a packet, fill its CRC fields
     * 
     * @param packet Pointer to first byte of packet
     * @param packet_len Length of the packet.
     *        if unspecified, will be determined with _packet_get_length().
     * @return int 0 if success, -1 if fail
     */
    int _packet_generate_crc(RM_Protocol::rm_protocol_t* packet, int packet_len = 0);

    /**
     * @brief Given a packet, fill its frame header for sending to referee system
     * 
     * @param packet Pointer to first byte of packet
     * @param packet_len Length of the packet.
     *        if unspecified, will be determined with _packet_get_length().
     * @return int 0 if success, -1 if fail
     */
    int _packet_generate_header(RM_Protocol::rm_protocol_t* packet, int packet_len = 0);

    /**
     * @brief Set necessary fields of a packet and send over serial to referee system
     * 
     * @param packet Pointer to first byte of packet
     * @return int 0 if success, -1 if fail
     */
    int _packet_send(RM_Protocol::rm_protocol_t* packet);

    /** Boost CRC8 computer for packet header checksum */
    rm_crc8_t _crc8;
    /** Boost CRC16 computer for whole packet checksum */
    rm_crc16_t _crc16;

    /** File Descriptor to serial port */
    int _serial_fd = -1;
};

#endif