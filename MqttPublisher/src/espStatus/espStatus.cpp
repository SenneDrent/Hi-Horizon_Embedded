#include "espStatus.h"

//gets status from wifi-library
void espStatus::updateStatus(wl_status_t wifi_status) {
    switch (wifi_status) {
        case WL_IDLE_STATUS:    
            break;
        case WL_NO_SSID_AVAIL:
        case WL_WRONG_PASSWORD:
            status = WIFI_LOGIN_WRONG_PASSWORD;
            break;
        case WL_CONNECTED:
            status = WIFI_CONNECTED;
            break;
        case WL_CONNECT_FAILED:
            status = WIFI_LOGIN_FAILED;
            break;
        case WL_CONNECTION_LOST:
            status = NO_SIGNAL_FAULT;
            connectionStrength = 0;
            break;
        case WL_DISCONNECTED:
            status = WIFI_DISCONNECTED;
            connectionStrength = 0;
            break;
    }
    // for debugging purposes
        Serial.println(status);
    //
    // sendStatusSerial();
}

//updates specified status
void espStatus::updateStatus(uint8_t newStatus) {
    status = newStatus;
    // for debugging purposes
        // Serial.println(status);
    //
    // sendStatusSerial();
}

//converts the rssi strength into a 1 to 3 strength
void espStatus::updateConnectionStrength(int32_t newConnectionStrength) {
    if (newConnectionStrength < -90) connectionStrength = 0;
    else if (newConnectionStrength < -80) connectionStrength = 1;
    else if (newConnectionStrength < -70) connectionStrength = 2;
    else if (newConnectionStrength < -67) connectionStrength = 3;
    else if (newConnectionStrength < -30) connectionStrength = 4;
    else connectionStrength = 5;
    // for debugging purposes
        // Serial.println(connectionStrength);
    //
    // sendStatusSerial();
}


//sends serialmessage to other microcontroller detailing the status of the espdevice
// void espStatus::sendStatusSerial() {
//     Wire.beginTransmission(4);
//     Wire.write(this->status);
//     Wire.write(this->connectionStrength);
//     Wire.endTransmission();
// }

//
uint8_t espStatus::getStatus() {
    return this->status;
}

//
uint8_t espStatus::getConnectionStrength() {
    return this->connectionStrength;
}