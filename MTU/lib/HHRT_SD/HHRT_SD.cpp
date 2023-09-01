#include "HHRT_SD.h"

SdFs sd;
FsFile file;

uint8_t HHRT_SD::init() {
  if(!sd.begin(SD_CONFIG)) {
    Serial.println("failed to initialize sd card");
    return sd.sdErrorCode();
  }

  writeHeader(generalLogFileName, generalColumnTitles);
  writeHeader(errorFileName, errorColumnTitles);

  return sd.sdErrorCode();
}

uint8_t HHRT_SD::writeHeader(char* fileName, String columnTitles) {
  if(!file.open(fileName, O_RDWR | O_CREAT | O_AT_END)){
    return sd.sdErrorCode();
  }
  file.write("\n");
  file.println(columnTitles);
  file.close();
  return sd.sdErrorCode();
}

uint8_t HHRT_SD::writeDataLogRow(DataFrame* data) {
    if(!file.open(this->generalLogFileName, O_WRITE | O_CREAT | O_AT_END)){
      return sd.sdErrorCode();
    }

    file.print(data->telemetry.unixTime);
    file.print(',');
    file.print(data->telemetry.localRuntime);
    file.print(',');
    file.print(data->mppt.power);
    file.print(',');
    file.print(data->mppt.voltage);
    file.print(',');
    file.print(data->gps.distance);
    file.print(',');
    file.print(data->gps.speed);
    file.print(',');
    file.print(data->motor.battery_voltage);
    file.print(',');
    file.print(data->motor.battery_current);
    file.print(',');
    file.print((data->motor.battery_current*data->motor.battery_voltage));
    file.print(',');
    file.print(data->motor.motor_temp);
    file.print(',');
    file.print(data->motor.rpm);
    file.print(',');
    file.print(data->bms.cells_temp);
    file.print(',');
    file.print(data->bms.env_temp);
    file.println(',');

    file.sync();
    file.close();
    return sd.sdErrorCode();
}

uint8_t HHRT_SD::triggerErrorLog(DataFrame* data, String type, int code) {
    if(!file.open(this->errorFileName, O_WRITE | O_CREAT | O_AT_END)){
      return sd.sdErrorCode();
    }

    file.print(type);
    file.print(",");
    file.print(code);
    file.print(",");
    file.print(data->telemetry.unixTime);

    file.close();
    return sd.sdErrorCode();
}

//doesn't work atm, but it's a cool idea
// bool HHRT_SD::writeDataLogRowBinary(DataFrame* data) {
//     buffer_append_uint32(writeBuffer,data->telemetry.unixTime, &index);
//     buffer_append_uint64(writeBuffer,data->telemetry.localRuntime, &index);
//     buffer_append_uint16(writeBuffer,data->mppt.power, &index);
//     buffer_append_float32(writeBuffer,data->mppt.voltage, 100, &index);
//     buffer_append_float32(writeBuffer,data->gps.distance, 100, &index);
//     buffer_append_float32(writeBuffer,data->gps.speed, 100, &index);
//     buffer_append_float32(writeBuffer,data->motor.battery_voltage, 100, &index);
//     buffer_append_float32(writeBuffer,data->motor.battery_current, 100, &index);
//     buffer_append_float32(writeBuffer,(data->motor.battery_current*data->motor.battery_voltage), 100, &index);
//     buffer_append_int32(writeBuffer, data->motor.motor_temp, &index);
//     buffer_append_float32(writeBuffer, data->motor.rpm, 100, &index);
//     buffer_append_float32(writeBuffer, data->bms.cells_temp, 100, &index);
//     buffer_append_float32(writeBuffer, data->bms.env_temp, 100, &index);

//     if (index + 54 >= 511) { //if another dataLine doesn't fit, send the current buffer
//       if(!file.open(this->generalLogFileName, O_WRITE | O_CREAT | O_AT_END)){
//         index = 0;
//         return false;
//       }
//       file.write(writeBuffer, index);
//       index = 0;
//       file.sync();
//       file.close();
//     }
    
//     return true;
// }
