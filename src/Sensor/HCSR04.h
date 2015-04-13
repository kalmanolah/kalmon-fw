/**
    HC-SR04 Ultrasonic Sensor class

    Based on a gist found here:
    https://gist.github.com/flakas/3294829

    Original docblock here:

    """
    HC-SR04 Sensor
    https://www.dealextreme.com/p/hc-sr04-ultrasonic-sensor-distance-measuring-module-133696

    This sketch reads a HC-SR04 ultrasonic rangefinder and returns the
    distance to the closest object in range. To do this, it sends a pulse
    to the sensor to initiate a reading, then listens for a pulse
    to return.  The length of the returning pulse is proportional to
    the distance of the object from the sensor.

    The circuit:
    * VCC connection of the sensor attached to +5V
    * GND connection of the sensor attached to ground
    * TRIG connection of the sensor attached to digital pin 2
    * ECHO connection of the sensor attached to digital pin 4


    Original code for Ping))) example was created by David A. Mellis
    Adapted for HC-SR04 by Tautvidas Sipavicius

    This example code is in the public domain.
    """
 */

#ifndef hcsr04_h
#define hcsr04_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#    include <Arduino.h>
#else
#    include <WProgram.h>
#endif

/*
 * HCSR04
 *
 * A class that is in charge of managing a HC-SR04 ultrasonic sensor.
 */
class HCSR04 {
private:
    // The last measured duration
    uint32_t duration = 0;

    // The last measured distance
    uint32_t distance = 0;

    // The pins we use to communicate with the sensor
    uint8_t echo_pin;
    uint8_t trig_pin;

public:
    /*
     * HCSR04
     *
     * Constructs a new HCSR04 object that manages a HC-SR04 ultrasonic sensor
     * using a given echo pin and a given trig pin.
     */
    HCSR04(uint8_t trig_pin, uint8_t echo_pin);

    /*
     * read
     *
     * Update the duration and distance of this object from the sensor.
     */
    void read();

    /*
     * getDistance
     *
     * Gets the last read distance in centimeters.
     */
    inline long getDistance() const {
        return this->distance;
    }

    /*
     * getDuration
     *
     * Gets the last read duration in microseconds.
     */
    inline long getDuration() const {
        return this->duration;
    }

private:
};

#endif
