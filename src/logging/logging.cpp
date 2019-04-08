#include <ctime>
#include <string>
#include "logging.hpp"
#include "colortext.hpp"

Logging normal(FG_DEFAULT);
Logging success(FG_GREEN);
Logging warn(FG_YELLOW);
Logging err(FG_RED);

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

std::ostream& operator<<(std::ostream& os, const Logging& obj) {
    os << "\r" << color(obj._fg_color, obj._bg_color) << "[" << obj._currentDateTime() << "] ";
    return os;
}