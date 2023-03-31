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
#include <i2cscan.h>
#include "sensorfactory.h"
#include "mpu6050sensor.h"
#include "ICM20948Sensor.h"
#include "bno080sensor.h"

SensorFactory::SensorFactory()
{
}

SensorFactory::~SensorFactory()
{
    for (int BankCount = 0; BankCount < 2; BankCount++)
    {
        for (int SensorCount = 0; SensorCount < IMUCount; SensorCount++)
        {
            delete IMUs[SensorCount + (BankCount * IMUCount)];
        }
    }
}

void SensorFactory::IMU_Int_Triggered(uint8_t IMU_ID)
{
    Serial.print(F("Int Triggerd : "));
    Serial.println(IMU_ID);
    this->IMUs[IMU_ID]->Int_Fired();
}

void SensorFactory::SetIMU(uint8_t bus)
{

    Wire.beginTransmission(0x70); // TCA9548A address is 0x70
    Wire.write(1 << bus);         // send byte to select bus
    Wire.endTransmission();
}

void SensorFactory::create(int imuIndex)
{
    Serial.println("Starting Bus Scan");

    for (int BankCount = 0; BankCount < 2; BankCount++)
    {
        this->SetIMU(imuIndex);
        I2CSCAN::DeviceParams DeviceParams = I2CSCAN::pickDevice(BankCount);

        switch (DeviceParams.DeviceID)
        {
        case MPU6050_t:
            this->IMUs[imuIndex + (BankCount * IMUCount)] = new MPU6050Sensor(DeviceParams.DeviceAddress);
            this->IMUs[imuIndex + (BankCount * IMUCount)]->Connected = true;
            Serial.println("Found MPU6050");
            break;

        case ICM_20948_t:
            this->IMUs[imuIndex + (BankCount * IMUCount)] = new ICM20948Sensor(DeviceParams.DeviceAddress);
            this->IMUs[imuIndex + (BankCount * IMUCount)]->Connected = true;
            Serial.println("Found ICM20948");
            break;

        case BNO_080_t:
            this->IMUs[imuIndex + (BankCount * IMUCount)] = new BNO080Sensor(DeviceParams.DeviceAddress);
            this->IMUs[imuIndex + (BankCount * IMUCount)]->Connected = true;
            Serial.println("Found BNO080");
            break;

        default:
            if (DeviceParams.DeviceAddress == 0x4A || DeviceParams.DeviceAddress == 0x4B) // fallback for the BNO IMU
            {
                this->IMUs[imuIndex + (BankCount * IMUCount)] = new BNO080Sensor(DeviceParams.DeviceAddress);
                this->IMUs[imuIndex + (BankCount * IMUCount)]->Connected = true;
                Serial.println("Found BNO080");
            }
            else
            {
                // Do nothing
            }
            break;
        }
        
        // Create a new Sensor object for the current IMU
        this->Sensors[imuIndex + (BankCount * IMUCount)] = new Sensor(imuIndex + (BankCount * IMUCount) + 1);
    }
}



void SensorFactory::init(int imuIndex)
{
    Serial.print("Setting up IMU Parameters for IMU ID: ");
    Serial.println(imuIndex);

    int bankCount = imuIndex / IMUCount;
    int sensorCount = imuIndex % IMUCount;

    SetIMU(sensorCount);
    if (IMUs[imuIndex]->Connected)
    {
        IMUs[imuIndex]->setupSensor(imuIndex);
        Serial.println("Setup complete");
    }
    else
    {
        Serial.println("No device");
    }
}

void SensorFactory::motionSetup(int imuIndex)
{
    Serial.print("Setting up Motion Engine for IMU ID: ");
    Serial.println(imuIndex);
    
    if (IMUs[imuIndex]->Connected)
    {
        IMUs[imuIndex]->motionSetup();
        UI::SetIMUStatus(imuIndex, IMUs[imuIndex]->isWorking() ? true : false);
        Serial.println("Complete");
    }
    else
    {
        Serial.println("No Device");
        UI::SetIMUStatus(imuIndex, false);
    }
}


void SensorFactory::motionLoop(int index) {
    this->SetIMU(index);
    if (IMUs[index]->Connected && IMUs[index]->isWorking()) {
        if (IMUs[index]->getSensorState() == SENSOR_OK) {
            IMUs[index]->motionLoop();
        }
    }
}

void SensorFactory::sendData() {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (IMUs[i]->Connected) {
            IMUs[i]->SendData();
        }
    }
}

void SensorFactory::startCalibration(int sensorId, int calibrationType)
{
    this->SetIMU(sensorId);
    if (IMUs[sensorId]->Connected && IMUs[sensorId]->isWorking())
    {
        IMUs[sensorId]->startCalibration(calibrationType);
    }
}
