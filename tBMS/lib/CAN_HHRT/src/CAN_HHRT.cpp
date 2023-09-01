#include "CAN_HHRT.h"

long ind = 0;

CAN_HHRT::CAN_HHRT()
{
    this->bms.balancing = new bool[16];
    this->bms.status.OS = false;
    this->bms.status.LSS = false;
    this->bms.status.CSS = false;

    this->screen.pump_on = false;
    this->screen.pump_auto = false;
    this->screen.fans = false;

    for (int i = 0; i < MAX_NUM_NODES; i++)
        this->received_time[i] = 0;
}

void CAN_HHRT::generate_bit_list(int len, unsigned long data, bool* return_array)
{
    for(int i = 0; i < len; i++) return_array[i] = (bool) (data & (((unsigned int) pow(2,len-1)) >> i));
}

void CAN_HHRT::receive_Message(const CAN_message_t &frame)
{
    this->received_time[frame.id & MAX_NUM_NODES] = millis();

    switch (frame.id)
    {

        case 0x601:
            {
                ind = 0;
                uint8_t status = buffer_get_uint8(frame.buf, &ind);
                bool status_array[3]; generate_bit_list(3, status, status_array);
                this->bms.status.OS = status_array[0];
                this->bms.status.LSS = status_array[1];
                this->bms.status.CSS = status_array[2];
                uint16_t cells_balancing = buffer_get_uint16(frame.buf, &ind);
                generate_bit_list(16, cells_balancing, this->bms.balancing);
                this->bms.min_cel_voltage = buffer_get_float16(frame.buf, 100, &ind);
                this->bms.max_cel_voltage = buffer_get_float16(frame.buf, 100, &ind);
                break;
            }

        case 0x611:
            {
                ind = 0;
                this->bms.battery_voltage = buffer_get_float16(frame.buf, 500, &ind);
                this->bms.battery_current = buffer_get_float16(frame.buf, 100, &ind);
                this->bms.bms_temp = buffer_get_float16(frame.buf, 100, &ind);
                this->bms.SOC = buffer_get_float16(frame.buf, 100, &ind);
                break;
            }

        // Motorcontroller

        case 0x14A10001:
            {
                this->motor.battery_voltage = ((frame.buf[0]) + 256*(frame.buf[1]))/57.45;
                this->motor.battery_current = ((frame.buf[2]) + 256*(frame.buf[3]))/10;
                this->motor.motor_rpm = ((frame.buf[4]) + 256*(frame.buf[5]) + 65536*(frame.buf[6]))*10;
                break;
            }

        case 0x14A10002:
            {
                this->motor.odometer = ((frame.buf[0]) + 256*(frame.buf[1]) + 65536*(frame.buf[2]) + 16777216*(frame.buf[3]));
                this->motor.controller_temp = frame.buf[4];
                this->motor.motor_temp = frame.buf[5];
                this->motor.battery_temp = frame.buf[6];
                break;
            }

        case 0x14A10003:
            {
                this->motor.req_power = ((frame.buf[0]) + 256*(frame.buf[1]))/10;
                this->motor.power_out = ((frame.buf[2]) + 256*(frame.buf[3]))/10;
                this->motor.warning = frame.buf[4] + 256*(frame.buf[5]);
                this->motor.failures = frame.buf[6] + 256*(frame.buf[7]);
                break;
            }

        case 0x14A10004:
            {
                this->motor.capacity = frame.buf[0];
                this->motor.term_voltage = ((frame.buf[6]) + 256*(frame.buf[7]))/10;
                break;
            }

        // Telemetry data

        case 0x701:
            {
                ind = 0;
                this->gps.distance = buffer_get_float16(frame.buf, 100, &ind);
                this->gps.time.hour = buffer_get_uint8(frame.buf, &ind);
                this->gps.time.minutes = buffer_get_uint8(frame.buf, &ind);
                this->gps.time.seconds = buffer_get_uint8(frame.buf, &ind);
                this->gps.speed = buffer_get_uint8(frame.buf, &ind);
                this->gps.satellites_count = buffer_get_uint8(frame.buf, &ind);
                this->gps.altitude = buffer_get_uint8(frame.buf, &ind);
                break;
            }
        
        case 0x711:
            {
                ind = 0;
                this->mppt.voltage = buffer_get_float16(frame.buf, 100, &ind);
                this->mppt.power = buffer_get_float16(frame.buf, 100, &ind);
                this->mppt.current = buffer_get_float16(frame.buf, 100, &ind);
                this->mppt.error = buffer_get_uint8(frame.buf, &ind);
                this->mppt.cs = buffer_get_uint8(frame.buf, &ind);
                break;
            }

        case 0x721:
            {
                ind = 0;
                this->telemetry.temp = buffer_get_uint8(frame.buf, &ind);
                uint8_t status = buffer_get_uint8(frame.buf, &ind);
                bool status_array[8]; generate_bit_list(8, status, status_array);
                this->telemetry.SD_status = status_array[0];
                this->telemetry.GPRS_status = status_array[1];
                this->telemetry.SD_runtime = buffer_get_uint16(frame.buf, &ind);
                break;
            }

        case 0x731:
            {
                ind = 0;
                this->screen.Pmotor = buffer_get_float16(frame.buf, 100, &ind);
                uint8_t status = buffer_get_uint8(frame.buf, &ind);
                Serial.println(status);
                bool status_array[8]; generate_bit_list(8, status, status_array);
                this->screen.SD_on = status_array[7];
                this->screen.SD_record = status_array[6];
                this->screen.GPRS_send = status_array[5];
                this->screen.fans = status_array[3];
                this->screen.pump_on = status_array[2];
                this->screen.pump_auto = status_array[1];
                this->screen.strategy_runtime = buffer_get_uint16(frame.buf, &ind);
                this->screen.temp = buffer_get_uint8(frame.buf, &ind);
                break;
            }

        default:
            {
            break;
            }
    }
}

void CAN_HHRT::printFrame(const CAN_message_t &frame)
{
    Serial.print("MB "); Serial.print(frame.mb);
    Serial.print("  LEN: "); Serial.print(frame.len);
    Serial.print(" ID: "); Serial.print(frame.id, HEX);
    Serial.print(" Buffer: ");
    for ( uint8_t i = 0; i < frame.len; i++ ) {
      Serial.print(frame.buf[i], HEX); Serial.print(" ");
    } Serial.println();
}

bool CAN_HHRT::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
    printFrame(frame);
    receive_Message(frame);
    return true;
}

bool CAN_HHRT::check_valid_data()
{
    bool reset = false;

	for (int i = 0; i < MAX_NUM_NODES; i++)
	{	
		// if (i == CAN_ID)
		// 	continue;		
		if ((millis() - this->received_time[i] > 2000))
        {
            reset_data(i);
            reset = true;
        }
	}

    return reset;
}

// TODO: uitvinden hoe die source_id werkt en de functie kloppend maken

void CAN_HHRT::reset_data(uint8_t source_id)
{
    switch ( source_id )
    {
    case 0x01:
        break;

    case 0x02:
        break;
    
    default:
        break;
    }
}
