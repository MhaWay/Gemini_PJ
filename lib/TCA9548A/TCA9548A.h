#ifndef _TCA9548A_H
#define _TCA9548A_H

#include <Wire.h>

class TCA9548A
{
  public:
    TCA9548A(uint8_t address); // costruttore con l'indirizzo I2C del multiplexer
    void begin();
    void selectChannel(uint8_t channel);
    void reset();

  private:
    uint8_t _address;
};

#endif
