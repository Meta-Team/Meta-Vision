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

void Logging::_newline(int fg_color, int bg_color) {
    std::cout << "\r" << color(fg_color, bg_color) << "[" << _currentDateTime() << "] ";
}

Logging::~Logging() {
    std::cout << color(FG_DEFAULT, BG_DEFAULT) << std::endl;
}