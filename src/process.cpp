#include <unistd.h>
#include <string>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid):pid_(pid) {
  userName_ = LinuxParser::User(pid_);
  command_ = LinuxParser::Command(pid_);
  ram_ = LinuxParser::Ram(pid_);
  upTime_ =  LinuxParser::UpTime() -
            LinuxParser::UpTime(pid_)/sysconf(_SC_CLK_TCK);
  cpuUtilization_ = CpuUtilizationPid(pid_);
}
int Process::Pid() const {
  return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
  return cpuUtilization_;
}

float Process::CpuUtilizationPid(int pid_) {
  long totalTimePid = LinuxParser::ActiveJiffiesProc(pid_);
  long startTimePid = LinuxParser::UpTime(pid_);
  long sysUpTime = LinuxParser::UpTime(); // secs

  float seconds = sysUpTime - startTimePid/sysconf(_SC_CLK_TCK);
  if (seconds  == 0){
    return 0;
  }
  return (float)(totalTimePid / sysconf(_SC_CLK_TCK)) / seconds;
}

string Process::Command() const {
  return command_;
}

string Process::Ram() const {
  return ram_;
}

string Process::User() const {
  return userName_;
}

long int Process::UpTime() const {
  return  upTime_;
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {

  // cpu utilization, if equal then use ram as secondary
  return this->cpuUtilization_ > a.cpuUtilization_;
}
