
#include "timedtasks.hpp"

TimedTasks::TimedTasks(void)
{
}

TimedTasks::~TimedTasks(void)
{
}

void TimedTasks::SetupTimers() // TODO: CALL IN THE MAIN SETUP
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
    network.CheckNetworkLoop(); // Check the network state and connect to the AP if needed
    ReadTimer_10.start();
  }
}

void TimedTasks::Run_Check_DataJSON_5()
{
  if (ReadTimer2.ding())
  {
    Accumulate_Data.InitAccumulateDataJson();
    Hum.SFM3003();
    ReadTimer2.start();
  }
}

void TimedTasks::updateCurrentData()// check to see if the data has changed
{
  if (ReadTimer_10_2.ding())
  {
    // call to save config if config has changed
    cfg.saveConfig();
    SERIAL_DEBUG_LNF("Heap: %d", system_get_free_heap_size())
    ReadTimer_10_2.start();
  }
}

TimedTasks timedTasks;