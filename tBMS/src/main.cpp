#include <Arduino.h>
#include <CAN_HHRT.h>
#include <tinyBMS.h>
#include <Watchdog_t4.h>

#define LOG     1

// Temperature
#define ADC_PIN A7
#define SELECT0 2
#define SELECT1 3
#define SELECT2 4
#define CELLS_TEMP_SENSOR 2
#define ENV_TEMP_SENSOR 4

// Settings for temperature sensor
#define Beta    3950
#define To      298.15
#define Rs      10000
#define Vcc     5
#define ADC_VOLTAGE   3.3
#define ADC_RES       1023

// Relays
#define RELAY1  14
#define RELAY2  15
#define RELAY3  16
#define RELAY4  17

#define FAN_L   5
#define FAN_R   6

CAN_HHRT can;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

#define CAN_INTERVAL 1000

#define BMS_SERIAL Serial2

tinyBMSdevice tinyBMS;

// Watchdog timer
WDT_T4<WDT1> wdt;

CAN_message_t bms_temp;
CAN_message_t bms_status;
CAN_message_t bms_main_data;

unsigned long timeCAN;
unsigned long logTime;
unsigned long LEDTIME;

double temp[8];
bool highTemp = false;

bool ledState = false;

// Functions
void initCANmessages(void);
void populateCAN(void);
double readTemp(int channel);
extern float tempmonGetTemp(void);

void setup() {
  // Watchdog settings
  WDT_timings_t config;
  config.timeout = 5;
  wdt.begin(config);

  BMS_SERIAL.begin(115200);
  Serial.begin(1);

  // CAN bus setup
  noInterrupts();

    Can1.begin();  
    Can1.setBaudRate(125000);
    Can1.setMaxMB(64);
    Can1.enableMBInterrupts();
    Can1.attachObj(&can);
    can.attachGeneralHandler();

  interrupts();

  initCANmessages();

  // Relay setup
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // Temperature setup
  pinMode(ADC_PIN, INPUT);
  pinMode(SELECT0, OUTPUT);
  pinMode(SELECT1, OUTPUT);
  pinMode(SELECT2, OUTPUT);

  // Fan setup
  pinMode(FAN_L, OUTPUT);
  pinMode(FAN_R, OUTPUT);

  digitalWrite(FAN_L, HIGH);
  digitalWrite(FAN_R, HIGH);

  timeCAN = logTime = LEDTIME = millis();

  digitalWrite(RELAY1, LOW);

  pinMode(LED_BUILTIN, OUTPUT);
}

void printFrame(const CAN_message_t frame)
{
    Serial.print("MB "); Serial.print(frame.mb);
    Serial.print("  LEN: "); Serial.print(frame.len);
    Serial.print(" ID: "); Serial.print(frame.id, HEX);
    Serial.print(" Buffer: ");
    for ( uint8_t i = 0; i < frame.len; i++ ) {
      Serial.print(frame.buf[i], HEX); Serial.print(" ");
    } Serial.println();
}

