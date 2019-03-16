#ifndef _COLORTEXT_HPP_
#define _COLORTEXT_HPP_

#include <iostream>
#include <string>

#define FG_DEFAULT          39
#define FG_BLACK            30
#define FG_RED              31
#define FG_GREEN            32
#define FG_YELLOW           33
#define FG_BLUE             34
#define FG_MAGENTA          35
#define FG_CYAN             36
#define FG_LIGHT_GRAY       37
#define FG_DARK_GRAY        90
#define FG_LIGHT_RED        91
#define FG_LIGHT_GREEN      92
#define FG_LIGHT_YELLOW     93
#define FG_LIGHT_BLUE       94
#define FG_LIGHT_MAGENTA    95
#define FG_LIGHT_CYAN       96
#define FG_WHITE            97

#define BG_DEFAULT          49
#define BG_BLACK            40
#define BG_RED              41
#define BG_GREEN            42
#define BG_YELLOW           43
#define BG_BLUE             44
#define BG_MAGENTA          45
#define BG_CYAN             46
#define BG_LIGHT_GRAY       47
#define BG_DARK_GRAY        100
#define BG_LIGHT_RED        101
#define BG_LIGHT_GREEN      102
#define BG_LIGHT_YELLOW     103
#define BG_LIGHT_BLUE       104
#define BG_LIGHT_MAGENTA    105
#define BG_LIGHT_CYAN       106
#define BG_WHITE            107

inline std::string color(int fg, int bg) {
    // os << "\e[" << obj._bg_color << ";0;" << obj._fg_color << "m";
    return std::string("\e[") + std::to_string(bg) + std::string(";0;") + std::to_string(fg) + std::string("m");
}

inline std::string color(int fg) {
    return color(fg, BG_DEFAULT);
}

#endif