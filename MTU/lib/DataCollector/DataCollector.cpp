#include "DataCollector.h"

long lastEspMsg = 0;

void updateTimedata(DataFrame* data, DateTime* timeNow, bool* timeSyncDone) {
    data->telemetry.unixTime         = timeNow->unixtime();
    data->telemetry.localRuntime     = millis();

    if (data->telemetry.espStatus >= 8 && data->telemetry.espStatus <= 12 && *timeSyncDone == false) {
        Teensy3Clock.set(data->telemetry.NTPtime);
        *timeSyncDone = true;
    }

    data->telemetry.MTUtemp = (float) tempmonGetTemp();
}

float traveledDistance = 0;

void updateGPSdata(DataFrame* data, HardwareSerial* uart, Adafruit_GPS* gps, HHRT_SD* sd) {
    //fetches serial data
    while(uart->available() > 0) {
        gps->read();
    }

    if (gps->newNMEAreceived()) {
      gps->parse(gps->lastNMEA());
      //checks if lock is lost
      if (data->gps.fix != 0 && gps->fixquality == 0) {
        data->telemetry.SD_error = sd->triggerErrorLog(data,"gps", 404);
      }
      //checks if fix is found
      if (data->gps.fix == 0 && gps->fixquality != 0) {
        data->telemetry.SD_error = sd->triggerErrorLog(data,"gps", 200);
      }

      data->gps.speed              = gps->speed * 1.852; // knots to kmph conversion
      data->gps.distance           = traveledDistance; //to be added / organized
      data->gps.antenna            = gps->antenna;
      data->gps.fix                = gps->fixquality;
      data->gps.lat                = gps->latitude;
      data->gps.lng                = gps->longitude;
    }
}

void updateMPPTdata(DataFrame* data, HardwareSerial* uart, VeDirectFrameHandler* mpptReader, HHRT_SD* sd) {
    //reads data from serial the mppt is connected to
    while(uart->available()) {
        int c = mpptSerial.read();
        mpptReader->rxData(c);
        Serial.print((char) c);
    }
    //checks if a error needs to be logged to sd
    if (data->mppt.error != atoi(mpptReader->veValue[MPPT_ERR])) {
        data->telemetry.SD_error = sd->triggerErrorLog(data, "mppt", (uint8_t) atoi(mpptReader->veValue[MPPT_ERR]));
    }
    //assign new values to vars
    data->mppt.voltage   = (float) atof(mpptReader->veValue[MPPT_VOLTAGE]) /1000.0;
    data->mppt.power     = atoi(mpptReader->veValue[MPPT_VPV]);
    data->mppt.current   = (float) atof(mpptReader->veValue[MPPT_CURRENT]) /1000.0;
    data->mppt.error     = (uint8_t) atoi(mpptReader->veValue[MPPT_ERR]);
    data->mppt.cs        = (uint8_t) atoi(mpptReader->veValue[MPPT_CS]);

}

float anchorLat = 69;
float anchorLon = 69;
uint16_t distance = 0;
float anchorTraveledDistance = distance;
int iterations = 0;
int coordinatesPerAnchor = 10;

// DEFUNCT - not used currently as gps lock is rare
// void updateTraveledDistance(Adafruit_GPS* gps){
//   if (gps->location.isValid()){
//     if (anchorLat == 69){
//       anchorLat = gps->location.lat();
//       anchorLon = gps->location.lng();
//     }

//     float difference = TinyGPSPlus::distanceBetween((gps->location.lat()), (gps->location.lng()), (anchorLat), (anchorLon));
//     iterations++;
//     traveledDistance = anchorTraveledDistance + difference;
//     if (iterations % coordinatesPerAnchor == 0){
//       iterations = 0;
//       anchorLat = gps->location.lat();
//       anchorLon = gps->location.lng();
//       anchorTraveledDistance = traveledDistance;
//     }
//   }
// }

float testLat = 52.971843;
float testLng = 5.439345;

//errlog wont work
void updateAllRandom(DataFrame* data) {
    testLat += 0.0000005;
    testLng += 0.000005;
    data->mppt.voltage           =   random(20,60);
    data->mppt.power             =   random(1200);
    data->mppt.current           =   random(100);
    data->mppt.error             =   random(100);
    data->mppt.cs                =   random(100);
    data->gps.speed              =   random(100);
    data->gps.distance           =   random(100);
    data->gps.lat                =   testLat;
    data->gps.lng                =   testLng;
    data->motor.battery_current  =   random(100);
    data->motor.battery_voltage  =   random(100);
}

