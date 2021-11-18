#include <string>
#include <cmath>

#include "format.h"

using std::string;

// Note to myself: long int 32 bits, int 16 bits
string Format::ElapsedTime(long seconds)
{
    int hours, minutes, remainSec;

    hours = seconds / 3600;
    minutes = (seconds % 3600) / 60;
    remainSec = (seconds % 3600) % 60;

    // Convert to strings
    string outputTime = std::to_string(hours) + ":" + std::to_string(minutes)
                         +  ":" + std::to_string(remainSec);
    return outputTime; 
}

