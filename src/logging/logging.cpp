#include <ctime>
#include <string>
#include "logging.hpp"
#include "colortext.hpp"

/**
 * @brief Returns the current date time, formatted as Y-m-d H:M:S
 * 
 * @return std::string Formatted current date time.
 */
std::string Logging::_currentDateTime() const {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
    std::string str(buffer);
    return str;
}

void Logging::_newline(int fg_color, int bg_color, const std::string& function_prettyprint) {
    std::size_t bracket_pos = function_prettyprint.find('(');
    if(bracket_pos == std::string::npos) {
        bracket_pos = function_prettyprint.length();
    }
    std::string function_nameonly = function_prettyprint.substr(0, bracket_pos);

    std::cout << "\r" << color(fg_color, bg_color) << "[" << _currentDateTime() << "] " << color(FG_BLUE, bg_color) << function_nameonly << ": " << color(fg_color, bg_color);
}

Logging::~Logging() {
    std::cout << color(FG_DEFAULT, BG_DEFAULT) << std::endl;
}