#include <string>

#include "format.h"
// macro used for floating point conversion
#define FLOATING_PRECISION 100

std::string formatTime(int time, bool addColon = false) {
  std::string timeFormat = addColon ? ":" : "";

  if(time >= 10) {
    timeFormat += std::to_string(time);
  } else {
    timeFormat += "0" + std::to_string(time);
  }

  return timeFormat;
}

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
std::string Format::ElapsedTime(long s) { 
  int hours = s/3600;
  float fMinutes = (float)s / 60.00f - (hours*60);
  int minutes = (int)fMinutes;
  // gets the fractional minutes for conversion into seconds
  auto fseconds = ((int)(fMinutes*FLOATING_PRECISION) % FLOATING_PRECISION) * .01;
  int seconds = (int)(fseconds*60);
  
  std::string time;
  
  time += formatTime(hours);
  time += formatTime(minutes, true);
  time += formatTime(seconds, true);
    
  return time;
}