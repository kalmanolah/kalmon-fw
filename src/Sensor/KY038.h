/**
    Keyes Microphone Sound Detection Sensor (KY-038) class

    http://www.dx.com/p/arduino-microphone-sound-detection-sensor-module-red-135533#.VSbAR_YRRhE
 */

#ifndef ky038_h
#define ky038_h

#include "GenericAnalogSensor.h"

/*
 * KY038
 *
 * A class that is in charge of managing a Keyes Microphone Sound Detection sensor.
 */
class KY038: public GenericAnalogSensor {
    public:
        KY038(uint8_t input_pin): GenericAnalogSensor(input_pin) {};
};

#endif
