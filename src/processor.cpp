#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization()
{
    long total = LinuxParser::Jiffies();
    long active = LinuxParser::ActiveJiffies();
    return active * (1.0 / total);
}