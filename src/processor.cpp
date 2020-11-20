#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization(int index) {
  // CPU_Percentage = (totald - idled)/totald
  long currTotal = LinuxParser::Jiffies(index);
  long currActive = LinuxParser::ActiveJiffies(index);

  float util_percent = static_cast<float>(currActive  - prevActives_[index]) /
                       static_cast<float>(currTotal - prevTotals_[index]);

  prevTotals_[index] = currTotal;
  prevActives_[index] = currActive;

  return util_percent;
}

Processor::Processor() : proc_count_(LinuxParser::getCpuCount()) {
  prevTotals_.resize(proc_count_,0);
  prevActives_.resize(proc_count_,0);
}

std::vector<float> Processor::Utilizations() {
  std::vector<float> utils(proc_count_);
  for(auto i =0; i < proc_count_; i++){
    utils[i] = Utilization(i);
  }
  return utils;
}
int Processor::getProcCount() const { return proc_count_; }
