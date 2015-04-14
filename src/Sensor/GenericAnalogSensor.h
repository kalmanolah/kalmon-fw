#ifndef generic_analog_sensor_h
#define generic_analog_sensor_h

#include "GenericSensor.h"

class GenericAnalogSensor: public GenericSensor {
    protected:
        uint16_t level = 0;

    public:
        GenericAnalogSensor(uint8_t input_pin): GenericSensor(input_pin) {};

        void read();

        inline uint16_t getLevel() const {
            return this->level;
        };
};

#endif
