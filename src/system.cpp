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
Processor& System::Cpu() { 
  vector<long> cpuJiffies = LinuxParser::CpuUtilization();
  cpu_.setCurrTotalJiffies(LinuxParser::Jiffies(cpuJiffies));
  cpu_.setCurrIdleJiffies(LinuxParser::IdleJiffies(cpuJiffies));
  cpu_.setCurrActiveJiffies(LinuxParser::ActiveJiffies(cpuJiffies));
  return cpu_;
}

vector<Process>& System::Processes() { 
  vector<int> processIds = LinuxParser::Pids();
  // reset the vector of processes to display on each method call
  if(!processes_.empty()) processes_.clear();

  for(auto it = processIds.begin(); it != processIds.end(); it++) {
    int pid = *it;
    string user = LinuxParser::User(pid);
    long active_jiffies = LinuxParser::ActiveJiffies(pid);
    string ram = LinuxParser::Ram(pid);
    long uptime = LinuxParser::UpTime(pid);
    string cmd = LinuxParser::Command(pid);
    processes_.emplace_back(Process(pid, user, uptime, cpu_uptime, active_jiffies, ram, cmd));
  }
  
  // sort the processes by cpu usage with the overloaded less than operator
  std::sort(processes_.begin(),processes_.end());
  
  return processes_;
}

std::string System::Kernel() {
  // the kernel version shouldn't change while running our program
  // so only read the file system once
  if(kernel == "")
    kernel = LinuxParser::Kernel();
	
  return kernel;
}

float System::MemoryUtilization() { 
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() {
  // the os name shouldn't change while running our program
  // so only read the file system once
  if(os_name == "")
	os_name = LinuxParser::OperatingSystem();

  return os_name; 
}

int System::RunningProcesses() { 
  return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() {
  return LinuxParser::TotalProcesses();
}

long int System::UpTime() {
  // current uptime will be used to calculate cpu utilization for the individual processes
  cpu_uptime = LinuxParser::UpTime();
  return cpu_uptime;
}