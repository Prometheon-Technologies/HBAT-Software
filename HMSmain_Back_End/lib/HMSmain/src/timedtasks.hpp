#pragma once
#ifndef TIMEDTASKS_HPP
#define TIMEDTASKS_HPP
#include "defines.hpp"
#include <timeObj.h>
#include <idlers.h>
/* #include <iostream>     // std::cout
#include <functional>   // std::bind */

class TimedTasks : public timeObj,
                   public idler
{
public:
  TimedTasks();
  virtual ~TimedTasks();
  void setCallback(void (*funct)(void));
  void setSeconds(float seconds);
  void (*callback)(void);
  virtual void idle(void);
  /* void updateCurrentData();
  void SetupTimers();
  void Run_Check_DataJSON_5();
  void Run_NetworkCheck_Background_every_10_Seconds(); */


private:
};

extern TimedTasks timedTasks;
extern TimedTasks timedTasks_2;

#endif