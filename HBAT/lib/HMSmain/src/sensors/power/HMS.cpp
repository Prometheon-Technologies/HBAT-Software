#include "HMS.hpp"

// ESP 32 (requires resistors to step down the logic voltage)
// ACS712  ACS(25, 5.0, 4095, 185);
#if !PRODUCTION
uint8_t _amppin = 33;
#else
uint8_t _amppin = 10;
#endif

ACS712 ACS(_amppin, 5.0, 4095, 100);

HMS::Cell_Voltages_t cell_voltages;

HMS::HMS() : _mux_enabled_voltage(false),
             _mux_enabled_amps(false),
             _power_mux_pin_amps(20),
             _power_mux_pin_voltage(46) {}

HMS::~HMS() {}

void HMS::begin()
{
#if !PRODUCTION
    // 36, 39, 34, 35, 32;
    byte _voltage_pins[] = {36, 39, 34, 35, 32};
    for (byte i = 0; i < sizeof(_voltage_pins) / sizeof(_voltage_pins[0]); i++)
    {
        pinMode(_voltage_pins[i], INPUT);
    }
    pinMode(33, INPUT);
#else
    byte pin = 1;
    for (byte i = 0; i < 9; i++)
    {
        pinMode(pin++, INPUT);
    }
#endif // !PRODUCTION

    // Set up the power mux
    pinMode(_power_mux_pin_amps, OUTPUT);
    pinMode(_power_mux_pin_voltage, OUTPUT);
    _POWER_MUX_ENABLED_AMPS = digitalRead(_power_mux_pin_amps);
    _POWER_MUX_ENABLED_VOLTAGE = digitalRead(_power_mux_pin_voltage);
}

/******************************************************************************
 * Function: Read Voltage per Cell
 * Description: Reads the voltage from the cell and returns the value
 * Parameters: int pinnumber
 * Return: float
 ******************************************************************************/
float HMS::readVoltage(byte pinnumber)
{
    return (float)((analogRead(pinnumber) * 3300.0) / 4095.0);
}

/* ADC readings v voltage
 *  y = -0.000000000009824x3 + 0.000000016557283x2 + 0.000854596860691x + 0.065440348345433
 * Polynomial curve match, based on raw data thus:
 *   464     0.5
 *  1088     1.0
 *  1707     1.5
 *  2331     2.0
 *  2951     2.5
 *  3775     3.0
 *
 *  Serial.println(readVoltagePolynomial(36), 3);
 */
double HMS::readVoltagePolynomial(byte pinnumber)
{
    double reading = analogRead(pinnumber); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
    if (reading < 1 || reading > 4095)
    {
        return 0;
    }

    // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
    return -0.000000000000016 * pow(reading, 4) + 0.000000000118171 * pow(reading, 3) - 0.000000301211691 * pow(reading, 2) + 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required

/******************************************************************************
 * Function: Read Voltage per Cell and average
 * Description: This function loops through 5 times and reads the voltage from each celland stores it in a float array
 * Parameters: None
 * Return: float array
 ******************************************************************************/
HMS::Cell_Voltages_t HMS::readSensAndCondition()
{
    if (!Cell_Temp.setSensorCount())
    {
        Serial.println("No cells detected");
        cell_voltages = {0};
        return cell_voltages;
    }

    byte numtoaverage = 5;

    for (byte i = 0; i < numtoaverage; i++)
    {
#if !PRODUCTION

        /* for (auto &pin : _voltage_pins)
        {
            _cell_voltage[i] += readVoltage(pin);
        } */
        byte _voltage_pins[] = {36, 39, 34, 35, 32};
        for (byte i = 0; i < sizeof(_voltage_pins) / sizeof(_voltage_pins[0]); i++)
        {
            cell_voltages.cell_voltage[i] += readVoltage(_voltage_pins[i]);
        }

        digitalWrite(_power_mux_pin_amps, LOW);
        my_delay(0.1L);
        if (!_mux_enabled_amps && !_POWER_MUX_ENABLED_AMPS)
        {
            _mux_enabled_voltage = true;
            digitalWrite(_power_mux_pin_voltage, HIGH);
            cell_voltages.cell_voltage[5] = readVoltage(33);
        }
        digitalWrite(_power_mux_pin_voltage, LOW);
        my_delay(0.1L);
        _mux_enabled_voltage = false;
#else

        byte pin = 9;
        for (byte i = 8; i > 8; i--)
        {
            cell_voltages.cell_voltage[i] += readVoltage(pin--);
        }
        digitalWrite(_power_mux_pin_amps, LOW);
        my_delay(0.1L);
        if (!_mux_enabled_amps && !_POWER_MUX_ENABLED_AMPS)
        {
            _mux_enabled_voltage = true;
            digitalWrite(_power_mux_pin_voltage, HIGH);
            cell_voltages.cell_voltage[9] = readVoltage(10);
        }
        digitalWrite(_power_mux_pin_voltage, LOW);
        my_delay(0.1L);
        _mux_enabled_voltage = false;
#endif // PRODUCTION
    }

    for (byte i = 0; i < sizeof(cell_voltages.cell_voltage) / sizeof(cell_voltages.cell_voltage[0]); i++)
    {
        cell_voltages.cell_voltage[i] = cell_voltages.cell_voltage[i] / numtoaverage;
    }
    return cell_voltages;
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
    digitalWrite(_power_mux_pin_voltage, LOW);
    if (!_mux_enabled_voltage && !_POWER_MUX_ENABLED_VOLTAGE)
    {
        _mux_enabled_amps = true;
        digitalWrite(_power_mux_pin_amps, HIGH);
        ACS.autoMidPoint();
        my_delay(0.1L);
        digitalWrite(_power_mux_pin_amps, LOW);
        _mux_enabled_amps = false;
    }
}

/******************************************************************************
 * Function: Read MilliAmps for stack
 * Description: This function reads the current from the ACS712 sensor and returns the value
 * Parameters: None
 * Return: Integer
 *************************
 *****************************************************/
int HMS::readAmps()
{
    // Setup the ACS712 sensor
    digitalWrite(_power_mux_pin_voltage, LOW);
    if (!_mux_enabled_voltage && !_POWER_MUX_ENABLED_VOLTAGE)
    {
        _mux_enabled_amps = true;
        digitalWrite(_power_mux_pin_amps, HIGH);
        int mA = ACS.mA_DC();
        String amps = String(mA);
        digitalWrite(_power_mux_pin_amps, LOW);
        _mux_enabled_amps = false;
        log_i("Stack mA: %s\n", amps.c_str());
        my_delay(0.1L);
        return amps.toInt();
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
    digitalWrite(_power_mux_pin_voltage, LOW);
    if (!_mux_enabled_voltage && !_POWER_MUX_ENABLED_VOLTAGE)
    {
        _mux_enabled_amps = true;
        digitalWrite(_power_mux_pin_amps, HIGH);
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
                log_i(", %d", ACS.getMidPoint());
                break;
            case '*':
                ACS.setmVperAmp(ACS.getmVperAmp() * 1.05);
                break;
            case '/':
                ACS.setmVperAmp(ACS.getmVperAmp() / 1.05);
                log_i(", %d", ACS.getmVperAmp());
                break;
            default:
                log_i("No input detected");
            }
        }
        my_delay(0.01L);
        digitalWrite(_power_mux_pin_amps, LOW);
        _mux_enabled_amps = false;
        my_delay(0.1L);
    }
}

HMS HMSmain;