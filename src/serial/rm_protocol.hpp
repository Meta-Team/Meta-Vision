#ifndef _RM_PROTOCOL_HPP_
#define _RM_PROTOCOL_HPP_

#include <stdint.h>
#include <map>

#define RM_FRAME_HEADER_LEN     5

#define RM_CMDID_GAME_STATE     0x0001
#define RM_CMDID_GAME_RESULT    0x0002
#define RM_CMDID_ROBOT_ALIVE    0x0003
#define RM_CMDID_REGION_EVENT   0x0101
#define RM_CMDID_SUPPLY_ACTION  0x0102
#define RM_CMDID_SUPPLY_REQUEST 0x0103
#define RM_CMDID_ROBOT_STATE    0x0201
#define RM_CMDID_POWER_HEAT     0x0202
#define RM_CMDID_ROBOT_POSITION 0x0203
#define RM_CMDID_ROBOT_BUFF     0x0204
#define RM_CMDID_DRONE_ENERGY   0x0205
#define RM_CMDID_DAMAGE         0x0206
#define RM_CMDID_BULLET_STATE   0x0207

#define RM_CMDID_CUSTOM_GIMBAL_CURRENT  0xff00
#define RM_CMDID_CUSTOM_GIMBAL_TARGET   0xff01

extern std::map<int, int> rm_map_cmdid_packetsize;

typedef struct __attribute__((packed)) {
    uint8_t type : 4;
    uint8_t stage: 4;
    uint16_t remain_time;
} game_state_t;

typedef struct {
    uint8_t winner;
} game_result_t;

typedef struct {
    uint16_t alive;
} robot_alive_t;

typedef struct __attribute__((packed)) {
    uint8_t own_drone_occupied : 2;
    uint8_t own_supply1_occupied : 1;
    uint8_t own_supply2_occupied : 1;
    uint8_t own_supply3_occupied : 1;
    uint8_t own_energy_state : 2;
    uint8_t own_gate_occupied : 1;
    uint8_t own_fort_occupied : 1;
    uint8_t own_resource_occupied : 1;
    uint8_t own_base_defense : 2;
    uint8_t icra_red_defense_bonus : 2;
    uint8_t icra_blue_defense_bonus : 2;
} region_event_t;

typedef struct {
    uint8_t supply_port_id;
    uint8_t supplied_robot_id;
    uint8_t supply_port_status;
    uint8_t supply_bullet_count;
} supply_action_t;

typedef struct {
    uint8_t supply_port_id;
    uint8_t supplied_robot_id;
    uint8_t supply_bullet_count;
} supply_request_t;

typedef struct {
    uint8_t id;
    uint8_t level;
    uint16_t hp;
    uint16_t hp_max;
    uint16_t cooling_17mm;
    uint16_t heatmax_17mm;
    uint16_t cooling_42mm;
    uint16_t heatmax_42mm;
    uint8_t power_state;
} robot_state_t;

typedef struct {
    uint16_t chassis_volt;
    uint16_t chassis_current;
    float chassis_power;
    uint16_t chassis_power_buffer;
    uint16_t heat_17mm;
    uint16_t heat_42mm;
} power_heat_t;

typedef struct {
    float x;
    float y;
    float z;
    float yaw;
} robot_position_t;

typedef struct {
    uint8_t buff;
} robot_buff_t;

typedef struct {
    uint8_t energy_point;
    uint8_t attack_time;
} drone_energy_t;

typedef struct __attribute__((packed)) {
    uint8_t armor_id : 4;
    uint8_t damage_type : 4;
} damage_t;

typedef struct {
    uint8_t type;
    uint8_t freq;
    float speed;
} bullet_state_t;

typedef struct {
    uint16_t yaw;
    uint16_t pitch;
} custom_gimbal_current_t;

typedef struct {
    uint16_t yaw;
    uint16_t pitch;
} custom_gimbal_target_t;

typedef struct {
    uint8_t sof;
    uint16_t data_length;
    uint8_t seq;
    uint8_t crc8;
} rm_frame_header_t;

typedef struct {
    game_state_t game_state;
    game_result_t game_result;
    robot_alive_t robot_alive;
    region_event_t region_event;
    supply_action_t supply_action;
    robot_state_t robot_state;
    power_heat_t power_heat;
    robot_position_t robot_position;
    robot_buff_t robot_buff;
    drone_energy_t drone_energy;
    damage_t damage;
    bullet_state_t bullet_state;
    custom_gimbal_current_t custom_gimbal_current;
} rm_state_t;

typedef struct {
    rm_frame_header_t frame_header;
    uint16_t cmd_id;

    union {
        game_state_t game_state;
        game_result_t game_result;
        robot_alive_t robot_alive;
        region_event_t region_event;
        supply_action_t supply_action;
        supply_request_t supply_request;
        robot_state_t robot_state;
        power_heat_t power_heat;
        robot_position_t robot_position;
        robot_buff_t robot_buff;
        drone_energy_t drone_energy;
        damage_t damage;
        bullet_state_t bullet_state;
        custom_gimbal_current_t custom_gimbal_current;
        custom_gimbal_target_t custom_gimbal_target;
    };
} rm_protocol_t;

#endif