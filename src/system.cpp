#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor &System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
// this is from my 5 months ago submission :)
// please see this commit: 
// https://github.com/Kathy626/UdacityCPPND-System-Monitor-KL/blob/ee7f2374f16ee98a5c01eaa256f4a9cb61516a78/src/system.cpp
vector<Process> &System::Processes()
{
  processes_.clear();
  vector<int> pIDs = LinuxParser::Pids();
  for (auto& pID : pIDs)
  {
    Process thisPid(pID);
    processes_.emplace_back(thisPid);
  }
  std::sort(processes_.begin(), processes_.end());
  reverse(processes_.begin(), processes_.end());
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }