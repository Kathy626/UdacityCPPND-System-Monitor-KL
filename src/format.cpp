#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
// Note to myself: long int 32 bits, int 16 bits
string Format::ElapsedTime(long seconds) { 
    int minutes =  seconds >= 60 ? (int)seconds/60 : 0;
    int hours = minutes >= 60 ? minutes/60 : 0;
    int remainSec = (int)seconds%60;
    string outputTime = std::to_string(hours) + ":" + std::to_string(minutes)
                         +  ":" + std::to_string(remainSec);
    return outputTime; 
}