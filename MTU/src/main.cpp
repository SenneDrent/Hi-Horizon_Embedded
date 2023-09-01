#include "Arduino.h"
#include "SPI.h"
#include "Watchdog_t4.h"
#include "ezLED.h"

#include "SdFat.h"
#include "Adafruit_GPS.h"
#include "RTClib.h"
#include "VeDirectFrameHandler.h"

#include <buffer.h>
#include "FlexCAN_T4.h"
#include "CANparser.h"

#include "DataFrame.h"
#include "DataCollector.h"
#include "HHRT_SD.h"
#include "ESP_SPI.h"
//--PINS--
  //gps
    //tx = 14
    //rx = 15
  //mppt
    //tx = 17
    //rx = 16

//CONFIG
#define GPS Serial3
#define mpptSerial Serial8

#define DATA_OUTPUT_SPEED 1000
unsigned long timeSinceDataOutput;

WDT_T4<WDT1> wdt;
ezLED builtInLed(LED_BUILTIN);

//SD
HHRT_SD SD;

DataFrame data;

//CAN

FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can0;
CAN_message_t tm_mppt;
CAN_message_t tm_gps;
CAN_message_t tm_general;
CAN_message_t tm_strategy;
CAN_message_t tm_esp;

int test = 0;

void CAN_parser_T4(const CAN_message_t &msg) {
  uint8_t mcFailBefore = data.motor.failures;
	CAN_parseMessage(msg.id, msg.buf, &data);
  if (mcFailBefore != data.motor.failures) {
    data.telemetry.SD_error = SD.triggerErrorLog(&data,"mc", data.motor.failures);
  }
}

void CAN_messages_init();
void CAN_messages_populate();
void CAN_messages_write();

//RTC
RTC_DS1307 rtc;
DateTime TimeNow;
bool syncDone = false;

//GPS 
// TinyGPSPlus gps;
Adafruit_GPS gps(&GPS);

//ACCELEROMETER

//MPPT
VeDirectFrameHandler mpptData;

void setup() {
  WDT_timings_t config;
  wdt.begin(config);
  builtInLed.blink(500,500);

  //SD INIT
  data.telemetry.SD_error = SD.init();

  //CAN INIT
  noInterrupts();

    can0.FLEXCAN_EnterFreezeMode();
    can0.begin();
    can0.setBaudRate(125000);
    can0.setMaxMB(64);
    can0.enableMBInterrupts();
    can0.onReceive(CAN_parser_T4);
    can0.FLEXCAN_ExitFreezeMode();

  interrupts();

  //SPI
  SPI1.setCS(38);
  SPI1.setMISO(39);
  SPI1.begin();
  
  //SERIAL
  Serial.begin(9600);
  Serial6.begin(9600);
  mpptSerial.begin(19200);

  gps.begin(9600);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  gps.sendCommand(PGCMD_ANTENNA);

  CAN_messages_init();

  delay(1000);
}

void loop() {
  can0.events();
  wdt.feed();
  builtInLed.loop();
  TimeNow = Teensy3Clock.get();

  //READ DATA
  updateTimedata(&data, &TimeNow, &syncDone);
  updateGPSdata(&data, &GPS, &gps, &SD);
  updateMPPTdata(&data, &mpptSerial, &mpptData, &SD);
  
  // // -function for testing- uncomment to use
  // updateAllRandom(&data);

  if(millis() - timeSinceDataOutput > DATA_OUTPUT_SPEED) {
    data.telemetry.SD_error = SD.writeDataLogRow(&data);  //SD
    ESPexchangeData(&SPI1, &data);                        //SPI to esp
    CAN_messages_populate();                              //CANBUS
    timeSinceDataOutput = millis();
  }
}
//END LOOP

//CAN WRITING FUNCTIONS
void CAN_messages_init() {
    tm_gps.flags.extended = 0;
    tm_gps.id = 0x701;
    tm_gps.len = 6;

    tm_mppt.flags.extended = 0;
    tm_mppt.id = 0x711;
    tm_mppt.len = 8;

    tm_general.flags.extended = 0;
    tm_general.id = 0x721;
    tm_general.len = 5;

    tm_strategy.flags.extended = 0;
    tm_strategy.id = 0x741;
    tm_strategy.len = 4;

    tm_esp.flags.extended = 0;
    tm_esp.id = 0x751;
    tm_esp.len = 2;
}

void CAN_messages_populate() {
    
    noInterrupts();
    //gps values
    long ind = 0;
    buffer_append_float16(tm_gps.buf, data.gps.distance, 100, &ind);
    buffer_append_float16(tm_gps.buf,   data.gps.speed, 100, &ind);
    buffer_append_uint8(tm_gps.buf,   data.gps.fix, &ind);
    buffer_append_uint8(tm_gps.buf,   data.gps.antenna, &ind);

    can0.write(tm_gps);

    //mppt values
    ind = 0;
    buffer_append_float16(tm_mppt.buf,  data.mppt.voltage, 100, &ind);
    buffer_append_uint16(tm_mppt.buf,  data.mppt.power, &ind);
    buffer_append_float16(tm_mppt.buf,  data.mppt.current, 100, &ind);
    buffer_append_uint8(tm_mppt.buf,    data.mppt.error, &ind); 
    buffer_append_uint8(tm_mppt.buf,    data.mppt.cs, &ind); 

    can0.write(tm_mppt);

    ind = 0;
    buffer_append_uint32(tm_general.buf,    data.telemetry.unixTime,  &ind);
    buffer_append_uint8(tm_general.buf,     data.telemetry.MTUtemp,      &ind);

    can0.write(tm_general);
    interrupts();
    // TODO: 
    // ind = 0;

    // buffer_append_uint16(tm_strategy.buf,  data.telemetry.strategyRuntime, &ind);
    // buffer_append_float16(tm_strategy.buf,  data.telemetry.strategyRuntime, 100, &ind);

    // can0.write(tm_strategy);

    // ind = 0;

    // buffer_append_uint8(tm_esp.buf, data.telemetry.espStatus, &ind);
    // buffer_append_uint8(tm_esp.buf, data.telemetry.internetConnection, &ind);

    // can0.write(tm_esp);
}