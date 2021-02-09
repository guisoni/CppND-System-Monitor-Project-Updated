#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>


#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const  { 
    long hertz = sysconf(_SC_CLK_TCK);
    long seconds = UpTime(); 
    long total_time = LinuxParser::ActiveJiffies(Pid());
    if(seconds <= 1){return 0.0;}
    return (float(total_time)/hertz)/float(seconds);

    }
// Return the command that generated this process
string Process::Command() const { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<( Process const& a) const { 
    return (this->CpuUtilization() < a.CpuUtilization());
     }