#include <string>
#include <cmath>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
// Note to myself: long int 32 bits, int 16 bits
string Format::ElapsedTime(long seconds)
{
    int hh, ss, mm;
    std::string str_ss, str_mm, str_hh, time;

    hh = seconds / 3600;
    mm = (seconds % 3600) / 60;
    ss = (seconds % 3600) % 60;

    // Convert to strings
    str_ss = std::to_string(ss);
    str_mm = std::to_string(mm);
    str_hh = std::to_string(hh);

    // to output as 2 digit ints
    str_ss.insert(0, 2 - str_ss.length(), '0');
    str_mm.insert(0, 2 - str_mm.length(), '0');
    str_hh.insert(0, 2 - str_hh.length(), '0');

    time = str_hh + ":" + str_mm + ":" + str_ss;

    return time;
}