#include "status.hpp"
#include "../logging/logging.hpp"
#include "../logging/timing.hpp"

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std;

SerialStatus::SerialStatus(string serial_device, speed_t baudrate) {

    cwarning << "Serial baudrate = " << baudrate;

    _serial_fd = open(serial_device.c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY | O_NDELAY);

    if(-1 == _serial_fd) {
        cerror << "Open serial device failed";
        return;
    }

    if(!isatty(_serial_fd)) {
        cerror << "Not a serial device";
        return;
    }

    struct termios config;
    if(tcgetattr(_serial_fd, &config) < 0) {
        cerror << "Failed to get serial configuration";
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
        cwarning << "Failed to set port baudrate";
    }

    if(tcsetattr(_serial_fd, TCSAFLUSH, &config) < 0) {
        cerror << "Failed to apply serial settings";
        return;
    }

    memset(&rm_state, 0, sizeof(rm_state));

    thread_run();
}

void SerialStatus::thread_job() {
    TIME_THIS;

    unsigned char buf[257];
    unsigned int buf_pos = 0;
    unsigned int len = 0;

    while (thread_should_run) {
        // Look for packet header, 0xA5
        buf_pos = 0;
        while (thread_should_run) {
            if (0 == read(_serial_fd, buf + buf_pos, 1)) continue;
            if (0xA5 != buf[0]) continue;
            buf_pos = 1;
            break;
        }

        // Read the rest of packet header
        for (int i = 0; i < 4 && thread_should_run; i++) {
            while (0 == read(_serial_fd, buf + buf_pos, 1) && thread_should_run) {}
            buf_pos++;
        }
        if (!thread_should_run) break;

        // Verify header
        _crc8.reset();
        _crc8.process_bytes(buf, 4);
        if (_crc8.checksum() != buf[4]) {
            cerror << "Serial: Received invalid header of length " << buf_pos;
            cerror << std::hex << (unsigned) buf[0] << " " << (unsigned) buf[1] << " " << (unsigned) buf[2] << " " << (unsigned) buf[3] << " " << (unsigned) buf[4];
            continue;
        }

        // Read the cmd_id & data & frame_tail
        len = buf[1] + (buf[2] << 8) + 4;
        for (unsigned int i = 0; i < len && thread_should_run; i++) {
            while (0 == read(_serial_fd, buf + buf_pos, 1) && thread_should_run) {}
            buf_pos++;
        }
        if (!thread_should_run) break;

        // Verify whole packet
        _crc16.reset();
        _crc16.process_bytes(buf, 7 + len);
        if (_crc16.checksum() != (buf[7 + len] + (buf[8 + len] << 8))) {
            cerror << "Serial: Received invalid data of length " << buf_pos;
            continue;
        }

        parse(buf, buf_pos);
        TIME_DONE;
    }
}

void SerialStatus::send_gimbal(float yaw, float pitch, float distance) {
    RM_Protocol::rm_protocol_t packet;
    packet.cmd_id = RM_CMDID_CUSTOM_GIMBAL_TARGET;
    packet.custom_gimbal_target.yaw = yaw;
    packet.custom_gimbal_target.pitch = pitch;
    packet.custom_gimbal_target.distance = distance;
    _packet_send(&packet);
}

int SerialStatus::_packet_get_length(RM_Protocol::rm_protocol_t *packet) {
    if (packet == NULL) {
        cerror << "NULL passed to packet_get_length!";
        return 0;
    }

    auto rm_map_result = RM_Protocol::rm_map_cmdid_packetsize.find(packet->cmd_id);
    if (rm_map_result == RM_Protocol::rm_map_cmdid_packetsize.end()) {
        cerror << "Unknown cmdid " << packet->cmd_id << ", cannot generate CRC!";
        return 0;
    }
    return rm_map_result->second;
}

