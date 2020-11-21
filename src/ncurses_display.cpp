#include <curses.h>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "format.h"
#include "ncurses_display.h"
#include "system.h"

using std::string;
using std::to_string;

// 40 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(float percent) {
  std::string result{"0%"};
  int size{40};
  float bars{percent * size};

  for (int i{0}; i < size; ++i) {
    result += i <= bars ? '|' : ' ';
  }

  string display{to_string(percent * 100).substr(0, 4)};
  if (percent < 0.1 || percent == 1.0)
    display = " " + to_string(percent * 100).substr(0, 3);
  return result + " " + display + "/100%";
}

void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
  int row{0};
  std::string label = "OS: ";
  mvwprintw(window, ++row, 2, label.c_str());
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 2 + label.length(), system.OperatingSystem().c_str());
  wattroff(window, COLOR_PAIR(1));

  label = "Kernel: ";
  mvwprintw(window, ++row, 2, label.c_str());
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 2 + label.length(), system.Kernel().c_str());
  wattroff(window, COLOR_PAIR(1));

  label = "Total Processes: ";
  mvwprintw(window, ++row, 2, label.c_str());
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 2 + label.length(),
            to_string(system.TotalProcesses()).c_str());
  wattroff(window, COLOR_PAIR(1));

  label = "Running Processes: ";
  mvwprintw(window, ++row, 2, label.c_str());
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 2 + label.length(),
            to_string(system.RunningProcesses()).c_str());
  wattroff(window, COLOR_PAIR(1));

  label = "Up Time: ";
  mvwprintw(window, ++row, 2, label.c_str());
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 2 + label.length(),
            Format::ElapsedTime(system.UpTime()).c_str());
  wattroff(window, COLOR_PAIR(1));

  wrefresh(window);
}

void NCursesDisplay::DisplayCpus(System& system, WINDOW* window, int cpuCount) {
  int row{0};
  auto utilData = system.Cpu().Utilizations();

  mvwprintw(window, ++row, 2, "Memory: ");
  wattron(window, COLOR_PAIR(4));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.MemoryUtilization()).c_str());
  wattroff(window, COLOR_PAIR(4));
  mvwprintw(window, ++row, 2, "");
  mvwprintw(window, ++row, 2, "CPU: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(utilData[0]).c_str());
  wattroff(window, COLOR_PAIR(1));
  for (auto i = 1; i < cpuCount; i++) {
    std::string cpu_label = "-CPU" + std::to_string(i) + ": ";
    mvwprintw(window, ++row, 2, cpu_label.c_str());
    wattron(window, COLOR_PAIR(3));
    mvwprintw(window, row, 10, "");
    wprintw(window, ProgressBar(utilData[i]).c_str());
    wattroff(window, COLOR_PAIR(3));
  }
  wrefresh(window);
}

void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,
                                      WINDOW* window, int n) {
  int row{0};
  int const pid_column{2};
  int const user_column{9};
  int const cpu_column{16};
  int const ram_column{26};
  int const time_column{35};
  int const command_column{46};
  wattron(window, COLOR_PAIR(2));
  mvwprintw(window, ++row, pid_column, "PID");
  mvwprintw(window, row, user_column, "USER");
  mvwprintw(window, row, cpu_column, "CPU[%%]");
  mvwprintw(window, row, ram_column, "RAM[MB]");
  mvwprintw(window, row, time_column, "TIME+");
  mvwprintw(window, row, command_column, "COMMAND");
  wattroff(window, COLOR_PAIR(2));
  for (int i = 0; i < n; ++i) {
    mvwprintw(window, ++row, pid_column, string(user_column - pid_column,' ').c_str());
    mvwprintw(window, row, pid_column, to_string(processes[i].Pid()).c_str());
    mvwprintw(window, row, user_column, string(cpu_column - user_column,' ').c_str());
    mvwprintw(window, row, user_column, processes[i].User().c_str());
    float cpu = processes[i].CpuUtilization() * 100;
    mvwprintw(window, row, cpu_column, string(4,' ').c_str());
    mvwprintw(window, row, cpu_column, to_string(cpu).substr(0, 4).c_str());
    mvwprintw(window, row, ram_column, string(time_column - ram_column,' ').c_str());
    mvwprintw(window, row, ram_column, processes[i].Ram().c_str());
    mvwprintw(window, row, time_column,
              Format::ElapsedTime(processes[i].UpTime()).c_str());
    mvwprintw(window, row, command_column, string(window->_maxx - command_column,' ').c_str());
    mvwprintw(window, row, command_column,
              processes[i].Command().substr(0, window->_maxx - command_column).c_str());
  }
  wrefresh(window);
}

[[noreturn]] void NCursesDisplay::Display(System& system, int n) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  cbreak();       // terminate ncurses on ctrl + c
  start_color();  // enable color

  int x_max{getmaxx(stdscr)};
  int cpuCount = system.Cpu().getProcCount();

  // stacked boxes format
  WINDOW* system_window = newwin(7, x_max - 1, 0, 0);
  WINDOW* cpu_window = newwin(4 + cpuCount, x_max - 1, system_window->_maxy + 1, 0);
  WINDOW* process_window =
      newwin(3 + n, x_max - 1, system_window->_maxy + cpu_window->_maxy + 2, 0);

  // TODO: remove constant dimensions and make them proportional to window res
  //  WINDOW* cpu_window = newwin(8, 65 , 0, 0);
  //  WINDOW* system_window = newwin(8, x_max - cpu_window->_maxx - 3, 0,
  //  cpu_window->_maxx + 2);
  //  WINDOW* process_window =
  //      newwin(3 + n, x_max - 1, system_window->_maxy + 1, 0);

  while (true) {
//    endwin();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

    box(system_window, 0, 0);
    box(process_window, 0, 0);
    box(cpu_window, 0, 0);

    DisplaySystem(system, system_window);
    DisplayProcesses(system.Processes(), process_window, n);
    DisplayCpus(system, cpu_window, cpuCount);
    wrefresh(system_window);
    wrefresh(process_window);
    wrefresh(cpu_window);

    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds (500));
  }
  endwin();
}