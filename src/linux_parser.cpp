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

// reuse logic for running and total process count
int GetProcessData(std::string label);

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

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

float LinuxParser::MemoryUtilization() { 
  long int memTotal, memFree, buffers, cached, data;
  string line, meminfo;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  
  if (!stream.is_open()) {
    return 0.0;
  }
  
  while(std::getline(stream, line)) {
    std::istringstream linestream(line);
    linestream >> meminfo >> data;
    
    if (meminfo == "MemTotal:") memTotal = data;
    else if (meminfo == "MemFree:") memFree = data;
    else if(meminfo == "Buffers:") buffers = data;
    else if(meminfo == "Cached:") cached = data;
  }
  
  // calculation done from redhat explaination
  return (1-(float)(memTotal - (memFree + buffers + cached))/(float)memTotal);
}

long LinuxParser::UpTime() { 
  float uptime;
  string line;
  std::ifstream fstream(kProcDirectory+kUptimeFilename);
  
  if(fstream.is_open()) {
    std::getline(fstream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
  }
  
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies(std::vector<long> cpuJiffies) {
  return std::accumulate(cpuJiffies.begin(), cpuJiffies.end(), 0); 
}

long LinuxParser::ActiveJiffies(int pid) { 
  string curr, utime, stime, cutime, cstime;
  string line;
  std::ifstream file(kProcDirectory+to_string(pid)+kStatFilename);
  
  if(!file.is_open())
    return 0;
  
  std::getline(file, line);
  std::istringstream sstream(line);
  
  for(int i = 0; i < 17 && sstream >> curr; i++) {    
    if(i == 13) utime = curr;
    else if(i == 14) stime = curr;
    else if(i == 15) cutime = curr;
    else if(i == 16) cstime = curr;
  }
  
  // returns sum of all the active jiffies for the process
  return (long)(stoi(utime)+stoi(stime)+stoi(cutime)+stoi(cstime)); 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies(std::vector<long> cpuJiffies) { 
  return (std::accumulate(cpuJiffies.begin(), cpuJiffies.end(), 0) - cpuJiffies[3]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies(std::vector<long> cpuJiffies) { 
  return cpuJiffies[3];
}

// Read and return all the jiffies for the cpu
vector<long> LinuxParser::CpuUtilization() {
  std::string line;
  vector<long> cpuInfo = {};
  std::ifstream cpuStream(kProcDirectory+kStatFilename);
  
  if(cpuStream.is_open()) {
    // ignore the first 5 characters (cpu label) and skip to next char set
    cpuStream.ignore(5, ' ');
    std::getline(cpuStream, line);    
    std::istringstream stream(line);
    
    // add all the cpu jiffies to the vector
    for(long cpu; stream >> cpu; cpuInfo.emplace_back(cpu));
  } 
  
  return cpuInfo;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  return GetProcessData("processes");
}

// Read and return running processes
int LinuxParser::RunningProcesses() {
  return GetProcessData("procs_running");
}

string LinuxParser::Command(int pid) { 
  string line;
  string data;
  string cmd;
  std::ifstream fstream(kProcDirectory+to_string(pid)+kCmdlineFilename);
  
  if(fstream.is_open()) {
    std::getline(fstream, line);
    return line;
  }  
     
  return string();
}

string LinuxParser::Ram(int pid) { 
  string line;
  string data;
  size_t ram;
  std::ifstream fstream(kProcDirectory+to_string(pid)+kStatusFilename);
  
  if(!fstream.is_open())
  	return string();
  
  while(std::getline(fstream, line)) {
    std::istringstream sstream(line);
    if(sstream >> data && data == "VmSize:") {
      sstream >> ram;
      break;
    }    
  }
  
  return std::to_string(ram/1000);
}

string LinuxParser::Uid(int pid) { 
  string line;
  string data;
  string uid;
  std::ifstream fstream(kProcDirectory+to_string(pid)+kStatusFilename);
  
  if(!fstream.is_open())
  	return string(); 
  
  while(std::getline(fstream, line)) {
    std::istringstream sstream(line);
    sstream >> data;
    if(data != "Uid:") continue;
    sstream >> uid;
    break;
  }
  
  return uid;
}

string LinuxParser::User(int pid) { 
  string line;
  string data;
  string sub_token;
  string user_name;
  bool found_username = false;
  size_t next_pos = 0;
  size_t prev_pos = 0;
  
  std::ifstream fstream(kPasswordPath);
  string uid = LinuxParser::Uid(pid);
  
  if (!fstream.is_open() || uid.length() == 0)
  	return string(); 
    
  while (!found_username && std::getline(fstream, line)) {
    std::istringstream sstream(line);
    sstream >> data;
	next_pos = 0;
    prev_pos = 0;

    // loop through the line splitting on the colon character
    // until the correct uid is found
    for (int i = 0; (next_pos = data.find(":", prev_pos)) != string::npos; i++) {
      sub_token = data.substr(prev_pos, next_pos-prev_pos);
      
      if (i == 0) {
        user_name = sub_token;
      } else if (i == 2 && sub_token.compare(uid) == 0) {
        found_username = true;
        break;
      }
	  
      // reset the previous start position to the current next position
      prev_pos = next_pos+1;
    }
  }
  
  return user_name;
}

long LinuxParser::UpTime(int pid) { 
  string data;
  string line;
  std::ifstream fstream(kProcDirectory+to_string(pid)+kStatFilename);
  
  if(!fstream.is_open()) {
    return 0;
  }
  
  std::getline(fstream, line);
  std::istringstream sstream(line);
  // get the 21 index value of the line of data
  for(int i = 0; i < 22 && sstream >> data; i++); 

  return (long)(stoi(data)/sysconf(_SC_CLK_TCK));
}

int GetProcessData(std::string label) {  
  int value;
  string data;
  string line;
  std::ifstream fstream(LinuxParser::kProcDirectory+LinuxParser::kStatFilename);
  
  if(!fstream.is_open())
    return 0;
  
  while(std::getline(fstream, line)) {
    std::istringstream linestream(line);
    linestream >> data;
    
    if(data != label) continue;
    // we got the value we need so just break the loop
    linestream >> value;
    break;
  }
  
  return value;
}

