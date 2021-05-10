#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process
{
public:
    Process(int pID);
    int Pid();                              // TODO: See src/process.cpp
    std::string User();                     // TODO: See src/process.cpp
    std::string Command();                  // TODO: See src/process.cpp
    float CpuUtilization();                 // TODO: See src/process.cpp
    std::string Ram();                      // TODO: See src/process.cpp
    long int UpTime();                      // TODO: See src/process.cpp
    bool operator<(Process const &a) const; // TODO: See src/process.cpp

    // TODO: Declare any necessary private members
private:
    int pid_;
    float cpuutilization_;
    // std::string user_;
    // std::string cmd_;
    // float CpuUsage_;
    // std::string ram_;
    // long int uptime_;

};

#endif