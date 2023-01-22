/*
    SlimeVR Code is placed under the MIT license
    Copyright (c) 2022 TheDevMinerTV

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

#ifndef SLIMEVR_SENSORMANAGER
#define SLIMEVR_SENSORMANAGER

#define BNO080_I2C_ADDRESS 0x4B
#define LSM9DS1_I2C_ADDRESS 0x6B
#define SENSOR_TYPE_IMU 0x01
#define ROTATION_270 0x03

#include "globals.h"
#include "sensor.h"
#include "EmptySensor.h"
#include "logging/Logger.h"
#include "TCA9548A.h"

namespace SlimeVR
{
namespace Sensors
{
    std::vector<uint8_t> scanI2CAddress();
    class SensorManager
    {
    public:
    SensorManager()
    : m_Logger(SlimeVR::Logging::Logger("SensorManager")), m_multiplexer(new TCA9548A(0x70)) {
    for (int i = 0; i < 8; ++i) {
    m_Sensors[i] = new EmptySensor(i);
    }
}
~SensorManager()
{
    for (int i = 0; i < 8; ++i) {
    delete m_Sensors[i];
    }
    delete m_multiplexer;
}
        void setup();
        void postSetup();
        void update();

        Sensor *getSensor(int index) { return m_Sensors[index]; };

    private:
        SlimeVR::Logging::Logger m_Logger;
        TCA9548A *m_multiplexer;

        Sensor *m_Sensors[8];
    };
}

}

#endif // SLIMEVR_SENSORMANAGER_H