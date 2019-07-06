#ifndef _RM_PROTOCOL_HPP_
#define _RM_PROTOCOL_HPP_

/** @file rm_protocol.hpp
 *  RoboMaster referee system serial protocol definition
 */

#include <stdint.h>
#include <map>

#define RM_CMDID_GAME_STATE     0x0001  /**< CMD_ID of Game State */
#define RM_CMDID_GAME_RESULT    0x0002  /**< CMD_ID of Game Result */
#define RM_CMDID_ROBOT_ALIVE    0x0003  /**< CMD_ID of Robot Alive */
#define RM_CMDID_REGION_EVENT   0x0101  /**< CMD_ID of Region Event */
#define RM_CMDID_SUPPLY_ACTION  0x0102  /**< CMD_ID of Supply Action */
#define RM_CMDID_SUPPLY_REQUEST 0x0103  /**< CMD_ID of Supply Request */
#define RM_CMDID_ROBOT_STATE    0x0201  /**< CMD_ID of Robot State */
#define RM_CMDID_POWER_HEAT     0x0202  /**< CMD_ID of Power & Heat */
#define RM_CMDID_ROBOT_POSITION 0x0203  /**< CMD_ID of Robot Position */
#define RM_CMDID_ROBOT_BUFF     0x0204  /**< CMD_ID of Robot Buff */
#define RM_CMDID_DRONE_ENERGY   0x0205  /**< CMD_ID of Drone Energy */
#define RM_CMDID_DAMAGE         0x0206  /**< CMD_ID of Damage */
#define RM_CMDID_BULLET_STATE   0x0207  /**< CMD_ID of Bullet State */

#define RM_CMDID_CUSTOM_GIMBAL_CURRENT  0xff00  /**< CMD_ID of Custom Data: Current gimbal state */
#define RM_CMDID_CUSTOM_GIMBAL_TARGET   0xff01  /**< CMD_ID of Custom Data: Target gimbal state */
#define RM_CMDID_CUSTOM_ENEMY_COLOR     0xff02  /**< CMD_ID of Custom Data: Enemy color setting */

namespace RM_Protocol {
    /** Packet size of each cmd_id */
    extern std::map<int, int> rm_map_cmdid_packetsize;

    /**
     * @brief 比赛状态数据：0x0001。发送频率：1Hz
     * 
     * See [RM2019裁判系统串口协议 V1.1](https://github.com/Meta-Team/Datasheets/blob/master/RM%20%E8%A3%81%E5%88%A4%E7%B3%BB%E7%BB%9F/RM2019%E8%A3%81%E5%88%A4%E7%B3%BB%E7%BB%9F%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE%20V1.1.pdf) for details.
     */
    typedef struct __attribute__((packed)) {
        /**
         * 0-3 bit：比赛类型
         * 
         * - 1：RoboMaster 机甲大师赛；
         * - 2：RoboMaster 机甲大师单项赛；
         * - 3：ICRA RoboMaster 人工智能挑战赛
         */
        uint8_t type : 4;
        /**
         * 4-7 bit：当前比赛阶段
         * 
         * - 0：未开始比赛；
         * - 1：准备阶段；
         * - 2：自检阶段；
         * - 3：5s 倒计时；
         * - 4：对战中；
         * - 5：比赛结算中
         */
        uint8_t stage: 4;
        /** 当前阶段剩余时间，单位 s */
        uint16_t remain_time;
    } game_state_t;

    /** 比赛结果数据：0x0002。发送频率：比赛结束后发送 */
    typedef struct __attribute__((packed)) {
        /** 0 平局 1 红方胜利 2 蓝方胜利 */
        uint8_t winner;
    } game_result_t;

    /** 机器人存活数据：0x0003。发送频率：1Hz */
    typedef struct __attribute__((packed)) {
        /**
         * - bit 0：红方英雄机器人；
         * - bit 1：红方工程机器人；
         * - bit 2：红方步兵机器人 1；
         * - bit 3：红方步兵机器人 2；
         * - bit 4：红方步兵机器人 3；
         * - bit 5：红方空中机器人；
         * - bit 6：红方哨兵机器人；
         * - bit 7：保留
         * - bit 8：蓝方英雄机器人；
         * - bit 9：蓝方工程机器人；
         * - bit 10：蓝方步兵机器人 1；
         * - bit 11：蓝方步兵机器人 2；
         * - bit 12：蓝方步兵机器人 3；
         * - bit 13：蓝方空中机器人；
         * - bit 14：蓝方哨兵机器人；
         * - bit 15：保留
         * - 对应的 bit 数值置 1 代表机器人存活，数值置 0 代表机器人死亡或者未上场。
         */
        uint16_t alive;
    } robot_alive_t;

