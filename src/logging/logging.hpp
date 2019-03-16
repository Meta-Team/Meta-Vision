#ifndef _LOGGING_HPP_
#define _LOGGING_HPP_

#include <iostream>
#include <string>
#include "colortext.hpp"

class Logging {
public:
    Logging(): _fg_color(FG_DEFAULT), _bg_color(BG_DEFAULT) {}
    Logging(int fg_color): _fg_color(fg_color), _bg_color(BG_DEFAULT) {}
    Logging(int fg_color, int bg_color): _fg_color(fg_color), _bg_color(bg_color) {}
    friend std::ostream& operator<<(std::ostream& os, const Logging& obj);
    int _fg_color;  // Foreground color in shell
    int _bg_color;  // Background color in shell
    std::string _currentDateTime() const;
};

extern Logging normal, success, warn, err;
#define cmessage cout << normal
#define csuccess cout << success
#define cwarning cout << warn
#define cerror cout << err
#define endlog color(FG_DEFAULT, BG_DEFAULT) << endl

#endif