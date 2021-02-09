#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid() const;                               
  std::string User() const;                      
  std::string Command() const;                   
  float CpuUtilization() const;                  
  std::string Ram() const;                       
  long int UpTime() const;                       
  bool operator<( Process const& a) const;  
  
  // TODO: Declare any necessary private members
 private:
   friend class System;
   int pid_;

};

#endif