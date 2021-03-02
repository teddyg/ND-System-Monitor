#include "processor.h"

// return percentage between 0 and 1
float Processor::Utilization() {
  const float totalJiffyDelta = currTotalJiffies-prevTotalJiffies;
  const float activeJiffyDelta = currActiveJiffies-prevActiveJiffies;
  
  // reset the prev jiffies to the current set for the next calculation
  prevTotalJiffies = currTotalJiffies;
  prevIdleJiffies = currIdleJiffies;
  prevActiveJiffies = currActiveJiffies;
  
  return (activeJiffyDelta/totalJiffyDelta);
}

void Processor::setCurrTotalJiffies(long jiffies) {
  currTotalJiffies = jiffies;
}

void Processor::setCurrActiveJiffies(long jiffies) {
  currActiveJiffies = jiffies;
}

void Processor::setCurrIdleJiffies(long jiffies) {
  currIdleJiffies = jiffies;
}