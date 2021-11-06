#include <BluetoothSerial.h>
#include <Wire.h>
#include <ACS712.h>
#include <Adafruit_SHT31.h>
#include <HMS.h>
#include <Arduino.h>
#include <Humidity.h>

uint8_t _amppin = 18;
ACS712 ACS(_amppin, 5.0, 4095, 100);

// ESP 32 example (requires resistors to step down the logic voltage)
//ACS712  ACS(25, 5.0, 4095, 185);

int voltageValues[10];

HMS::HMS()
{
}

float HMS::readVoltage(int pinnumber)
{
    return (float)((analogRead(pinnumber) * 5.0) / 1024.0);
}

float *HMS::readSensAndCondition()
{
    float sval[10] = {0};

    for (int i = 0; i < 5; i++)
    {
        sval[0] += readVoltage(39); // sensor on analog pin
        sval[1] += readVoltage(34);
        sval[2] += readVoltage(35);
        sval[3] += readVoltage(32);
        sval[4] += readVoltage(33);
        sval[5] += readVoltage(25);
        sval[6] += readVoltage(26);
        sval[7] += readVoltage(27);
        sval[8] += readVoltage(14);
        sval[9] += readVoltage(12);

        delay(100);
    }

    for (int i = 0; i < sizeof(sval); i++)
    {
        sval[i] = sval[i] / 5;
    }

    return sval;
}

//if (input_voltage < 0.50 && input_voltage >= 0.00 )
//{
//digitalWrite(2, HIGH);
//delay (30);
//digitalWrite(2, LOW);
//delay (30);
//}
//else if (input_voltage < 1.00 && input_voltage >= 0.50)
//{
// DO STUFF
//}
//else if (input_voltage < 1.50 && input_voltage >= 1.00)
//{
// DO STUFF
//}
//else if (input_voltage < 2.00 && input_voltage >= 1.50)
//{
// DO STUFF
//}
//else if (input_voltage < 2.50 && input_voltage >= 2.00)
//{
// DO STUFF
//}
//else if (input_voltage < 3.00 && input_voltage >= 2.50)
//{
// DO STUFF
//}
//else if (input_voltage < 3.50 && input_voltage >= 3.00)
//{
// DO STUFF
//}
//else if (input_voltage < 4.00 && input_voltage >= 3.50)
//{
// DO STUFF
//}
//else if (input_voltage < 4.50 && input_voltage >= 4.00)
//{
// DO STUFF
//}
//else if (input_voltage < 5.00 && input_voltage >= 4.50)
//{
// DO STUFF
//}

// ACS712 5A  uses 185 mV per A
// ACS712 20A uses 100 mV per A
// ACS712 30A uses  66 mV per A
void HMS::setupSensor()
{
    ACS.autoMidPoint();
}

float HMS::*readAmps()
{
    int mA = ACS.mA_DC();
    String Amps = String(mA);
    //SerialandBT(Amps);
    Serial.println("," + Amps);
}

void HMS::calibrateAmps()
{
    if (Serial.available())
    {
        char c = Serial.read();
        switch (c)
        {
        case '+':
            ACS.incMidPoint();
            break;
        case '-':
            ACS.decMidPoint();
            break;
        case '0':
            ACS.setMidPoint(512);
            Serial.print("," + ACS.getMidPoint());
            break;
        case '*':
            ACS.setmVperAmp(ACS.getmVperAmp() * 1.05);
            break;
        case '/':
            ACS.setmVperAmp(ACS.getmVperAmp() / 1.05);
            //SerialBT.print("," + ACS.getmVperAmp());
            break;
        default:
            Serial.printf("No input detected");
        }
    }
    delay(1000);
}