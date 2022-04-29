#include "i2cscan.hpp"

Scanner::Scanner(void)
{
}

Scanner::~Scanner(void)
{
}

void Scanner::SetupScan()
{
  Wire.begin();
  //Serial.begin(115200);
  log_i("\nI2C Scanner");
}

void Scanner::BeginScan()
{
  byte error, address;
  int nDevices;
  log_i("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      log_i("%c", address, HEX);
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      log_i("%c", address, HEX);
    }
  }
  if (nDevices == 0)
  {
    log_i("No I2C devices found\n");
  }
  else
  {
    log_i("done\n");
  }
  delay(5000);
}

Scanner Scan;
