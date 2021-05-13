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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
  string line, key, memVal;
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

// TODO: Read and return the system uptime
long LinuxParser::UpTime()
{
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  long upTime = 0;
  long idleTime;
  if (filestream.is_open())
  {
    std::string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idleTime;
  }
  return upTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    // Example format:
    //     user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
    //cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
    std::string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    string cpu;

    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    long totalUserTime = user - guest;
    long totalNiceTime = nice - guest_nice;
    long totalIdleTime = idle + iowait;
    long totalSystem = system + irq + softirq;
    long totalVirtualTime = guest + guest_nice;
    return totalUserTime + totalNiceTime + totalIdleTime + totalSystem + totalVirtualTime;
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid)
{
  std::stringstream filename;
  filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
  std::ifstream filestream(filename.str());
  if (filestream.is_open())
  {
    string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    string ignore;
    long utime;
    long stime;
    long cutime;
    long cstime;
    long starttime;
    for (int i = 0; i < 13; i++)
      linestream >> ignore;
    linestream >> utime >> stime >> cutime >> cstime;
    for (int i = 0; i < 4; i++)
      linestream >> ignore;
    linestream >> starttime;
    return utime + stime + cutime + cstime + starttime;
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  long upTime = 0;
  long idleTime;
  if (filestream.is_open())
  {
    string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idleTime;
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  string os, kernel;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    string cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    return (idle + iowait); //idleTime
  }
  return 0;
}

// TODO: Read and return CPU utilization
vector<LinuxParser::CpuTimes> LinuxParser::CpuUtilPercentage()
{
  std::ifstream filestream(kProcDirectory + kStatFilename);
  vector<LinuxParser::CpuTimes> CpuTimesResult;
  if (filestream.is_open()) {
      std::string line;
      while (std::getline(filestream, line)) {
          std::istringstream linestream(line);
          std::string key;
          long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
          linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
          if (key.find("cpu") != string::npos)
              return CpuTimesResult;
          
          long totalIdleTime = idle + iowait;
          long totalNoIdleTime = user + nice + system + irq + softirq;
          
          LinuxParser::CpuTimes thisCpuTimes;
          thisCpuTimes.idleTime = totalIdleTime;
          thisCpuTimes.totalTime = totalIdleTime + totalNoIdleTime;

          CpuTimesResult.emplace_back(thisCpuTimes);
      }    
  } 
  return CpuTimesResult;
}
// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::vector<LinuxParser::CpuTimes> prevCPUTimes = LinuxParser::CpuUtilPercentage(); 
  sleep(1);
  std::vector<LinuxParser::CpuTimes> currentCPUTimes = LinuxParser::CpuUtilPercentage(); 
  vector<std::string> CpuUtilizationResult;
  for(int i = 0; i < (int)(currentCPUTimes.size()); i++) {
      long dTotal = currentCPUTimes[i].totalTime - prevCPUTimes[i].totalTime ;
      long dIdle = currentCPUTimes[i].idleTime - prevCPUTimes[i].idleTime ;
      CpuUtilizationResult.emplace_back(std::to_string((dTotal - dIdle)/dTotal));
  }
  return CpuUtilizationResult;
  }

// TODO: Read and return the total number of processes
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

// TODO: Read and return the number of running processes
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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid)
{
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  string processCMD;
  if (filestream.is_open())
  {
    std::getline(filestream, processCMD);
  }
  return processCMD;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key;
  string value;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key;
  string value;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  // read the user ID for this process
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
  return userInfo;
 }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid)  + kStatFilename);
  string line, value;
  long uptime = 0;
  const int StartTime = 22;
  if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i < StartTime; i++) {
        linestream >> value;
        if (i == StartTime) {
          try {
            uptime = std::stol(value) / sysconf(_SC_CLK_TCK);
            return uptime;
          } catch (const std::invalid_argument& arg) {
            return 0;
          }
        }
      }
    }
  }
  return uptime; 
}


// 
LinuxParser::CpuProcessInfo LinuxParser::GetProcessCpuInfo(int pid) {
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
    std::ifstream filestream(filename.str());
    LinuxParser::CpuProcessInfo resultInfo;
    if (filestream.is_open()) {
        std::string line;
        std::getline(filestream, line);
        std::istringstream linestream(line);
        std::string ignore;
        long utime;
        long stime;
        long cutime;
        long cstime;
        long starttime;
        for(int i = 0; i < 13; i++) linestream >> ignore;
        linestream >> utime >> stime >> cutime >> cstime ;
        for(int i = 0; i < 4; i++) linestream >> ignore;
        linestream >> starttime;
        resultInfo.seconds = LinuxParser::UpTime() - (starttime/sysconf(_SC_CLK_TCK));
        resultInfo.totalTime = (utime + stime + cutime + cstime)/sysconf(_SC_CLK_TCK);
    }   
    return resultInfo;
  }

  float LinuxParser::CpuUtilization(int pid)
  {
    //LinuxParser::CpuProcessInfo previous = LinuxParser::GetProcessCpuInfo(pid);
    //sleep(1);
    LinuxParser::CpuProcessInfo current = LinuxParser::GetProcessCpuInfo(pid);
    long secondsd = current.seconds; // - previous.seconds;
    long totald = current.totalTime;// - previous.totalTime;
    return totald*1.0/secondsd;//secondsd;
  }
