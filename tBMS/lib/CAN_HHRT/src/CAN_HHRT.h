
#ifndef CAN_HHRT_h
#define CAN_HHRT_h

#if defined(__IMXRT1062__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#include "../utility/FlexCAN_T4.h"
#include <TimeLib.h>
#else
#error "Only works for teensies."
#endif

//#include "../utility/ArduinoJson.h"
#include "buffer.h"

#define MAX_NUM_NODES 15

class BMS_data
{
    public:
        float battery_voltage;
        float battery_current;
        float bms_temp;
        float SOC;

        bool *balancing;
        struct STATUS {
            bool OS;      // BMS online status
            bool LSS;     // Load switch status (0 - OFF, 1 - ON)
            bool CSS;     // Charger switch status (0 - OFF, 1 - ON)
        } status;

        float min_cel_voltage;
        float max_cel_voltage;
};

class Motorcontroller_data
{
    public:
        float battery_voltage;
        float battery_current;
        float motor_rpm;

        long odometer;
        int controller_temp;
        int motor_temp;
        int battery_temp;

        float req_power;
        float power_out;

        int warning;
        int failures;

        float capacity;
        float term_voltage;
};

class MPPT_data
{
    public:
        float voltage;
        float power;
        float current;

        int error;
        int cs;
};

class Screen_data
{
    public:
        float Pmotor;

        int strategy_runtime;
        uint8_t temp;

        bool pump_on;
        bool pump_auto;
        bool fans;
        bool SD_on;
        bool SD_record;
        bool GPRS_send;
};

class Telemetry_data
{
    public:
        uint8_t temp;

        int SD_runtime;

        bool SD_status;
        bool GPRS_status;
};

class GPS_data
{
    public:
        float speed;
        float distance;

        uint8_t altitude; 
        uint8_t satellites_count;

        struct time_data
        {
            uint8_t hour;
            uint8_t minutes;
            uint8_t seconds;
        } time;

        
};

class CAN_HHRT : public CANListener
{
    private:
        bool receivedTime = false;
        unsigned long received_time[MAX_NUM_NODES];

        void generate_bit_list(int len, unsigned long data, bool* return_array);

    public:
        int *filters; // Als we later bepaalde berichten willen filteren
        CAN_message_t msg; // CAN bericht structure
        BMS_data bms;
        Motorcontroller_data motor;
        Screen_data screen;
        Telemetry_data telemetry;
        MPPT_data mppt;
        GPS_data gps;

        CAN_HHRT();
        void receive_Message(const CAN_message_t &frame);
        void printFrame(const CAN_message_t &msg);
        bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller);
        bool check_valid_data();
        void reset_data(uint8_t source_id);
};

#endif