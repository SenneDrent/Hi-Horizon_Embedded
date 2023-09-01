#ifndef ESP_SPI__H_
#define ESP_SPI__H_

#include "DataFrame.h"
#include "buffer.h"
#include "Arduino.h"
#include "SpiConfig.h"
#include "SPI.h"

void SPITransferArray(SPIClass *spi, byte *arr, int len) {
  for (int i = 0; i < len; i++) {spi->transfer(arr[i]);}
  for (int i = 0; i < 32 - len; i++) {spi->transfer(0);} //send empty data to fill out 
}

void sendFrameToEsp(SPIClass *spi, DataFrame* data) {
  spi->transfer(0x02);
  spi->transfer(0x00);

  int32_t index = 0;
  uint8_t buf[32];
  buffer_append_uint16(buf, millis(), &index);
  buffer_append_float32(buf, data->gps.lat, 100, &index);
  buffer_append_float32(buf, data->gps.lng, 100, &index);
  buffer_append_float32(buf, data->gps.speed, 100, &index);

  buffer_append_float32(buf, data->motor.battery_current, 100, &index);
  buffer_append_float32(buf, data->motor.battery_voltage, 100, &index);

  buffer_append_uint16(buf, data->mppt.power, &index);
  SPITransferArray(spi, buf, index);
}

void storeResponse(DataFrame* data, uint8_t* buf, int32_t length) {
    int32_t index = 0;
    data->telemetry.espStatus          = buffer_get_uint8(buf, &index);
    data->telemetry.internetConnection = buffer_get_uint8(buf, &index);
    if (length == 6) {
        data->telemetry.NTPtime            = buffer_get_uint32(buf, &index);
    }
}

void receiveResponseFromESP(SPIClass *spi, DataFrame* data) {
  spi->transfer(0x03);
  spi->transfer(0x00);

  if (spi->transfer(0) != SpiHeaderByte) return; //end if first byte is not the header byte

  int32_t index = 0;
  uint8_t statBuf[RESPONSE_SIZE];

  while(index != RESPONSE_SIZE) { 
    uint8_t next = spi->transfer(0);
    if (next == SpiHeaderByte) return; //wrong header byte
    if (next == SpiTrailerByte) break; //end of message
    if (next == SpiFlagByte) {
        statBuf[index++] = spi->transfer(0); //frame byte as actual data
    }
    else {
        statBuf[index++] = next;
    } //generic byte
  }
  storeResponse(data, statBuf, index);
}

void ESPexchangeData(SPIClass *spi, DataFrame* data) {
    sendFrameToEsp(spi, data);
    delay(10);
    receiveResponseFromESP(spi, data);
}

#endif