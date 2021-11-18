#include <string>
#include <cmath>

#include "format.h"

using std::string;

// Note to myself: long int 32 bits, int 16 bits
string Format::ElapsedTime(long seconds)
{
    int hours, minutes, remainSec;
    std::string str_ss, str_mm, str_hh, time;

    hours = seconds / 3600;
    minutes = (seconds % 3600) / 60;
    remainSec = (seconds % 3600) % 60;

    // Convert to strings
    str_ss = std::to_string(remainSec);
    str_mm = std::to_string(minutes);
    str_hh = std::to_string(hours);

    // to output as 2 digit ints
    str_ss.insert(0, 2 - str_ss.length(), '0');
    str_mm.insert(0, 2 - str_mm.length(), '0');
    str_hh.insert(0, 2 - str_hh.length(), '0');

    time = str_hh + ":" + str_mm + ":" + str_ss;

    return time;
}