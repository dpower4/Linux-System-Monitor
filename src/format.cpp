#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  std::div_t dv{};
  std::string time = "";
  dv.quot = seconds;
  dv = std::div(dv.quot, 3600);
  if (dv.quot < 10){
    time += "0";
  }
  time += std::to_string(dv.quot) + "::";
  dv.quot = dv.rem;
  dv = std::div(dv.quot, 60);
  if (dv.quot < 10){
    time += "0";
  }
  time += std::to_string(dv.quot) + "::";
  if (dv.rem < 10){
    time += "0";
  }
  time += std::to_string(dv.rem);
  return time;
}