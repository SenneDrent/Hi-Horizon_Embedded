# Hi-Horizon_Embedded
this repository contains all the code used by the microcontroller on board of the raceboat. Its purpose is to collect data of power consumption and gain, speed and status of the boat. And display this on a screen and website through MQTT protocol.

## Setup
Download the files or start a local branch on your PC. Except for the commons folder, every folder is a platformIO project except for interface (to be added), which is an STM32CubeIDE Project.

### MqttPublisher Wifi
Since I don't want to expose passwords on the repository, the Wifi configuration file has been intentionally left out.
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
