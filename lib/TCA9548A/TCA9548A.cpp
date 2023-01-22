#include "TCA9548A.h"

TCA9548A::TCA9548A(uint8_t address)
{
_address = address;
}

void TCA9548A::begin()
{
Wire.begin();
}

void TCA9548A::selectChannel(uint8_t channel)
{
Wire.beginTransmission(_address);
Wire.write(1 << channel);
Wire.endTransmission();
}

void TCA9548A::reset()
{
Wire.beginTransmission(_address);
Wire.write(0);
Wire.endTransmission();
}