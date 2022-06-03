#include <Arduino.h>
#include "SensirionI2CSfmSf06.h"
#include <Wire.h>

SensirionI2CSfmSf06 sfmSf06;

void setup() {

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    Wire.begin();

    uint16_t error;
    char errorMessage[256];

    /**
     * select the proper i2c address for your sensor
     * see datasheet of your sensor
     *
     */
    sfmSf06.begin(Wire, ADDR_SFM3003_300_CE);

    error = sfmSf06.stopContinuousMeasurement();

    if (error) {
        Serial.print("Error trying to execute stopContinuousMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    uint32_t productIdentifier;
    uint8_t serialNumber[8];
    uint8_t serialNumberSize = 8;

    error = sfmSf06.readProductIdentifier(productIdentifier, serialNumber,
                                          serialNumberSize);

    if (error) {
        Serial.print("Error trying to execute readProductIdentifier(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Product Identifer:");
        Serial.println(productIdentifier);
        Serial.print("Serial Number:");
        Serial.print("0x");
        for (size_t i = 0; i < serialNumberSize; i++) {
            uint8_t value = serialNumber[i];
            Serial.print(value < 16 ? "0" : "");
            Serial.print(value, HEX);
        }
        Serial.println();
    }

    sfmSf06.startO2ContinuousMeasurement();
    Serial.print("flow");
    Serial.print("\t");
    Serial.println("temperature");
}

void loop() {
    uint16_t error;
    char errorMessage[256];
    float flow = 0;
    float temperature = 0;
    uint16_t status = 0;

    // Read Measurement
    error = sfmSf06.readMeasurementData(flow, temperature, status);
    delay(64);
    if (!error) {
        Serial.print(flow);
        Serial.print("\t");
        Serial.println(temperature);
    }
}
