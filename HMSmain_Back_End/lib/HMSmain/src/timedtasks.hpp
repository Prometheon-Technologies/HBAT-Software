#pragma once
#ifndef TIMEDTASKS_HPP
#define TIMEDTASKS_HPP
#include "defines.hpp"
#include <blinker.h>
/* #include <iostream>     // std::cout
#include <functional>   // std::bind */

class TimedTasks : public timeObj,
                   public idler
{
public:
  TimedTasks();
  virtual ~TimedTasks();
  virtual void Run_NetworkCheck_Background_every_10_Seconds();
  virtual void Run_Check_DataJSON_5();
  void setCallback(void (*funct)(void));
  void setSeconds(float seconds);
  virtual void idle(void);
  void updateCurrentData();

  void (*callback)(void);
  void SetupTimers();

private:
};

extern TimedTasks timedTasks;
#endif