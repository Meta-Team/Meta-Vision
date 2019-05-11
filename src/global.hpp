#ifndef _GLOBAL_HPP_
#define _GLOBAL_HPP_

#define SUCCESS 0
#define FAIL -1

// Path to configuration file if no parameter is passed on startup.
#define DEFAULT_CONFIG_NAME "../meta.yml"

// Queue size of some video targets
#define VIDEO_TARGET_QUEUE_SIZE 150

// Definition of enemy team's color
enum EnemyColor {RED = 0, BLUE = 1};

#endif