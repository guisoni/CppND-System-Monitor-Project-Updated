#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization()
 {    

      long prev_cpu = LinuxParser::Jiffies();
      long prev_iddle = LinuxParser::IdleJiffies();

      long cpu = LinuxParser::Jiffies();
      long iddle = LinuxParser::IdleJiffies();
      long total_diff =cpu-prev_cpu;;
      long iddle_diff = iddle - prev_iddle; 
      while(total_diff < 10){
          cpu = LinuxParser::Jiffies();
          iddle = LinuxParser::IdleJiffies();
          total_diff = cpu-prev_cpu;
          iddle_diff = iddle - prev_iddle;

      }
     return float(total_diff-iddle_diff)/(total_diff);
    }