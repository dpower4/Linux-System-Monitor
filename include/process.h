#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  //  Process(int pid):pid_(pid){};
  [[nodiscard]] int Pid() const;
  [[nodiscard]] std::string User() const;                      // TODO: See src/process.cpp
  [[nodiscard]] std::string Command() const;                   // TODO: See src/process.cpp
  [[nodiscard]] float CpuUtilization() const;                  // TODO: See src/process.cpp
  [[nodiscard]] std::string Ram() const;                       // TODO: See src/process.cpp
  [[nodiscard]] long int UpTime() const;                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

 private:
  [[nodiscard]] static float CpuUtilizationPid(int pid) ;
  int pid_;
  long upTime_;
  float cpuUtilization_;
  std::string userName_, command_, ram_;
};

#endif