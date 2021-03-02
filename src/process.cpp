#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// constructor
Process::Process(int pid, string user, long uptime, long cpu_uptime, long active_jiffies, string ram, string cmdline) : 
pid(pid), user(user), uptime(uptime), cpu_uptime(cpu_uptime), active_jiffies(active_jiffies), ram(ram), cmdline(cmdline) {}

// Return this process's Id
int Process::Pid() { return pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
  // seconds to calculate by is the difference between cpu and process uptime
  long seconds = cpu_uptime - uptime;
  return (float)(active_jiffies/(float)sysconf(_SC_CLK_TCK))/(float)seconds;
}

// Return the command tht generated this process
string Process::Command() { return cmdline; }

// Return this process's memory utilization
string Process::Ram() { return ram; }

// Return the user (name) that generated this process
string Process::User() { return user; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return uptime; }

// overload less than operator to sort in descending order based on cpu usage
bool Process::operator<(Process const& a) const {
  long b_seconds = this->cpu_uptime - this->uptime;
  float b_cpu = 100*((float)(this->active_jiffies/(float)sysconf(_SC_CLK_TCK))/(float)b_seconds);
  long a_seconds = a.cpu_uptime - a.uptime;
  float a_cpu = 100*((float)(a.active_jiffies/(float)sysconf(_SC_CLK_TCK))/(float)a_seconds);
  
  return b_cpu > a_cpu;
}