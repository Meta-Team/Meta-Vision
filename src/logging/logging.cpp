#include <ctime>
#include <string>
#include "logging.hpp"
#include "colortext.hpp"

Logging normal(FG_DEFAULT);
Logging success(FG_GREEN);
Logging warn(FG_YELLOW);
Logging err(FG_RED);

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

/**
 * @brief Prints the current date time at the beginning of the line.
 * 
 * @param os Ostream object, passed from the previous source
 * @param obj A logging object, sets the color of the date time
 * @return std::ostream& Ostream object, passed to the next source
 */
std::ostream& operator<<(std::ostream& os, const Logging& obj) {
    os << "\r" << color(obj._fg_color, obj._bg_color) << "[" << obj._currentDateTime() << "] ";
    return os;
}