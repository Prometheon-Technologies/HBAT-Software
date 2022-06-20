#ifndef SECURE_OTA_HPP
#define SECURE_OTA_HPP
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <Update.h>
#include <BintrayClient.hpp>

void checkFirmwareUpdates();
void processOTAUpdate(const String &version);

#endif // SECURE_OTA_HPP