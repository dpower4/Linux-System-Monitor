#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  Processor();
  [[nodiscard]] int getProcCount() const;
  std::vector<float> Utilizations();

 private:
  float Utilization(int index);
  int proc_count_ = 0;

  std::vector<long> prevTotals_;
  std::vector<long> prevActives_;
};

#endif