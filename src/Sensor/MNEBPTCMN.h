/**
    Meeeno MN-EB-PTCMN Photosensitive Sensor Module  (MN-EB-PTCMN) class

    http://www.dx.com/p/meeeno-mn-eb-ptcmn-photosensitive-sensor-module-orange-202511#.VS046_YRRhE
 */

#ifndef mnebptcmn_h
#define mnebptcmn_h

#include "GenericAnalogSensor.h"

/*
 * MNEBPTCMN
 *
 * A class that is in charge of managing a Meeeno MN-EB-PTCMN Photosensitive Sensor.
 */
class MNEBPTCMN: public GenericAnalogSensor {
    public:
        MNEBPTCMN(uint8_t input_pin): GenericAnalogSensor(input_pin) {};

        inline uint16_t getLevel() const {
            return 1023 - this->level;
        };
};

#endif
