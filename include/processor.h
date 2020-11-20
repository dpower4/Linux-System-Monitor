#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor()= default;
  float Utilization();
 private:
  long prevTotal_ = 0;
  long prevActive_ = 0;
};

#endif