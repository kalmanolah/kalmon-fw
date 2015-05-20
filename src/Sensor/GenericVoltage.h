/**
 * Generic voltage sensor.
 */

#ifndef generic_voltage_h
#define generic_voltage_h

#include "GenericAnalogSensor.h"

/*
 * GenericVoltage
 *
 * A class that is in charge of managing a relatively generic voltage sensor.
 */
class GenericVoltage: public GenericAnalogSensor {
    protected:
        uint8_t sample_count = 1;
        float coefficient = 1.0;

    public:
        GenericVoltage(uint8_t input_pin, uint8_t sample_count = 1, float coefficient = 1.0): GenericAnalogSensor(input_pin), sample_count(sample_count), coefficient(coefficient) {};
        void read();

        inline float getLevel() const {
            return ((((float) this->level / (float) this->sample_count) / (float) 1024.0) * 5.0) / (float) this->coefficient;
        };
};

#endif
