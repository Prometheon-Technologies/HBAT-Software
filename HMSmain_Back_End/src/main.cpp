#include <Arduino.h>

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x) Serial.printf(x)
#define debugCalibrateAmps() HMSmain.calibrateAmps()

#else
#define debug(x)
#define debugln(x)
#define debugf(x)
#define debugCalibrateAmps()
#endif

#define LED1 37
#define LED2 47

/******************************************************************************
 * Function: Main Loop
 * Description: This is the main loop for the whole program.
 * Parameters: None
 * Return: None
 ******************************************************************************/

// if (input_voltage < 0.50 && input_voltage >= 0.00 )
//{
// digitalWrite(2, HIGH);
// delay (30);
// digitalWrite(2, LOW);
// delay (30);
// }
// else if (input_voltage < 1.00 && input_voltage >= 0.50)
//{
//  DO STUFF
// }
// else if (input_voltage < 1.50 && input_voltage >= 1.00)
//{
//  DO STUFF
// }
// else if (input_voltage < 2.00 && input_voltage >= 1.50)
//{
//  DO STUFF
// }
// else if (input_voltage < 2.50 && input_voltage >= 2.00)
//{
//  DO STUFF
// }
// else if (input_voltage < 3.00 && input_voltage >= 2.50)
//{
//  DO STUFF
// }
// else if (input_voltage < 3.50 && input_voltage >= 3.00)
//{
//  DO STUFF
// }
// else if (input_voltage < 4.00 && input_voltage >= 3.50)
//{
//  DO STUFF
// }
// else if (input_voltage < 4.50 && input_voltage >= 4.00)
//{
//  DO STUFF
// }
// else if (input_voltage < 5.00 && input_voltage >= 4.50)
//{
//  DO STUFF
// }

void setup(){}

void loop(){}