#ifndef SPICONTROL__H_
#define SPICONTROL__H_

#include "DataFrame.h"
#include "buffer.h"
#include "Arduino.h"
#include "SpiConfig.h"


uint8_t receiveSpiData(DataFrame *dataFrame, uint8_t *data) {
    int32_t index = 0;
    uint8_t staleness = buffer_get_uint16(data, &index);
    dataFrame->gps.lat = buffer_get_float32(data, 100, &index);
    dataFrame->gps.lng = buffer_get_float32(data, 100, &index);
    dataFrame->gps.speed = buffer_get_float32(data, 100, &index);

    dataFrame->motor.battery_current = buffer_get_float32(data, 100, &index);
    dataFrame->motor.battery_voltage = buffer_get_float32(data, 100, &index);

    dataFrame->mppt.power = buffer_get_uint16(data, &index);
    return staleness;
}

void append_with_stuffing(uint8_t *buffer, uint8_t byte, int32_t *index) {
    if (byte == SpiHeaderByte || byte == SpiFlagByte || byte == SpiTrailerByte) {
        buffer_append_uint8(buffer, SpiFlagByte, index);
    }
    buffer_append_uint8(buffer, byte , index);
}

int32_t generateMessageWithStuffing(uint8_t *buf, uint32_t NTPtime, bool timeSync, uint8_t status, uint8_t signal) {
    int32_t index = 0;
    buffer_append_uint8(buf, SpiHeaderByte, &index);

    append_with_stuffing(buf, status , &index);
    append_with_stuffing(buf, signal, &index);
    if (timeSync) {
        append_with_stuffing(buf, NTPtime >> 24, &index);
        append_with_stuffing(buf, NTPtime >> 16, &index);
        append_with_stuffing(buf, NTPtime >> 8, &index);
        append_with_stuffing(buf, NTPtime, &index);
    }

    buffer_append_uint8(buf, SpiTrailerByte, &index);
    return index;
}

#endif