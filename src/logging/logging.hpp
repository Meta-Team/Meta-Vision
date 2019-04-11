#ifndef _LOGGING_HPP_
#define _LOGGING_HPP_

#include <iostream>
#include <string>
#include "colortext.hpp"

/**
 * @brief Logging object, to simplify logging code.
 */
class Logging {
public:
    /**
     * @brief Construct a new Logging object with default colors
     */
    Logging(): _fg_color(FG_DEFAULT), _bg_color(BG_DEFAULT) {}

    /**
     * @brief Construct a new Logging object with specified foreground color and default background color
     * 
     * @param fg_color Foreground color of the log message
     */
    Logging(int fg_color): _fg_color(fg_color), _bg_color(BG_DEFAULT) {}

    /**
     * @brief Construct a new Logging object with specified foreground and background colors
     * 
     * @param fg_color Foreground color of the log message
     * @param bg_color Background color of the log message
     */
    Logging(int fg_color, int bg_color): _fg_color(fg_color), _bg_color(bg_color) {}
    friend std::ostream& operator<<(std::ostream& os, const Logging& obj);

    /**
     * @brief Foreground color in shell.
     */
    int _fg_color;

    /**
     * @brief Background color in shell.
     */
    int _bg_color;

    std::string _currentDateTime() const;
};

extern Logging normal, success, warn, err;
#define cmessage cout << normal
#define csuccess cout << success
#define cwarning cout << warn
#define cerror cout << err
#define endlog color(FG_DEFAULT, BG_DEFAULT) << endl

#endif