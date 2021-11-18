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
  string line;
  string key;
  string value;
  string mtotal;
  string mfree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "MemTotal:")
        {
          mtotal = value;
        }
        if (key == "MemFree:")
        {
          mfree = value;
        }
      }
    }
  }
  return (stof(mtotal) - stof(mfree)) / stof(mtotal);
}

// Read and return the system uptime
long LinuxParser::UpTime()
{
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  // Example format:
  //     user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
  //cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0

  vector<string> values = LinuxParser::CpuUtilization();
  vector<long> valueslong(10, 0);
  long total = 0;
  vector<CPUStates> all = {kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_};
  for (int i : all)
  { // All non-guest values
    valueslong[i] = stol(values[i]);
    total += valueslong[i];
  };
  return total;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid)
{
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value)
    {
      values.push_back(value);
    }
  }
  return stol(values[13] + values[14]);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  vector<string> jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  vector<string> jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
  string line, value;
  string key;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
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
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open())
  {
    std::getline(stream, command);
  }
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
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
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
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
  return "0";
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
  return "unknown";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value)
    {
      values.push_back(value);
    };
  }
  return LinuxParser::UpTime() - (stol(values[21]) / 100);
}