    /** 场地事件数据：0x0101。发送频率：事件改变后发送 */
    typedef struct __attribute__((packed)) {
        /** bit 0-1：己方停机坪占领状态
         * - 0 为无机器人占领；
         * - 1 为空中机器人已占领但未停桨；
         * - 2 为空中机器人已占领并停桨
         */
        uint8_t own_drone_occupied : 2;
        /** bit 2：己方补给站 1 号补血点占领状态 1 为已占领； */
        uint8_t own_supply1_occupied : 1;
        /** bit 3：己方补给站 2 号补血点占领状态 1 为已占领； */
        uint8_t own_supply2_occupied : 1;
        /** bit 4：己方补给站 3 号补血点占领状态 1 为已占领； */
        uint8_t own_supply3_occupied : 1;
        /** bit 5-6：己方大能量机关状态：
         * - 0 为打击点未占领且大能量机关未激活；
         * - 1 为打击点占领且大能量机关未激活；
         * - 2 为大能量机关已激活；
         * - 3 为大能量机关已激活且打击点被占领；
         */
        uint8_t own_energy_state : 2;
        /** bit 7：己方关口占领状态 1 为已占领； */
        uint8_t own_gate_occupied : 1;
        /** bit 8：己方碉堡占领状态 1 为已占领； */
        uint8_t own_fort_occupied : 1;
        /** bit 9：己方资源岛占领状态 1 为已占领； */
        uint8_t own_resource_occupied : 1;
        /** bit 10-11：己方基地防御状态：
         * - 2 为基地 100%防御；
         * - 1 为基地有哨兵防御；
         * - 0 为基地无防御；
         */
        uint8_t own_base_defense : 2;
        /** bit 12-13：ICRA 红方防御加成
         * - 0：防御加成未激活；
         * - 1：防御加成 5s 触发激活中；
         * - 2：防御加成已激活
         */
        uint8_t icra_red_defense_bonus : 2;
        /** bit 14-15：ICRA 蓝方防御加成
         * - 0：防御加成未激活；
         * - 1：防御加成 5s 触发激活中；
         * - 2：防御加成已激活
         */
        uint8_t icra_blue_defense_bonus : 2;
    } region_event_t;

    /** 补给站动作标识：0x0102。发送频率：动作改变后发送 */
    typedef struct __attribute__((packed)) {
        /** 补给站口 ID：
         * - 1：1 号补给口；
         * - 2：2 号补给口
         */
        uint8_t supply_port_id;
        /** 补弹机器人 ID
         * - 0 为当前无机器人补弹，
         * - 1 为红方英雄机器人补弹，
         * - 2 为红方工程机器人补弹，
         * - 3/4/5 为红方步兵机器人补弹，
         * - 11 为蓝方英雄机器人补弹，
         * - 12 为蓝方工程机器人补弹，
         * - 13/14/15 为蓝方步兵机器人补弹
         */
        uint8_t supplied_robot_id;
        /** 出弹口开闭状态：0 为关闭，1 为子弹准备中，2 为子弹下落 */
        uint8_t supply_port_status;
        /** 补弹数量：
         * - 50：50 颗子弹；
         * - 100：100 颗子弹；
         * - 150：150 颗子弹；
         * - 200：200 颗子弹。
         */
        uint8_t supply_bullet_count;
    } supply_action_t;

    /** 请求补给站补弹子弹：cmd_id (0x0103)。发送频率：上限 10Hz。RM 对抗赛尚未开放 */
    typedef struct __attribute__((packed)) {
        /** 补给站补弹口 ID：
         * - 1：1 号补给口
         */
        uint8_t supply_port_id;
        /** 补弹机器人 ID：
         * - 1 为红方英雄机器人补弹，
         * - 2 为红方工程机器人补弹，
         * - 3/4/5 为红方步兵机器人补弹，
         * - 11 为蓝方英雄机器人补弹，
         * - 12 为蓝方工程机器人补弹，
         * - 13/14/15 为蓝方步兵机器人补弹
         */
        uint8_t supplied_robot_id;
        /** 补弹数目：
         * - 50 ：请求 50 颗子弹下落
         */
        uint8_t supply_bullet_count;
    } supply_request_t;

