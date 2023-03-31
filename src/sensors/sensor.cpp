/*
    SlimeVR Code is placed under the MIT license
    Copyright (c) 2021 Eiren Rain & SlimeVR contributors

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/
#include "sensor.h"
#include "network/network.h"
#include <i2cscan.h>
#include "calibration.h"


uint8_t Sensor::getSensorState() {
    return isWorking() ? SensorStatus::SENSOR_OK : SensorStatus::SENSOR_OFFLINE;
}

void Sensor::sendData()
{
    // Inviamo i dati attraverso l'oggetto udpClient corretto per ogni sensore
    if (this->sensorId == 1) {
        this->udpClient1.beginPacket(this->serverIP, this->serverPort);
        this->udpClient1.write(this->buffer, this->bufferLength);
        this->udpClient1.endPacket();
    } else if (this->sensorId == 2) {
        this->udpClient2.beginPacket(this->serverIP, this->serverPort);
        this->udpClient2.write(this->buffer, this->bufferLength);
        this->udpClient2.endPacket();
    } else if (this->sensorId == 3) {
        this->udpClient3.beginPacket(this->serverIP, this->serverPort);
        this->udpClient3.write(this->buffer, this->bufferLength);
        this->udpClient3.endPacket();
    } else if (this->sensorId == 4) {
        this->udpClient4.beginPacket(this->serverIP, this->serverPort);
        this->udpClient4.write(this->buffer, this->bufferLength);
        this->udpClient4.endPacket();
    } else if (this->sensorId == 5) {
        this->udpClient5.beginPacket(this->serverIP, this->serverPort);
        this->udpClient5.write(this->buffer, this->bufferLength);
        this->udpClient5.endPacket();
    } else if (this->sensorId == 6) {
        this->udpClient6.beginPacket(this->serverIP, this->serverPort);
        this->udpClient6.write(this->buffer, this->bufferLength);
        this->udpClient6.endPacket();
    } else if (this->sensorId == 7) {
        this->udpClient7.beginPacket(this->serverIP, this->serverPort);
        this->udpClient7.write(this->buffer, this->bufferLength);
        this->udpClient7.endPacket();
    } else if (this->sensorId == 8) {
        this->udpClient8.beginPacket(this->serverIP, this->serverPort);
        this->udpClient8.write(this->buffer, this->bufferLength);
        this->udpClient8.endPacket();
    }
}



void Sensor::Int_Fired() 
{
}


const char * getIMUNameByType(int imuType) {
    switch(imuType) {
        case IMU_MPU9250:
            return "MPU9250";
        case IMU_MPU6500:
            return "MPU6500";
        case IMU_BNO080:
            return "BNO080";
        case IMU_BNO085:
            return "BNO085";
        case IMU_BNO055:
            return "BNO055";
        case IMU_MPU6050:
            return "MPU6050";
        case IMU_BNO086:
            return "BNO086";
    }
    return "Unknown";
}
