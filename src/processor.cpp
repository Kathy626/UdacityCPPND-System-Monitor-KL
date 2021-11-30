#include "processor.h"
#include "linux_parser.h"

// constructor
Processor::Processor() {
  PrevTotal_ = LinuxParser::Jiffies();
  PrevIdle_ = LinuxParser::IdleJiffies();
}

// Return the aggregate CPU utilization
float Processor::Utilization()
{
    /* see https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    here is the CPU usage percentage calculation:

    PrevIdle = previdle + previowait
    Idle = idle + iowait

    PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
    NonIdle = user + nice + system + irq + softirq + steal

    PrevTotal = PrevIdle + PrevNonIdle
    Total = Idle + NonIdle

    # differentiate: actual value minus the previous one
    totald = Total - PrevTotal
    idled = Idle - PrevIdle 

    CPU_Percentage = (totald - idled)/totald
 
    */
    long Total = LinuxParser::Jiffies();
    long Idle = LinuxParser::IdleJiffies();
    float Cpu_Utilization = 0.0f;

    long Totalld = Total - PrevTotal_;
    long Idled = Idle - PrevIdle_; 

    if(Totalld > 0)
    {
        Cpu_Utilization = static_cast<float>((Totalld - Idled) / Totalld);
    }

    /* update previous total and idle value */
    PrevTotal_ = Total;
    PrevIdle_ = Idle;

    return Cpu_Utilization;
}