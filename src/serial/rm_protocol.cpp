#include "rm_protocol.hpp"

namespace RM_Protocol {
    std::map<int, int> rm_map_cmdid_packetsize = {
        {RM_CMDID_GAME_STATE, 12},
        {RM_CMDID_GAME_RESULT, 10},
        {RM_CMDID_ROBOT_ALIVE, 11},
        {RM_CMDID_REGION_EVENT, 13},
        {RM_CMDID_SUPPLY_ACTION, 12},
        {RM_CMDID_SUPPLY_REQUEST, 11},
        {RM_CMDID_ROBOT_STATE, 24},
        {RM_CMDID_POWER_HEAT, 23},
        {RM_CMDID_ROBOT_POSITION, 25},
        {RM_CMDID_ROBOT_BUFF, 10},
        {RM_CMDID_DRONE_ENERGY, 12},
        {RM_CMDID_DAMAGE, 10},
        {RM_CMDID_BULLET_STATE, 15},
        {RM_CMDID_CUSTOM_GIMBAL_CURRENT, 13},
        {RM_CMDID_CUSTOM_GIMBAL_TARGET, 13},
    };
};