    /** 比赛机器人状态：0x0201。发送频率：10Hz */
    typedef struct __attribute__((packed)) {
        /** 机器人 ID：
         * - 1：红方英雄机器人；
         * - 2：红方工程机器人；
         * - 3/4/5：红方步兵机器人；
         * - 6：红方空中机器人；
         * - 7：红方哨兵机器人；
         * - 11：蓝方英雄机器人；
         * - 12：蓝方工程机器人；
         * - 13/14/15：蓝方步兵机器人；
         * - 16：蓝方空中机器人；
         * - 17：蓝方哨兵机器人。
         */
        uint8_t id;
        /** 机器人等级：1：一级；2：二级；3：三级。 */
        uint8_t level;
        /** 机器人剩余血量 */
        uint16_t hp;
        /** 机器人上限血量 */
        uint16_t hp_max;
        /** 机器人 17mm 枪口每秒冷却值 */
        uint16_t cooling_17mm;
        /** 机器人 17mm 枪口热量上限 */
        uint16_t heatmax_17mm;
        /** 机器人 42mm 枪口每秒冷却值 */
        uint16_t cooling_42mm;
        /** 机器人 42mm 枪口热量上限 */
        uint16_t heatmax_42mm;
        /** 主控电源输出情况：
         * - 0 bit：gimbal 口输出： 1 为有 24V 输出，0 为无 24v 输出；
         * - 1 bit：chassis 口输出：1 为有 24V 输出，0 为无 24v 输出；
         * - 2 bit：shooter 口输出：1 为有 24V 输出，0 为无 24v 输出；
         */
        uint8_t power_state;
    } robot_state_t;

    /** 实时功率热量数据：0x0202。发送频率：50Hz */
    typedef struct __attribute__((packed)) {
        /** 底盘输出电压 单位 毫伏 */
        uint16_t chassis_volt;
        /** 底盘输出电流 单位 毫安 */
        uint16_t chassis_current;
        /** 底盘输出功率 单位 W 瓦 */
        float chassis_power;
        /** 底盘功率缓冲 单位 J 焦耳 */
        uint16_t chassis_power_buffer;
        /** 17mm 枪口热量 */
        uint16_t heat_17mm;
        /** 42mm 枪口热量 */
        uint16_t heat_42mm;
    } power_heat_t;

    /** 机器人位置：0x0203。发送频率：10Hz */
    typedef struct __attribute__((packed)) {
        /** 位置 x 坐标，单位 m */
        float x;
        /** 位置 y 坐标，单位 m */
        float y;
        /** 位置 z 坐标，单位 m */
        float z;
        /** 位置枪口，单位度 */
        float yaw;
    } robot_position_t;

    /** 机器人增益：0x0204。发送频率：状态改变后发送 */
    typedef struct __attribute__((packed)) {
        /**
         * - bit 0：机器人血量补血状态
         * - bit 1：枪口热量冷却加速
         * - bit 2：机器人防御加成
         * - bit 3：机器人攻击加成
         * - 其他 bit 保留
         */
        uint8_t buff;
    } robot_buff_t;

    /** 空中机器人能量状态：0x0205。发送频率：10Hz */
    typedef struct __attribute__((packed)) {
        /** 积累的能量点 */
        uint8_t energy_point;
        /** 可攻击时间 单位 s。50s 递减至 0 */
        uint8_t attack_time;
    } drone_energy_t;

    /** 伤害状态：0x0206。发送频率：伤害发生后发送 */
    typedef struct __attribute__((packed)) {
        /** bit 0-3：当血量变化类型为装甲伤害，代表装甲 ID，
         * 其中数值为 0-4 号代表机器人的五个装甲片，
         * 其他血量变化类型，该变量数值为 0。 */
        uint8_t armor_id : 4;
        /** bit 4-7：血量变化类型
         * - 0x0 装甲伤害扣血；
         * - 0x1 模块掉线扣血；
         * - 0x2 超枪口热量扣血；
         * - 0x3 超底盘功率扣血。
         */
        uint8_t damage_type : 4;
    } damage_t;

