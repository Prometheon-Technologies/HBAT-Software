
#include "timedtasks.hpp"

TimedTasks::TimedTasks()
{
}

TimedTasks::~TimedTasks()
{
}

// Timer delay Settings
void TimedTasks::setCallback(void (*funct)(void))
{
  callback = funct;
  hookup();
}

void TimedTasks::setSeconds(float seconds) { setTime(seconds * 1000); }

void TimedTasks::idle(void)
{
  if (ding() && callback)
  {
    stepTime();
    callback();
  }
}

TimedTasks timedTasks;
TimedTasks timedTasks_2;