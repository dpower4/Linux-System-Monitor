#include <string>
#include <chrono>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
//  auto out_format{std::chrono::seconds(seconds)};
//  return format("{:%T}", out_format);;

  string formatted_time;
  int hours = seconds / 3600;
  seconds = seconds % 3600;
  int mins = seconds / 60;
  seconds = seconds % 60;

  if(hours < 10){
    formatted_time += "0";
  }
  formatted_time += std::to_string(hours) + ":";
  if(mins < 10){
    formatted_time += "0";
  }
  formatted_time += std::to_string(mins) + ":";
  if(seconds < 10){
    formatted_time += "0";
  }
  formatted_time += std::to_string(seconds);

  return formatted_time;
}