    /** 实时射击信息：0x0207。发送频率：射击后发送 */
    typedef struct __attribute__((packed)) {
        /** 子弹类型: 1：17mm 弹丸 2：42mm 弹丸 */
        uint8_t type;
        /** 子弹射频 单位 Hz */
        uint8_t freq;
        /** 子弹射速 单位 m/s */
        float speed;
    } bullet_state_t;

    /**
     * Custom command: 0xff00
     * 
     * Gimbal yaw/pitch FROM RM board TO vision board.
     * 
     * Vision board needs this value to correctly calculate the target yaw/pitch.
     * 
     * We're not simply sending delta of yaw/pitch, because vision board has
     * delay in processing, which may lead to inaccuracy of angles
     */
    typedef struct __attribute__((packed)) {
        /** Current gimbal yaw of the robot, unit: angle */
        float yaw;
        /** Current gimbal pitch of the robot, unit: angle */
        float pitch;
    } custom_gimbal_current_t;

    /** 
     * Custom command: 0xff01
     *  
     * Gimbal yaw/pitch TARGET sent by vision board to RM board.
     */
    typedef struct __attribute__((packed)) {
        /** Target gimbal yaw, unit: angle */
        float yaw;
        /** Target gimbal pitch, unit: angle */
        float pitch;
        /** Distance to the object, unit: cm */
        float distance;
    } custom_gimbal_target_t;

    /**
     * Custom command: 0xff02
     *
     * Enemy color setting sent by vision board to RM board.
     */
    typedef struct __attribute__((packed)) {
        /** Enemy Color, RED = 0, BLUE = 1 */
        uint8_t enemy_color;
    } custom_enemy_color_t;

    /** frame_header */
    typedef struct __attribute__((packed)) {
        /** 数据帧起始字节，固定值为 0xA5 */
        uint8_t sof;
        /** 数据帧中 data 的长度 */
        uint16_t data_length;
        /** 包序号 */
        uint8_t seq;
        /** 帧头 CRC8 校验 */
        uint8_t crc8;
    } rm_frame_header_t;

    /**
     * @brief Stores the last received data of each parameter.
     * 
     * TODO: Support storing parameter of multiple robots.
     */
    typedef struct {
        game_state_t game_state;                /**< Game State */
        game_result_t game_result;              /**< Game Result */
        robot_alive_t robot_alive;              /**< Robot Alive */
        region_event_t region_event;            /**< Region Event */
        supply_action_t supply_action;          /**< Supply Action */
        robot_state_t robot_state;              /**< Robot State */
        power_heat_t power_heat;                /**< Power & Heat */
        robot_position_t robot_position;        /**< Robot Position */
        robot_buff_t robot_buff;                /**< Robot Buff */
        drone_energy_t drone_energy;            /**< Drone Energy */
        damage_t damage;                        /**< Damage */
        bullet_state_t bullet_state;            /**< Bullet State */
        custom_gimbal_current_t custom_gimbal_current;  /**< Custom Data: Current gimbal state */
        custom_enemy_color_t custom_enemy_color;        /**< Custom Data: Enemy color setting */
    } rm_state_t;

    /** Full frame excluding CRC16 from referee system */
    typedef struct {
        /** Frame header */
        rm_frame_header_t frame_header;
        /** Command ID */
        uint16_t cmd_id;

        union {
            game_state_t game_state;            /**< Game State */
            game_result_t game_result;          /**< Game Result */
            robot_alive_t robot_alive;          /**< Robot Alive */
            region_event_t region_event;        /**< Region Event */
            supply_action_t supply_action;      /**< Supply Action */
            supply_request_t supply_request;    /**< Supply Request */
            robot_state_t robot_state;          /**< Robot State */
            power_heat_t power_heat;            /**< Power & Heat */
            robot_position_t robot_position;    /**< Robot Position */
            robot_buff_t robot_buff;            /**< Robot Buff */
            drone_energy_t drone_energy;        /**< Drone Energy */
            damage_t damage;                    /**< Damage */
            bullet_state_t bullet_state;        /**< Bullet State */

            /* Custom Data: Current gimbal state */
            custom_gimbal_current_t custom_gimbal_current;
            /* Custom Data: Enemy color setting */
            custom_enemy_color_t custom_enemy_color;
            /* Custom Data: Target gimbal state */
            custom_gimbal_target_t custom_gimbal_target;
        };
    } rm_protocol_t;
};

#endif