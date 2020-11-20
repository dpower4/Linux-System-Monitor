#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
  // CPU_Percentage = (totald - idled)/totald
  long currTotal = LinuxParser::Jiffies();
  long currActive = LinuxParser::ActiveJiffies();

  float util_percent = static_cast<float>(currActive  - prevActive_) /
                       static_cast<float>(currTotal - prevTotal_);

  prevTotal_ = currTotal;
  prevActive_ = currActive;

  return util_percent;
}