/*
 * sensorParser.c
 *
 *  Created on: Sep 28, 2023
 *      Author: senne
 */


#include "sensorParser.h"

void parseNMEA(DataFrame* data, uint8_t* buf, uint16_t size) {

}

bool calculateCheskumMPPT(uint8_t* buf, uint16_t size) {
	int checksum = 0;
	for (int i = 0; i < size; i++) {
	   checksum = (checksum + buf[i]) & 255; /* Take modulo 256 in account */
	}

	if (checksum == 0) {
		return true;
	} else {
		return false;
	}
}

void parseMPPT(DataFrame* data, uint8_t* buf, uint16_t size) {
	bool tagPart = false;
	bool valuePart = false;

	char tags[MPPT_VALUEARRAY_SIZE][MPPT_TAG_LEN] = {};
	char values[MPPT_VALUEARRAY_SIZE][MPPT_VALUE_LEN] = {};
	uint8_t tagLength = 0;
	uint8_t valueLength = 0;
	uint8_t variableIndex = 0;

	for (int i = 0; i < size; i++) {
		char c = buf[i];
		switch(c) {
			case '\r':
				if (!tagPart && !valuePart) break;
				tagPart = false;
				valuePart = false;
				tagLength = 0;
				valueLength = 0;
				variableIndex++;
				break;
			case '\n':
				valuePart = false;
				tagPart = true;
				break;
			case '\t':
				tagPart = false;
				valuePart = true;
				break;
			default:
				if (tagPart) {
					tags[variableIndex][tagLength] = c;
					tagLength++;
				}
				else if (valuePart) {
					values[variableIndex][valueLength] = c;
					valueLength++;
				}
		}
	}
//	if (calculateCheskumMPPT(buf, size)) { //checksum doesnt work yet
		for (int i = 0; i < MPPT_VALUEARRAY_SIZE; i++) {
			if (strcmp(tags[i], "P") == 0) {
				data->mppt.power = atoi(values[i]);
			}
			else if (strcmp(tags[i], "V") == 0) {
				data->mppt.voltage = atof(values[i]) / 1000.0;
			}
			else if (strcmp(tags[i], "ERR") == 0) {
				data->mppt.error = atoi(values[i]);
			}
			else if (strcmp(tags[i], "CS") == 0) {
				data->mppt.cs = atoi(values[i]);
			}
		}
//	}
}
