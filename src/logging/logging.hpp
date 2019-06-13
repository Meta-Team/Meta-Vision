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
     * 
     * @param function_prettyprint name of the function from which Logging is called.
     */
    Logging(const std::string& function_prettyprint) {
        _newline(FG_DEFAULT, BG_DEFAULT, function_prettyprint);
    }

    /**
     * @brief Construct a new Logging object with specified foreground color and default background color
     * 
     * @param fg_color Foreground color of the log message
     * @param function_prettyprint name of the function from which Logging is called.
     */
    Logging(int fg_color, const std::string& function_prettyprint) {
        _newline(fg_color, BG_DEFAULT, function_prettyprint);
    }

    /**
     * @brief Construct a new Logging object with specified foreground and background colors
     * 
     * @param fg_color Foreground color of the log message
     * @param bg_color Background color of the log message
     * @param function_prettyprint name of the function from which Logging is called.
     */
    Logging(int fg_color, int bg_color, const std::string& function_prettyprint) {
        _newline(fg_color, bg_color, function_prettyprint);
    }

    /**
     * @brief Implements a simple left arrow operator, to work similar to std::cout.
     * 
     * @param v object to be printed out
     */
    template <typename T>
    Logging& operator<< (const T& v) {
        std::cout << v;
        return *this;
    }

    ~Logging();

private:
    /**
     * @brief returns the current datetime as a string.
     * 
     * The datetime returned is in Y-m-d H:M:S format.
     */
    std::string _currentDateTime() const;

    /**
     * @brief Called by the constructor of Logging object, to print a line
     *        of datetime, function name before message.
     * 
     * @param fg_color Foreground color of the log message
     * @param bg_color Background color of the log message
     * @param function_prettyprint name of the function from which Logging is called.
     */
    void _newline(int fg_color, int bg_color, const std::string& function_prettyprint);
};

#define cmessage Logging(__PRETTY_FUNCTION__)
#define csuccess Logging(FG_GREEN, __PRETTY_FUNCTION__)
#define cwarning Logging(FG_YELLOW, __PRETTY_FUNCTION__)
#define cerror Logging(FG_RED, __PRETTY_FUNCTION__)

#endif