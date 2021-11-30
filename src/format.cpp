#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "format.h"

using std::string;

// Note to myself: long int 32 bits, int 16 bits
string Format::ElapsedTime(long seconds)
{
    int hours, minutes, remainSec;

    hours = seconds / 3600;
    minutes = (seconds % 3600) / 60;
    remainSec = (seconds % 3600) % 60;

    /* Referred to Stackoverflow for below 2 digits format, please see link
    https://stackoverflow.com/questions/225362/convert-a-number-to-a-string-with-specified-length-in-c */
    std::stringstream oS;
    oS << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << remainSec;
    
    return oS.str();

}

