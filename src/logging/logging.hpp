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
    Logging() {
        _newline(FG_DEFAULT, BG_DEFAULT);
    }

    /**
     * @brief Construct a new Logging object with specified foreground color and default background color
     * 
     * @param fg_color Foreground color of the log message
     */
    Logging(int fg_color) {
        _newline(fg_color, BG_DEFAULT);
    }

    /**
     * @brief Construct a new Logging object with specified foreground and background colors
     * 
     * @param fg_color Foreground color of the log message
     * @param bg_color Background color of the log message
     */
    Logging(int fg_color, int bg_color) {
        _newline(fg_color, bg_color);
    }

    template <typename T>
    Logging& operator<< (const T& v) {
        std::cout << v;
        return *this;
    }

    ~Logging();

private:
    std::string _currentDateTime() const;
    void _newline(int fg_color, int bg_color);
};

#define cmessage Logging()
#define csuccess Logging(FG_GREEN)
#define cwarning Logging(FG_YELLOW)
#define cerror Logging(FG_RED)

#endif