void loop() {

  while(BMS_SERIAL.available()) 
    tinyBMS.encodeTinyBMSSerial(BMS_SERIAL.read());

  // Measuring temperatures
  for (int i = 0; i < 8; i++) temp[i] = readTemp(i);

  // Pump settings

  // digitalWriteFast(RELAY1, LOW);

  if(can.screen.pump_on) digitalWriteFast(RELAY1, LOW);
  else digitalWriteFast(RELAY1, HIGH);

  // if(can.screen.pump_auto) digitalWriteFast(RELAY2, LOW);
  // else digitalWriteFast(RELAY2, HIGH);

  // Fans turn on/off
  // if(can.screen.fans || highTemp)
  // {
  //   digitalWriteFast(FAN_L, HIGH);
  //   digitalWriteFast(FAN_R, HIGH);
  // }
  // else
  // {
  //   digitalWriteFast(FAN_L, LOW);
  //   digitalWriteFast(FAN_R, LOW);
  // }

  if(millis() - timeCAN > CAN_INTERVAL)
  {
    populateCAN();
    Serial.println();
    printFrame(bms_status);
    
    printFrame(bms_main_data);
    
    printFrame(bms_temp);
    
    Serial.println("CAN SEND");
    timeCAN = millis();
  }

  // Refreshing watchdog
  wdt.feed();

  // LED because having LEDs improves performance (scientifically proven!)
  if(millis() - LEDTIME > 500) {
    ledState = !ledState; 
    digitalWrite(LED_BUILTIN, ledState);
    LEDTIME = millis();
  }

  #if LOG
  if(millis() - logTime > 1000)
  {
    Serial.println("TEMPERATURES:");
    for(int i = 0; i < 8; i++)
    {
      Serial.print("\ttemp"); 
      Serial.print(i); 
      Serial.print(" "); 
      Serial.println(temp[i]);
    }

    Serial.println("RELAYS");
    Serial.print("\t"); 
    Serial.print(can.screen.pump_on); 
    Serial.print(" ");
    Serial.println(can.screen.pump_auto);

    Serial.print("FANS: ");
    Serial.print("\t"); 
    Serial.print(can.screen.fans); 
    Serial.print(" ");
    Serial.println(highTemp);

    logTime = millis();
  }
  #endif

}

void initCANmessages(void) 
{
  bms_status.flags.extended = 0;
  bms_status.id = 0x601;
  bms_status.len = 7;

  bms_main_data.flags.extended = 0;
  bms_main_data.id = 0x611;
  bms_main_data.len = 8;

  bms_temp.flags.extended = 0;
  bms_temp.id = 0x621;
  bms_temp.len = 4;
}

void populateCAN(void)
{
  Can1.mailboxStatus();

  long index = 0;

  buffer_append_float8(bms_temp.buf, (float) temp[CELLS_TEMP_SENSOR], 1, &index);
  buffer_append_float8(bms_temp.buf, (float) temp[ENV_TEMP_SENSOR], 1, &index);
  buffer_append_float8(bms_temp.buf, tempmonGetTemp(), 1, &index);
  buffer_append_float8(bms_temp.buf, tinyBMS.data.BMST, 1, &index);

  Can1.write(bms_temp);

  index = 0;

  uint8_t status =  tinyBMS.data.status.OS 
                    + 2*tinyBMS.data.status.LSS 
                    + 4*tinyBMS.data.status.CSS;

  buffer_append_uint8(bms_status.buf, status, &index);
  buffer_append_uint16(bms_status.buf, 0, &index);
  buffer_append_float16(bms_status.buf, tinyBMS.data.MINCV, 10, &index);
  buffer_append_float16(bms_status.buf, tinyBMS.data.MAXCV, 10, &index);

  Can1.write(bms_status);

  index = 0;

  buffer_append_float16(bms_main_data.buf, tinyBMS.data.BPV, 100, &index);
  buffer_append_float16(bms_main_data.buf, tinyBMS.data.BPC, 100, &index);
  buffer_append_float16(bms_main_data.buf, 1.0, 100, &index);
  buffer_append_float16(bms_main_data.buf, 2.0, 100, &index);

  Can1.write(bms_main_data);
}

double readTemp(int channel)
{
  switch(channel)
  {
    case ENV_TEMP_SENSOR:
      digitalWrite(SELECT0, LOW);
      digitalWrite(SELECT1, LOW);
      digitalWrite(SELECT2, HIGH);
      break;
    case CELLS_TEMP_SENSOR:
      digitalWrite(SELECT0, LOW);
      digitalWrite(SELECT1, HIGH);
      digitalWrite(SELECT2, LOW);
      break;
    default:
      return 0.0;
  }

  int data = analogRead(ADC_PIN);
  double Vin = data * ADC_VOLTAGE / ADC_RES;
  double Rsensor = (Vin * Rs) / (Vcc - Vin);

  return (1 / ((1 / To) + (1/(Beta * log(Rsensor))))) - 273.15;
}