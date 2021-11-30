#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem()
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "PRETTY_NAME")
        {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel()
{
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids()
{
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr)
  {
    // Is this a directory?
    if (file->d_type == DT_DIR)
    {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit))
      {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
  // this is from my old submission :) 
  // see https://github.com/Kathy626/UdacityCPPND-System-Monitor-KL/blob/ee7f2374f16ee98a5c01eaa256f4a9cb61516a78/src/linux_parser.cpp
  std::string line, key, memVal;
  float memTotal = 0;
  float memFree = 0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      //std::remove(line.begin(), line.end(), ' ');
      //std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);
      while (linestream >> key >> memVal)
      {
        // search for key memTotal
        if (key == "MemTotal:")
        {
          memTotal = std::stof(memVal);
        }
        // search for key memFree
        else if (key == "MemFree:")
        {
          memFree = std::stof(memVal);
          break;
        }
      }
    }
  }
  return ((memTotal - memFree) / memTotal);
}

// Read and return the system uptime
long LinuxParser::UpTime()
{
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  long upTime, idleTime;
  if (filestream.is_open())
  {
    std::string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idleTime;
  }
  return upTime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  // Example format:
  //     user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
  //cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0

  vector<string> values = LinuxParser::CpuUtilization();
  vector<long> valueslong(10, 0);
  long totalTime = 0;
  vector<CPUStates> all = {kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_};
  for (auto a : all)
  { // All non-guest values
    valueslong[a] = stol(values[a]);
    totalTime += valueslong[a];
  };
  return totalTime;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid)
{
  string line, value;
  vector<string> values;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value)
    {
      values.emplace_back(value);
    }
  }
  return stol(values[13] + values[14]);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  // use enum index to get needed contents from vector
  vector<string> jiffies = CpuUtilization();
  // user + nice + system + irq + softirq + steal
  long myActiveJiffies = stol(jiffies[CPUStates::kUser_]) + + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]);
  return myActiveJiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  vector<string> jiffies = CpuUtilization();
  long myIdleJiffies = stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
  return myIdleJiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, value, key;
  vector<string> values;
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    // get key out as it is unique
    linestream >> key;
    while (linestream >> value)
    {
      values.emplace_back(value);
    };
  }
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  std::ifstream filestream(kProcDirectory + kStatFilename);
  long totalProcesses = 0;
  if (filestream.is_open())
  {
    string line;
    bool processNumberFound = false;
    while (std::getline(filestream, line) && !processNumberFound)
    {
      std::istringstream linestream(line);
      string key;
      linestream >> key;
      if (key == "processes")
      {
        linestream >> totalProcesses;
        processNumberFound = true;
      }
    }
  }
  return totalProcesses;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  std::ifstream filestream(kProcDirectory + kStatFilename);
  long runningProcesses = 0;
  if (filestream.is_open())
  {
    string line;
    bool processNumberFound = false;
    while (std::getline(filestream, line) && !processNumberFound)
    {
      std::istringstream linestream(line);
      string key;
      linestream >> key;
      if (key == "procs_running")
      {
        linestream >> runningProcesses;
        processNumberFound = true;
      }
    }
  }
  return runningProcesses;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
  string command;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, command);
  }
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
  string line,key,value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key)
      {
        if (key == "VmSize:")
        {
          linestream >> value;
          return to_string(stol(value) / 1024);
        }
      }
    }
  }
  return "0";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{
  string line, key;
  string value = "0";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "Uid:")
        {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid)
{
  string uid = Uid(pid);
  string line,key;
  string userInfo;
  string other;
  // find user name for this user ID in /etc/passwd
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> userInfo >> other >> key) {
        if (key == uid) {
          return userInfo;
        }
      }
    }
  }
  return "unknown";   // userinfo = "unknown"
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{
  string line, value;
  vector<string> values;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  const int startTime = 22;
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value)
    {
      values.push_back(value);
    };
  }
  return LinuxParser::UpTime() - (stol(values[startTime-1]) / 100);
}


// Refered to Stackoverflow
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
// we need #14 utime, #15 stime, #16 cutime, #17 cstime, #22 starttime
float LinuxParser::CpuUtilization(int pid) {  
  string line;
 
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  
  std::getline(filestream, line); // file contains only one line    
  std::istringstream buffer(line);
  std::istream_iterator<string> beginning(buffer), end;
  std::vector<string> line_content(beginning, end);

  const int utimeID = 14 - 1;  // -1 due to starts at 0
  const int stimeID = 15 - 1;
  const int cutimeID = 16 - 1;
  const int cstimeID = 17 - 1;
  const int starttimeID = 22 - 1;

  float uptime = LinuxParser::UpTime();
  float utime = stof(line_content[13]);
  float stime = stof(line_content[14]);
  float cutime = stof(line_content[15]);
  float cstime = stof(line_content[16]);
  float starttime = stof(line_content[21]); 
  float hertz = sysconf(_SC_CLK_TCK);

  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / hertz);
  float cpu_load = 100.0f * (total_time / hertz ) / seconds;
  
  return cpu_load;
}