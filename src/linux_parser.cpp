#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}


// read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// get the processes ids folders
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string label;
  float mem_total{0};
  float  mem_free{0};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::istringstream linestream;
    std::getline(stream, line);
    linestream.str(line);
    //reads MemTotal
    linestream >> label >> mem_total;
    if(mem_total == 0){ return 0;}    
    std::getline(stream, line);
    linestream.str(line);
    //reads MemFree
    linestream >> label >> mem_free;
  }  
  return (mem_total - mem_free)/mem_total;
}

// Read and return the system uptime in seconds
long LinuxParser::UpTime() { 
  long up_time{0};
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    //reads first double
    linestream >> up_time;
  }
return up_time;
}


// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
   long idle_jif{0}; 
   long non_idle_jif{0}; 
   idle_jif = IdleJiffies();
   non_idle_jif = ActiveJiffies();
   return idle_jif + non_idle_jif;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long total_time{0}, utime{0}, stime{0}, cutime{0}, cstime{0};
  string value, line;
  int count = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    //reads first double
    while(linestream >> value){
      ++count;
      switch(count){
        case 14: utime = stol(value,nullptr,10); break;
        case 15: stime = stol(value,nullptr,10); break;
        case 16: cutime = stol(value,nullptr,10); break;
        case 17: cstime = stol(value,nullptr,10); break;
      }
    }
  }
  total_time = utime + stime;
  return total_time + cutime + cstime;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_values = CpuUtilization();
  long user_jif{0}; 
  long nice_jif{0};
  long system_jif{0};
  long irq_jif{0}; 
  long soft_jif{0}; 
  long steal_jif{0}; 
  user_jif = stol(cpu_values[0],nullptr,10);
  nice_jif = stol(cpu_values[1],nullptr,10);
  system_jif = stol(cpu_values[2],nullptr,10);
  irq_jif = stol(cpu_values[5],nullptr,10);
  soft_jif = stol(cpu_values[6],nullptr,10);
  steal_jif = stol(cpu_values[7],nullptr,10); 
  return user_jif + nice_jif + system_jif + irq_jif + soft_jif + steal_jif; 
  }


// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> cpu_values = CpuUtilization(); 
  long  idle{0};
  long  iowait{0};
  idle = stol(cpu_values[3],nullptr,10);
  iowait = stol(cpu_values[4],nullptr,10);
  return idle + iowait; 
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_fields{"0", "0", "0", "0", "0", "0", "0", "0"};
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line);
    if(linestream >> value){
      int i = 0;
      while(linestream >> value){
        if(i > 7){
          break;
        } 
        cpu_fields[i] = value;
        ++i;
      }
    }
  }
  return cpu_fields;
}
  



// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }else{
          continue;
        }
      }
    }
  }
return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses(){
  string line;
  string key;
  int value{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }else{
          continue;
        }
      }
    }
  }
return value;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) +  kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line); 
  }
  return line;
}
// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value{"0"};
  std::ifstream filestream(kProcDirectory + to_string(pid) +  kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmPeak") {
          return to_string(stol(value,nullptr,10)/1000);
        }else{
          continue;
        }
      }
    }
  }
return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) +  kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }else{
          continue;
        }
      }
    }
  }
return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> value) {
        if (value == LinuxParser::Uid(pid)) {
          if(key.size()<7){
             for(int c = key.size(); c <= 7; ++c )
             { key+=" "; }
          
          }
          return key.substr(0,7);
        }
      }
    }
  }
return key;
}

// Read and return the uptime of a process in seconds
long LinuxParser::UpTime(int pid) {
  long  starttime{0};
  long hertz = sysconf(_SC_CLK_TCK);
  string value, line;
  int count = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      ++count;
      switch(count){
        case 22: starttime = stol(value,nullptr,10); break;
      }
    }
  }
  return (UpTime() - (starttime / hertz)); 
  }