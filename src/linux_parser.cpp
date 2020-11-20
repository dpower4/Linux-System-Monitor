#include <unistd.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <unordered_map>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
namespace fs = std::filesystem;

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

string LinuxParser::Kernel() {
  string os, versionTag, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> versionTag >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;

  for (const auto& dir : fs::directory_iterator(kProcDirectory)){
    if (dir.is_directory()){
      auto filename = dir.path().stem().generic_string();
      if(std::all_of(filename.begin(), filename.end(), isdigit)) {
        pids.emplace_back(stoi(filename));
      }
    }
  }
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value, mem_total{1}, mem_free{0};
  bool total_captured{false}, free_captured{false};

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if(key == "MemTotal"){
          mem_total = value;
          total_captured = true;
        }
        else if (key == "MemFree"){
          mem_free = value;
          free_captured = true;
        }
        if(total_captured && free_captured ){
          return (mem_total - mem_free)/mem_total;
        }
      }
    }
  }
  return 0;
}

long LinuxParser::UpTime() {
  string line;
  long upTime{0};

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    if (linestream >> upTime) {
      return upTime;
    }
  }
  return upTime;
}

long LinuxParser::Jiffies(int index) {
  long totalJiffies = 0;
  auto cpuUtilData = LinuxParser::CpuUtilization();
  // guest and guest_nice are already accounted for in the user and nice time
  for(int i = kUser_; i <= kSteal_; i++) {
    totalJiffies += stol(cpuUtilData[index][i]);
  }
  return totalJiffies;
}

long LinuxParser::ActiveJiffiesProc(int pid) {
  string line, val;
  long totalJiffies = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    auto counter = 0;
    while (linestream >> val) {
      if(counter > 12 && counter <= 16){
        totalJiffies+=stol(val);
      }
      counter++;
    }
  }
  return totalJiffies;
}

long LinuxParser::ActiveJiffies(int index) {
  // NonIdle = user + nice + system + irq + softirq + steal
  return LinuxParser::Jiffies(index) - LinuxParser::IdleJiffies(index);
}

long LinuxParser::IdleJiffies(int index) {
  long idleJiffies = 0;
  // Idle = idle + iowait
  auto cpuUtilData = LinuxParser::CpuUtilization();
  for(int i = kIdle_; i <= kIOwait_; i++) {
    idleJiffies += stol(cpuUtilData[index][i]);
  }
  return idleJiffies;
}

int LinuxParser::getCpuCount() {
  string line;
  string key, value;
  int count = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      // if key starts with cpu
      if(key.length() >= 3 && key.substr(0, 3)  == "cpu"){
        count++;
      }
    }
  }
  return count;
}\
vector<vector<string>> LinuxParser::CpuUtilization() {
  string line;
  string key, value;
  vector<vector<string>> cpusData;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      // if key starts with cpu
      if(key.length() >= 3 && key.substr(0, 3)  == "cpu"){
        vector<string> cpuValues;
        while(linestream >> value){
          cpuValues.emplace_back(value);
        }
        cpusData.emplace_back(cpuValues);
      }
    }
  }
  return cpusData;
}

string LinuxParser::GetValueForKey(const std::string& filename,
                                   const string& key){
  string line;
  string value;
  string tempKey;
  std::ifstream filestream(filename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> tempKey >> value) {
        if (tempKey == key) {
          return value;
        }
      }
    }
  }
  return string();
}

int LinuxParser::TotalProcesses() {
  std::string val = GetValueForKey(kProcDirectory + kStatFilename,
                                   "processes");
  return val.empty() ? 0 : std::stoi(val);
}

int LinuxParser::RunningProcesses() {
  std::string val =
      GetValueForKey(kProcDirectory + kStatFilename, "procs_running");
  return val.empty() ? 0 : std::stoi(val);
}

string LinuxParser::Command(int pid) {
  string line;
  string upTime{};

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

string LinuxParser::Ram(int pid) {
  auto mem = GetValueForKey(
      kProcDirectory + std::to_string(pid) + kStatusFilename, "VmSize:");
  // return in MB
  return mem.empty() ? "0" : std::to_string(std::stol(mem)/1000);
}

string LinuxParser::Uid(int pid) {
  return GetValueForKey(kProcDirectory + std::to_string(pid) + kStatusFilename,
                        "Uid:");
}

string LinuxParser::User(int pid) {
  string line;
  string user, pwd, uid;
  string uid_ =  LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', ':');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> pwd >> uid) {
        if (uid == uid_) {
          return user;
        }
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) {
  string line;
  string upTime{};

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
      std::istringstream linestream(line);
      auto counter = 0;
      while (linestream >> upTime) {
        if(counter == 21){
          return stol(upTime);
        }
        counter++;
      }
    }
  return 0;
}