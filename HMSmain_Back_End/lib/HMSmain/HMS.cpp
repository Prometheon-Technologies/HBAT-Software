
#include <HMS.h>

// ESP 32 (requires resistors to step down the logic voltage)
// ACS712  ACS(25, 5.0, 4095, 185);
uint8_t _amppin = 18;
ACS712 ACS(_amppin, 5.0, 4095, 100);
CellTemp Cell_Temp = CellTemp();

HMS::HMS()
{
}

/******************************************************************************
 * Function: Read Voltage per Cell
 * Description: Reads the voltage from the cell and returns the value
 * Parameters: int pinnumber
 * Return: float
 ******************************************************************************/
float HMS::readVoltage(int pinnumber)
{
    return (float)((analogRead(pinnumber) * 5.0) / 1024.0);
}

/******************************************************************************
 * Function: Read Voltage per Cell and average
 * Description: This function loops through 5 times and reads the voltage from each cell. Adding the last result to the next one then averages these values - and stores it in a float array
 * Parameters: None
 * Return: float array
 ******************************************************************************/
float *HMS::readSensAndCondition()
{
    float *cell_voltage = new float[5];
    for (int i = 0; i < 5; i++)
    {
        cell_voltage[0] += readVoltage(39); // sensor on analog pin
        cell_voltage[1] += readVoltage(34);
        cell_voltage[2] += readVoltage(35);
        cell_voltage[3] += readVoltage(32);
        cell_voltage[4] += readVoltage(33);
        cell_voltage[5] += readVoltage(25);
        cell_voltage[6] += readVoltage(26);
        cell_voltage[7] += readVoltage(27);
        cell_voltage[8] += readVoltage(14);
        cell_voltage[9] += readVoltage(12);

        delay(100);
    }

    for (int i = 0; i < sizeof(cell_voltage); i++)
    {
        cell_voltage[i] = cell_voltage[i] / 5;
    }
    return cell_voltage;
}

// ACS712 5A  uses 185 mV per A
// ACS712 20A uses 100 mV per A
// ACS712 30A uses  66 mV per A
/******************************************************************************
 * Function: Setup the ACS712 sensor
 * Description: This function setups the ACS712 sensor by calculating the automidpoint and setting the sensitivty to the correct value
 * Parameters: None
 * Return: None
 ******************************************************************************/
void HMS::setupSensor()
{
    ACS.autoMidPoint();
}

/******************************************************************************
 * Function: Read Amps for stack
 * Description: This function reads the current from the ACS712 sensor and returns the value
 * Parameters: None
 * Return: String
 ******************************************************************************/
String HMS::readAmps()
{
    int mA = ACS.mA_DC();
    String Amps = String(mA);
    Serial.println("," + Amps);
    return Amps;
}

/******************************************************************************
 * Function: Calibrate the ACS712 sensor
 * Description: This function allows for manual calibration of the ACS712 sensor by setting the sensitivity to the correct value
 * Parameters: None
 * Return: None
 ******************************************************************************/
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
            // SerialBT.print("," + ACS.getmVperAmp());
            break;
        default:
            Serial.printf("No input detected");
        }
    }
    delay(1000);
}