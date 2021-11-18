#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

Process::Process(int pid) {
  pid_ = pid;
}

// Returning this process's ID
int Process::Pid() const { return pid_; }

// Returning this process's CPU utilization
float Process::CpuUtilization(){
  cpuutilization_ = LinuxParser::CpuUtilization(pid_);
  return cpuutilization_;
}

// Returning the command that generated this process
std::string Process::Command() { return LinuxParser::Command(Pid()); }

// Returning this process's memory utilization
std::string Process::Ram() const{ return LinuxParser::Ram(Pid()); }

// Returning the user (name) that generated this process
std::string Process::User() { return LinuxParser::User(Pid()); }

// Returning the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// Overloading the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return (cpuutilization_ < a.cpuutilization_) ? true : false; 
}