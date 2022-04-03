#include "HMS.hpp"
#include <esp_adc_cal.h>

// ESP 32 (requires resistors to step down the logic voltage)
// ACS712  ACS(25, 5.0, 4095, 185);
uint8_t _amppin = 18;
ACS712 ACS(_amppin, 5.0, 4095, 100);

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
int HMS::readVoltage(adc1_channel_t pinnumber)
{
    if (!PRODUCTION)
    {
        // Check TP is burned into eFuse
        if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
        {
            printf("eFuse Two Point: Supported\n");
            // setup esp_adc_cal_characteristics_t
            esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t();
            // Characterize ADC
            esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, adc_chars);
            // use adc1 API to read EFUSE value
            if (esp_adc_cal_get_voltage(pinnumber, ESP_ADC_CAL_VAL_EFUSE_TP, &efuse_voltage) == ESP_OK)
            {

                printf("eFuse Two Point: %dmV\n", efuse_voltage);
            }
        }
        else
        {
            printf("eFuse Two Point: NOT supported\n");
        }

        // Check Vref is burned into eFuse
        if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
        {
            printf("eFuse Vref: Supported\n");
        }
        else
        {
            printf("eFuse Vref: NOT supported\n");
        }
    }
    else
    {
    }

    // Read the voltage from the cell
    adc1_config_channel_atten(pinnumber, ADC_ATTEN_DB_11);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc_power_acquire();

    uint32_t voltage = adc1_get_raw(pinnumber);
    int voltage_mv = voltage * 3300.0 / 4095.0;

    // return (float)((analogRead(pinnumber) * 3.3) / 4096.0);
    esp_adc_cal_raw_to_voltage(); // convert adc to voltage
    adc_power_release();
}

/******************************************************************************
 * Function: Read Voltage per Cell and average
 * Description: This function loops through 5 times and reads the voltage from each cell. Adding the last result to the next one then averages these values - and stores it in a float array
 * Parameters: None
 * Return: float array
 ******************************************************************************/
int *HMS::readSensAndCondition()
{
    int numtoaverage = 10;
    int *cell_voltage = (int *)malloc(sizeof(int) * 10);
    for (int i = 0; i < numtoaverage; i++)
    {
        cell_voltage[0] = readVoltage(36); // sensor on analog pins ADC1 && ADC2 - ADC2 pins do not work when wifi is enabled
        cell_voltage[1] = readVoltage(39);
        cell_voltage[2] = readVoltage(34);
        cell_voltage[3] = readVoltage(35);
        cell_voltage[4] = readVoltage(32);
        cell_voltage[5] = readVoltage(33);
        cell_voltage[6] = readVoltage(25);
        cell_voltage[7] = readVoltage(26);
        cell_voltage[8] = readVoltage(27);
        cell_voltage[9] = readVoltage(14);
    }

    for (int i = 0; i < numtoaverage; i++)
    {
        cell_voltage[i] = cell_voltage[i] / numtoaverage;
    }
    return cell_voltage;
}

int HMS::sumArray(int array[], int size)
{
    int sum = 0;
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
int HMS::StackVoltage()
{
    int array[10];
    for (int i = 0; i < 10; i++)
    {
        array[i] = readSensAndCondition()[i];
    }
    int size = sizeof(array) / sizeof(array[0]);
    int sum = sumArray(array, size);
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
    ACS.autoMidPoint();
}

/******************************************************************************
 * Function: Read Amps for stack
 * Description: This function reads the current from the ACS712 sensor and returns the value
 * Parameters: None
 * Return: String
 ******************************************************************************/
int HMS::readAmps()
{
    int mA = ACS.mA_DC();
    String Amps = String(mA);
    Serial.println("Stack Amps:" + Amps);
    return mA;
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
    my_delay(100000L);
}

HMS HMSmain;