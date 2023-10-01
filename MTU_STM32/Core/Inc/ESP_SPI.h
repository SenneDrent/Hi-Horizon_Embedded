#ifndef ESP_SPI__H_
#define ESP_SPI__H_

#include <buffer/buffer.h>
#include "DataFrame.h"
#include "SpiConfig.h"

void sendFrameToEsp(SPI_HandleTypeDef *spi, DataFrame* data) {
  uint8_t transfercode[2] = {0x02, 0x00};


  int32_t index = 0;
  uint8_t buf[32];
  for (int i = 0; i < 32; i++) {
	  buf[i] = 0;
  }
//  uint16_t millis = (uint16_t) (HAL_GetTick() && 0x0000FFFF);
  uint16_t millis = 0;
  buffer_append_uint16(buf, millis, &index);
  buffer_append_float32(buf, data->gps.lat, 100, &index);
  buffer_append_float32(buf, data->gps.lng, 100, &index);
  buffer_append_float32(buf, data->gps.speed, 100, &index);

  buffer_append_float32(buf, data->motor.battery_current, 100, &index);
  buffer_append_float32(buf, data->motor.battery_voltage, 100, &index);

  buffer_append_uint16(buf, data->mppt.power, &index);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);

  HAL_SPI_Transmit(spi, transfercode, sizeof(transfercode), 1000);
  HAL_SPI_Transmit(spi, buf, sizeof(buf), 1000);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
}

void storeResponse(DataFrame* data, uint8_t* buf, int32_t length) {
    int32_t index = 0;
    data->telemetry.espStatus          = buffer_get_uint8(buf, &index);
    data->telemetry.internetConnection = buffer_get_uint8(buf, &index);
    if (length == 6) {
        data->telemetry.NTPtime            = buffer_get_uint32(buf, &index);
    }
}

//void receiveResponseFromESP(SPI_HandleTypeDef *spi, DataFrame* data) {
//  uint8_t transfercode[2] = {0x03, 0x00};
//  HAL_SPI_Transmit(spi, transfercode, sizeof(transfercode), 1000);
//
//  if (spi->transfer(0) != SpiHeaderByte) return; //end if first byte is not the header byte
//
//  int32_t index = 0;
//  uint8_t statBuf[RESPONSE_SIZE];
//
//  while(index != RESPONSE_SIZE) {
//    uint8_t next = spi->transfer(0);
//    if (next == SpiHeaderByte) return; //wrong header byte
//    if (next == SpiTrailerByte) break; //end of message
//    if (next == SpiFlagByte) {
//        statBuf[index++] = spi->transfer(0); //frame byte as actual data
//    }
//    else {
//        statBuf[index++] = next;
//    } //generic byte
//  }
//  storeResponse(data, statBuf, index);
//}

void ESPexchangeData(SPI_HandleTypeDef *spi, DataFrame* data) {
    sendFrameToEsp(spi, data);
    HAL_Delay(10);
//    receiveResponseFromESP(spi, data);
}

#endif
