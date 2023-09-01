#ifndef DATACOLLECTOR_h
#define DATACOLLECTOR_h

#include "Arduino.h"
#include "DataFrame.h"
#include "Adafruit_GPS.h"
#include "VeDirectFrameHandler.h"
#include "RTClib.h"
#include "HHRT_SD.h"

#define GPS Serial3
#define mpptSerial Serial8
#define espSerial Serial2
#define espI2C Wire2

void updateTimedata(DataFrame* data, DateTime* timeNow, bool* timeSyncDone);
void updateGPSdata(DataFrame* data, HardwareSerial* uart, Adafruit_GPS* gps, HHRT_SD* sd);
void updateMPPTdata(DataFrame* data, HardwareSerial* uart, VeDirectFrameHandler* mpptReader, HHRT_SD* sd);
void updateTraveledDistance(Adafruit_GPS* gps);
void updateEspStatus(int bytes);
void serialSend();
void updateAllRandom(DataFrame* data);

#endif