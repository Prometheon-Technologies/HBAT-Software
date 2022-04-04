#include "HMS.hpp"
#include <esp_adc_cal.h>

// ESP 32 (requires resistors to step down the logic voltage)
// ACS712  ACS(25, 5.0, 4095, 185);
uint8_t _amppin = 10;
ACS712 ACS(_amppin, 5.0, 4095, 100);

bool mux_enabled_voltage = false;
bool mux_enabled_amps = false;
int power_mux_pin_amps = 20;
int power_mux_pin_voltage = 46;
bool POWER_MUX_ENABLED_AMPS = digitalRead(power_mux_pin_amps);
bool POWER_MUX_ENABLED_VOLTAGE = digitalRead(power_mux_pin_voltage);

HMS::HMS()
{
}

HMS::~HMS()
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
    return (float)((analogRead(pinnumber) * 3300.0) / 4095.0);
}

/******************************************************************************
 * Function: Read Voltage per Cell and average
 * Description: This function loops through 5 times and reads the voltage from each celland stores it in a float array
 * Parameters: None
 * Return: float array
 ******************************************************************************/
float *HMS::readSensAndCondition()
{
    int numtoaverage = 10;
    float *cell_voltage = (float *)malloc(sizeof(numtoaverage));
    for (int i = 0; i < numtoaverage; i++)
    {
        cell_voltage[0] = readVoltage(1);
        cell_voltage[1] = readVoltage(2);
        cell_voltage[2] = readVoltage(3);
        cell_voltage[3] = readVoltage(4); // voltage leads on analog pins ADC1 - ADC2 pins do not work when wifi is enabled
        cell_voltage[4] = readVoltage(5);
        cell_voltage[5] = readVoltage(6);
        cell_voltage[6] = readVoltage(7);
        cell_voltage[7] = readVoltage(8);
        cell_voltage[8] = readVoltage(9);
        digitalWrite(power_mux_pin_amps, LOW);
        my_delay(1000L);
        if (!mux_enabled_amps && !POWER_MUX_ENABLED_AMPS)
        {
            mux_enabled_voltage = true;
            digitalWrite(power_mux_pin_voltage, HIGH);
            cell_voltage[9] = readVoltage(10);
        }
        digitalWrite(power_mux_pin_voltage, LOW);
        my_delay(1000L);
        mux_enabled_voltage = false;
    }

    for (int i = 0; i < numtoaverage; i++)
    {
        cell_voltage[i] = cell_voltage[i] / numtoaverage;
    }
    return cell_voltage;
}

/******************************************************************************
 * Function: Summation of elements in an array
 * Description: This function sums the elements in an array
 * Parameters: float array and int size
 * Return: float sum
 ******************************************************************************/
float HMS::sumArray(float array[], int size)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += array[i];
    }
    return sum;
}

/******************************************************************************
 * Function: Setup the Stack Voltage
 * Description: This function setups the mean Stack Voltage by calculating the sum of the cell_voltage array
 * Parameters: None
 * Return: The mean Stack Voltage
 ******************************************************************************/
float HMS::StackVoltage()
{
    float array[10];
    for (int i = 0; i < 10; i++)
    {
        array[i] = readSensAndCondition()[i];
    }
    int size = sizeof(array) / sizeof(array[0]);
    float sum = sumArray(array, size);
    Serial.println(sum);
    return sum;
}

/******************************************************************************
 * Function: Setup the ACS712 sensor
 * Description: This function setups the ACS712 sensor by calculating the automidpoint and setting the sensitivity to the correct value
 * Parameters: None
 * Return: None
 * ACS712 5A  uses 185 mV per A
 * ACS712 20A uses 100 mV per A
 * ACS712 30A uses  66 mV per A
 ******************************************************************************/
void HMS::setupSensor()
{
    // Setup the ACS712 sensor
    digitalWrite(power_mux_pin_voltage, LOW);
    if (!mux_enabled_voltage && !POWER_MUX_ENABLED_VOLTAGE)
    {
        mux_enabled_amps = true;
        digitalWrite(power_mux_pin_amps, HIGH);
        ACS.autoMidPoint();
        my_delay(1000L);
        digitalWrite(power_mux_pin_amps, LOW);
        mux_enabled_amps = false;
    }
}

/******************************************************************************
 * Function: Read Amps for stack
 * Description: This function reads the current from the ACS712 sensor and returns the value
 * Parameters: None
 * Return: String
 ******************************************************************************/
int HMS::readAmps()
{
    // Setup the ACS712 sensor
    digitalWrite(power_mux_pin_voltage, LOW);
    if (!mux_enabled_voltage && !POWER_MUX_ENABLED_VOLTAGE)
    {
        mux_enabled_amps = true;
        digitalWrite(power_mux_pin_amps, HIGH);
        int mA = ACS.mA_DC();
        char buffer[sizeof(mA)];
        digitalWrite(power_mux_pin_amps, LOW);
        mux_enabled_amps = false;
        my_delay(1000L);
        snprintf(buffer, sizeof(buffer), "Stack Amps: %d", mA);
        Serial.println(buffer);
        return mA;
    }
}

/******************************************************************************
 * Function: Return Charge Status of Stack
 * Description: This function reads the current stack voltage and returns a number representing the charge status
 * Parameters: None
 * Return: String
 ******************************************************************************/
int HMS::ChargeStatus()
{
    if (StackVoltage() < 8.00 && StackVoltage() >= 0.00)
    {
        return 1;
        printf("Stack is fully discharged");
    }
    else if (StackVoltage() < 10.00 && StackVoltage() >= 8.00)
    {
        return 2;
        printf("Stack needs to be charged");
    }
    else if (StackVoltage() < 13.00 && StackVoltage() >= 11.0)
    {
        return 3;
        printf("Stack has a full charge");
    }
    else if (StackVoltage() < 14.0 && StackVoltage() >= 12.0)
    {
        return 4;
        printf("Stack is charging");
    }
    else if (StackVoltage() < 16.0 && StackVoltage() >= 14.0)
    {
        return 5;
        printf("Stack has encountered an overcharge condition");
    }

    else
    {
        return 0;
        printf("Stack is in an unknown state");
    }
    return 0;
}

/******************************************************************************
 * Function: Calibrate the ACS712 sensor
 * Description: This function allows for manual calibration of the ACS712 sensor by setting the sensitivity to the correct value
 * Parameters: None
 * Return: None
 ******************************************************************************/
void HMS::calibrateAmps()
{
    // Setup the ACS712 sensor
    digitalWrite(power_mux_pin_voltage, LOW);
    if (!mux_enabled_voltage && !POWER_MUX_ENABLED_VOLTAGE)
    {
        mux_enabled_amps = true;
        digitalWrite(power_mux_pin_amps, HIGH);
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
                Serial.print("," + String(ACS.getMidPoint()));
                break;
            case '*':
                ACS.setmVperAmp(ACS.getmVperAmp() * 1.05);
                break;
            case '/':
                ACS.setmVperAmp(ACS.getmVperAmp() / 1.05);
                Serial.print("," + String(ACS.getmVperAmp()));
                break;
            default:
                Serial.println("No input detected");
            }
        }
        my_delay(10000L);
        digitalWrite(power_mux_pin_amps, LOW);
        mux_enabled_amps = false;
        my_delay(1000L);
    }
}

HMS HMSmain;