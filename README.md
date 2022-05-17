<p align="center">
   <img width="250px" height="120px" title="HBAT Logo" src="img/logo.png">
</p>

----

# HBAT-Software Public repo

- [HBAT-Software Public repo](#hbat-software-public-repo)
  - [Main Library](#main-library)
  - [Source Files](#source-files)
  - [Note to Voxdale](#note-to-voxdale)
  - [Mapping Bezier curve to voltage readings](#mapping-bezier-curve-to-voltage-readings)
  - [Current Network Flow-Chart](#current-network-flow-chart)
  - [MQTT Broker Configuration and Connection](#mqtt-broker-configuration-and-connection)
  - [Project Configuration](#project-configuration)
  - [Important Notes](#important-notes)

Custom Software Stack files are located in `HBAT/lib` and `HBAT/HBAT_HMS_V1.0`.

main.cpp runs the control script, HMSmain library does the heavy lifting.

## Main Library

[HMSmain Library](HBAT/lib/HMSmain)

## Source Files

[HMSmain](HBAT/HBAT_HMS_V1.0)

<details>
<summary></summary>

## Note to Voxdale

For this project, we need to map the discharge curve of the system, in order to present the user with an accurate representation of charge and discharge states.
The discharge curve of our battery system is not linear, therefor a basic linear percentage calculation will not work.

Below i have one strategy, this strategy is simple to implement, however has low accuracy and could be prone to error.

Take minimum voltage reading of just before esp32 stops functioning, then
take the max voltage reading of a freshly charged battery (or input source).
The product of this subtraction is then divided by the realtime measured voltage
and multiplied by 100.

```python
def percentage_calc:
 Vmin # minimum possible voltage for the load to function
 Vmax # maximum voltage of the battery when in a fully charged state
 Vreal # real-time measured voltage for the whole system (or the sum of all series connected cells individual readings)
 Vmin - Vmax = X

 x = (X / Vreal) * 100
 return x
```

An alternative approach is to correctly map the voltage readings. Such as the example below, however this must be done in real-time to present accurate data to the user, and for the software to take correct actions based on the data of this curve.

## Mapping Bezier curve to voltage readings

According to this paper: [PEMFC Discharge Curve](https://github.com/Prometheon-Technologies/HBAT-Software/blob/main/sustainability-12-08127-v2.pdf)
the discharge curve matches a very predictable Bezier curve.

![Curve](img/curve.png)

![Curve+Math](img/curve%2Bmath.png)

Currently, i am not able to correctly implement this approach into the software stack. Is Voxdale capable of doing this? Does voxdale have an alternative approach in mind?

My current thoughts were to map the voltage readings to an array, and set that array equal to the results of a mapping function containing the formula for the curve we wish to fit our data to.

## Current Network Flow-Chart

```mermaid
graph TD;
          A(Device hosts local AP) -->|Local SSID and Pass embedded into a QR Code| B(User Scans QR Code);
          B --> C{Web interface prompts user for local SSID and Pass}-->|User enters SSID and Pass| D(User clicks connect);
          C --> G[/User does not have a local network/];
          G --> H[/User Connects to AP to interface/];
          D -->|Device writes new credentials to config file| E(config.json);
          E -->|Device Reboots from Config| F( fa:fa-power-off );
          I(fa:fa-wifi Device Connects to Wifi ) -->|Device does mDNS lookup for MQTT| J(Device connects to MQTT Broker);
          F --> I;
          J -->|User navigates to browser using mDNS| L( fa:fa-chrome );
          click B href "https://github.com/Prometheon-Technologies/HBAT-Software-Public/blob/main/HBAT/extras/HBAT_HMS-qrcode.png" "HMS QR CODE"
          click E href "https://github.com/Prometheon-Technologies/HBAT-Software-Public/blob/main/HBAT/data/config.json" "JSON Config File"
          subgraph 1;
            C;
            D;
            E;
            F;
            G;
            H;
            I;
            J;
            L;
          end;
```

## MQTT Broker Configuration and Connection

The MQTT Broker is a free service that allows you to publish and subscribe to MQTT messages.
To automate the connection process, we will use Multi-Cast DNS (mDNS/Zeroconf) to find the broker.
For this to work, you must have a broker on your local network, and you must have only **_1_** MQTT broker within the range of the client, otherwise the client will simply connect to the first broker it finds.

Using a Zeroconf approach we can avoid having to hard code the broker's IP address or hostname into the client device's firmware. Instead we can use **_DNS-SD_** and Avahi/Bonjour to discover the server hosting the MQTT broker.

To enable MQTT discovery on the broker, simply install avahi-daemon. For a Raspberry Pi, use the following command:

```bash
sudo apt-get install avahi-daemon
```

For this to work, the MQTT service needs to be advertised. On a Linux host system, Avahi can be configured to do this by including the following in /etc/avahi/services/mqtt.service:

```xml
<!DOCTYPE service-group SYSTEM "avahi-service.dtd">
<service-group>
 <name replace-wildcards="yes">MQTT on %h</name>
  <service>
   <type>_mqtt._tcp</type>
   <port>1883</port>
  </service>
</service-group>
```

[Top](#hbat-software-private-repo)

</details>

## Project Configuration

- [ ] Implement a way to read the battery's discharge curve from a file.
- [x] Implement Wifi server with fail to AP mode.
- [x] Implement Error Correct Code for sensors.
- [x] Implement I2C Scanner.
- [x] Implement Humidity and Temperature sensors.
- [x] Implement Relay Logic. //TODO: Interface with UI
- [x] Implement UI.
- [ ] Implement MQTT. //TODO: Full implementation still needs to be done.
- [x] Implement MQTT discovery.
- [x] Implement mDNS.
- [ ] Implement OTA. //TODO: Interface with UI and test current implementation.
- [x] Implement file upload.
- [x] Implement Voltage and Current Sensors.
- [ ] Implement Battery Discharge Curve.
- [ ] Implement Battery Charge Curve.
- [ ] Implement Battery Capacity measurements.
- [x] Implement config file stored in flash.

## Important Notes

> **Important**: If you receive the error:

      WebAuthentication.cpp:73: undefined reference to mbedtls_md5_starts

> Please remove the code *within* the `ifdef ESP32` block on line `72`. and paste the following:

```ino
   mbedtls_md5_init(&_ctx); mbedtls_md5_update_ret (&_ctx,data,len);
   mbedtls_md5_finish_ret(&_ctx,data);
   mbedtls_internal_md5_process( &_ctx ,data); // mbedtls_md5_starts(&_ctx); // mbedtls_md5_update(&_ctx, data, len); // mbedtls_md5_finish(&_ctx, _buf);
```

> the comments are the old-lines.
