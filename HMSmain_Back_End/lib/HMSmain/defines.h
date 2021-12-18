// define EEPROM settings
// https://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-für-parameter-verwenden
// define debugging MACROS
#define HMS_DEBUG 1
#if HMS_DEBUG != 0
#define SERIAL_DEBUG_ADD(s) Serial.print(s);
#define SERIAL_DEBUG_ADDF(format, ...) Serial.printf(format, __VA_ARGS__);
#define SERIAL_DEBUG_EOL Serial.print("\n");
#define SERIAL_DEBUG_BOL Serial.printf("DEBUG [%lu]: ", millis());
#define SERIAL_DEBUG_LN(s) SERIAL_DEBUG_BOL SERIAL_DEBUG_ADD(s) SERIAL_DEBUG_EOL
#define SERIAL_DEBUG_LNF(format, ...) SERIAL_DEBUG_BOL SERIAL_DEBUG_ADDF(format, __VA_ARGS__) SERIAL_DEBUG_EOL
#else
#define SERIAL_DEBUG_ADD(s) \
    do                      \
    {                       \
    } while (0);
#define SERIAL_DEBUG_ADDF(format, ...) \
    do                                 \
    {                                  \
    } while (0);
#define SERIAL_DEBUG_EOL \
    do                   \
    {                    \
    } while (0);
#define SERIAL_DEBUG_BOL \
    do                   \
    {                    \
    } while (0);
#define SERIAL_DEBUG_LN(s) \
    do                     \
    {                      \
    } while (0);
#define SERIAL_DEBUG_LNF(format, ...) \
    do                                \
    {                                 \
    } while (0);
#endif
