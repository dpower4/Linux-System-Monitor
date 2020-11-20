#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  Processor();
  [[nodiscard]] int getProcCount() const;
  float Utilization(int index);
  std::vector<float> Utilizations();
 private:

  int proc_count_ = 0;

 private:
  std::vector<long> prevTotals_;
  std::vector<long> prevActives_;
};

#endif