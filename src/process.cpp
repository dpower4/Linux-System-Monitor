#include <unistd.h>
#include <string>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  userName_ = LinuxParser::User(pid_);
  command_ = LinuxParser::Command(pid_);
  ram_ = LinuxParser::Ram(pid_);
  upTime_ =
      LinuxParser::UpTime() - LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK);
  cpuUtilization_ = CpuUtilizationPid(pid_);
}
int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const { return cpuUtilization_; }

// TODO: compute and return active usage
float Process::CpuUtilizationPid(int pid_) {
  long totalTimePid = LinuxParser::ActiveJiffiesProc(pid_);
  long startTimePid = LinuxParser::UpTime(pid_);
  long sysUpTime = LinuxParser::UpTime();  // secs

  const float seconds = sysUpTime - startTimePid / sysconf(_SC_CLK_TCK);
  if (seconds == 0) {
    return 0;
  }
  return static_cast<float>(totalTimePid / sysconf(_SC_CLK_TCK)) / seconds;
}

string Process::Command() const { return command_; }

string Process::Ram() const { return ram_; }

string Process::User() const { return userName_; }

long int Process::UpTime() const { return upTime_; }

bool Process::operator<(Process const& a) const {
  return this->cpuUtilization_ > a.cpuUtilization_;
}
