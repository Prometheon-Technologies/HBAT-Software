
#include "timedtasks.hpp"

timeObj ReadTimer;
timeObj ReadTimer_10;
timeObj ReadTimer2_10;
timeObj ReadTimer3_10;
timeObj ReadTimer2;
timeObj ReadTimer3;
timeObj ReadTimer_10_2;

TimedTasks::TimedTasks()
{
}

TimedTasks::~TimedTasks()
{
}

void TimedTasks::SetupTimers()
{
  ReadTimer.setTime(5000);
  ReadTimer_10.setTime(10000);
  ReadTimer2_10.setTime(10000);
  ReadTimer3_10.setTime(10000);
  ReadTimer2.setTime(5000);
  ReadTimer3.setTime(5000);
  ReadTimer_10_2.setTime(10000);
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

void TimedTasks::Run_NetworkCheck_Background_every_10_Seconds()
{
  if (ReadTimer_10.ding())
  {
    //network.CheckNetworkLoop(); // Check the network state and connect to the AP if needed
    ReadTimer_10.start();
  }
}

void TimedTasks::Run_Check_DataJSON_5()
{
  if (ReadTimer2.ding())
  {
    accumulatedata.InitAccumulateDataJson();
    //Hum.loopSFM3003();
    ReadTimer2.start();
  }
}

void TimedTasks::updateCurrentData() // check to see if the data has changed
{
  if (ReadTimer_10_2.ding())
  {
    // call to save config if config has changed
    //cfg.updateCurrentData();
    //SERIAL_DEBUG_LNF("Heap: %d", ESP.getFreeHeap());
    ReadTimer_10_2.start();
  }
}

TimedTasks timedTasks;