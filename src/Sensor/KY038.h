/**
    Keyes Microphone Sound Detection Sensor (KY-038) class

    http://www.dx.com/p/arduino-microphone-sound-detection-sensor-module-red-135533#.VSbAR_YRRhE
 */

#ifndef ky038_h
#define ky038_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#    include <Arduino.h>
#else
#    include <WProgram.h>
#endif

/*
 * KY038
 *
 * A class that is in charge of managing a Keyes Microphone Sound Detection sensor.
 */
class KY038 {
private:
    // The last measured sound level
    uint16_t level = 0;

    // The pin we use to communicate with the sensor
    uint8_t analog_input_pin;

public:
    /*
     * KY038
     *
     * Constructs a new KY038 object that manages a Keyes Microphone Sound Detection sensor
     * using a given analog input pin.
     */
    KY038(uint8_t analog_input_pin);

    /*
     * read
     *
     * Update the duration and distance of this object from the sensor.
     */
    void read();

    /*
     * getLavel
     *
     * Gets the last read sound level.
     */
    inline uint16_t getLevel() const {
        return this->level;
    }

private:
};

#endif
