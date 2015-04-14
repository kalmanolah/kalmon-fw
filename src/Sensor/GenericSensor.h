#ifndef generic_sensor_h
#define generic_sensor_h

#include "../ArduinoHeader.h"

class GenericSensor {
    protected:
        uint8_t input_pin;

    public:
        GenericSensor(uint8_t input_pin): input_pin(input_pin) {};
};

#endif
