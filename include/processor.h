#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  void setCurrTotalJiffies(long jiffies);
  void setCurrActiveJiffies(long jiffies);
  void setCurrIdleJiffies(long jiffies);
 // TODO: Declare any necessary private members
 private:
  int numberOfCPUs{0};
  long currTotalJiffies{0};
  long currActiveJiffies{0};
  long currIdleJiffies{0};
  long prevTotalJiffies{0};
  long prevActiveJiffies{0};
  long prevIdleJiffies{0};
};

#endif