int SerialStatus::_packet_generate_crc(RM_Protocol::rm_protocol_t *packet, int packet_len) {
    if (packet == NULL) {
        cerror << "NULL passed to packet_generate_crc!";
        return FAIL;
    }

    uint8_t *packet_uint8 = (uint8_t *) packet;
    if (!packet_len) packet_len = _packet_get_length(packet);
    if (!packet_len) return FAIL;

    // Create separate CRC object here for thread safety
    rm_crc8_t local_crc8;
    local_crc8.reset();
    local_crc8.process_bytes(packet_uint8, sizeof(RM_Protocol::rm_frame_header_t) - 1);
    packet->frame_header.crc8 = local_crc8.checksum();

    rm_crc16_t local_crc16;
    local_crc16.reset();
    local_crc16.process_bytes(packet_uint8, packet_len - 2);
    uint16_t crc16_checksum = local_crc16.checksum();

    // Update CRC16 field depending on packet size
    packet_uint8[packet_len - 2] = crc16_checksum & 0xff;
    packet_uint8[packet_len - 1] = (crc16_checksum >> 8) & 0xff;

    return SUCCESS;
}

int SerialStatus::_packet_generate_header(RM_Protocol::rm_protocol_t *packet, int packet_len) {
    static uint8_t packet_tx_seq = 0;

    if (packet == NULL) {
        cerror << "NULL passed to packet_generate_header!";
        return FAIL;
    }

    if (!packet_len) packet_len = _packet_get_length(packet);
    if (!packet_len) return FAIL;

    packet->frame_header.sof = 0xA5;
    packet->frame_header.data_length = packet_len - 9;
    packet->frame_header.seq = packet_tx_seq++;
    if (FAIL == _packet_generate_crc(packet, packet_len)) return FAIL;

    return SUCCESS;
}

int SerialStatus::_packet_send(RM_Protocol::rm_protocol_t *packet) {
    int packet_len = _packet_get_length(packet);
    if (!packet_len) return FAIL;

    if (FAIL == _packet_generate_header(packet, packet_len)) return FAIL;
    int ret = write(_serial_fd, packet, packet_len);
    if (ret < 0) cerror << "packet_send fail: " << ret;
    return ret;
}

bool SerialStatus::parse(unsigned char *data, unsigned int len) {
    RM_Protocol::rm_protocol_t *rm_protocol = (RM_Protocol::rm_protocol_t *) data;
    switch (rm_protocol->cmd_id) {
        case RM_CMDID_GAME_STATE:
            rm_state.game_state = rm_protocol->game_state;
            break;
        case RM_CMDID_GAME_RESULT:
            rm_state.game_result = rm_protocol->game_result;
            break;
        case RM_CMDID_ROBOT_ALIVE:
            rm_state.robot_alive = rm_protocol->robot_alive;
            break;
        case RM_CMDID_REGION_EVENT:
            rm_state.region_event = rm_protocol->region_event;
            break;
        case RM_CMDID_SUPPLY_ACTION:
            rm_state.supply_action = rm_protocol->supply_action;
            break;
        case RM_CMDID_SUPPLY_REQUEST:
            break;
        case RM_CMDID_ROBOT_STATE:
            rm_state.robot_state = rm_protocol->robot_state;
            break;
        case RM_CMDID_POWER_HEAT:
            rm_state.power_heat = rm_protocol->power_heat;
            break;
        case RM_CMDID_ROBOT_POSITION:
            rm_state.robot_position = rm_protocol->robot_position;
            break;
        case RM_CMDID_ROBOT_BUFF:
            rm_state.robot_buff = rm_protocol->robot_buff;
            break;
        case RM_CMDID_DRONE_ENERGY:
            rm_state.drone_energy = rm_protocol->drone_energy;
            break;
        case RM_CMDID_DAMAGE:
            rm_state.damage = rm_protocol->damage;
            break;
        case RM_CMDID_BULLET_STATE:
            rm_state.bullet_state = rm_protocol->bullet_state;
            break;
        case RM_CMDID_CUSTOM_GIMBAL_CURRENT:
            rm_state.custom_gimbal_current = rm_protocol->custom_gimbal_current;
            cwarning << "Get custom current: Pitch " << rm_state.custom_gimbal_current.pitch
                     << ", Yaw " << rm_state.custom_gimbal_current.yaw;
            break;
        case RM_CMDID_CUSTOM_ENEMY_COLOR:
            rm_state.custom_enemy_color = rm_protocol->custom_enemy_color;
            break;
        default:
            cerror << "Unrecognized cmd_id: " << rm_protocol->cmd_id;
            return false;
    }

    return true;
}

SerialStatus::~SerialStatus() {
    if (-1 != _serial_fd) close(_serial_fd);
}
