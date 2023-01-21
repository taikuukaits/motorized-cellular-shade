# motorized-cellular-shade
DIY motorized cellular shades! 
TBD

# Getting Started

Requires an Installation of Home Assistant with the MQTT Integration
    The Home Assistant MQTT connection is what should be used in secrets.sh

(Assumes you are using ESP8266 D1 Mini as the board)
Install the Arduino IDE
Install the CH340 Driver: https://www.wemos.cc/en/latest/ch340_driver.html
Install the ESP8266 Board Manager: https://create.arduino.cc/projecthub/electropeak/getting-started-w-nodemcu-esp8266-on-arduino-ide-28184f
    Select "LOLIN(WEMOS) D1 R2 & mini"
Install Libraries: 
    PubSubClient 2.8.0 by Nick O'Leary
    ArduinoJson 6.19.4 by Benoit Blanchon
    EEPRPOM_Rotate (for ESP8266) 0.9.2 by Xose Perez

Create your secrets file:
    secrets.h
        #define WIFI_SSID ""
        #define WIFI_PASS ""
        #define MQTT_SERVER ""
        #define MQTT_PORT 1883
        #define MQTT_USER ""
        #define MQTT_PASS ""

Modify the device file your shade,
    Device Id / Device Name
    Pins

### Shade Boss
Install shade-boss dependencies: npm install
Create secrets file for shade-boss: secrets.json


## Additional Configuration

Additional configuration options are in config.sh, though generally if you change these settings you should change them across all shades. For example changing the '__announce__' payload could cause problems.  
