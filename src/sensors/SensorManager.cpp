#include "SensorManager.h"
#include <i2cscan.h>
#include "network/network.h"
#include "bno055sensor.h"
#include "bno080sensor.h"
#include "mpu9250sensor.h"
#include "mpu6050sensor.h"
#include "bmi160sensor.h"
#include "icm20948sensor.h"
#include "ErroneousSensor.h"
#include "TCA9548A.h"

#include "SensorManager.h"
#include "SpecificSensor.h"
#include "EmptySensor.h"

namespace SlimeVR {
    namespace Sensors {
        void SensorManager::setup() {
            std::vector<uint8_t> addresses = scanI2CAddresses();
            m_multiplexer->begin();
            uint8_t address;
            for (int channel = 0; channel < 8; ++channel) {
                m_multiplexer->selectChannel(channel);
                if (channel < addresses.size()) {
                    address = addresses[channel];
                    switch(address) {
                        case BNO080_I2C_ADDRESS:
                            m_Sensors[channel] = new BNO080Sensor(channel, SENSOR_TYPE_IMU, address, ROTATION_270, PIN_IMU_INT);
                            break;
                        case LSM9DS1_I2C_ADDRESS:
                            m_Sensors[channel] = new LSM9DS1Sensor(channel, address, ROTATION_270);
                            break;
                        default:
                            m_Sensors[channel] = new ErroneousSensor(channel, address);
                            break;
                    }
                    m_Sensors[channel]->setup();
                } else {
                    m_Sensors[channel] = new EmptySensor(channel);
                }
            }
        }


        void SensorManager::postSetup() {
            for (int i = 0; i < 8; ++i) {
                if (m_Sensors[i] != nullptr) {
                    m_Sensors[i]->postSetup();
                }
            }
        }

        void SensorManager::update() {
            for (int i = 0; i < 8; i++) {
                if (m_Sensors[i] != nullptr) {
                    m_Sensors[i]->update();
                }
            }
        }
    }
}
