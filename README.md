# Hi-Horizon_Embedded
Embedded code for Hi-Horizon racing team.

## Setting up ESP wifi
Since I don't want to expose passwords on the repository, the wifi configuration file has been intentionally left out.
In order to make the code for the ESP work, use the following steps:
- Add a file named wifiConfig.h in MqttPublisher/include folder, this file is going to contain the information
- In this file, paste the following template:
```
#ifndef wifiConfig_h
#define wifiConfig_h

//wifi name
#define Wifi_SSID "user"
//wifi password
#define Wifi_PASSWORD "pwrd"

//MQTT username
#define MQTT_USER "user"
//MQTT password
#define MQTT_PWD "pwrd"
//Link to MQTT server
const char* mqtt_server = "example.com";

#endif
```
- Upload the code to the ESP and check if everything works.
