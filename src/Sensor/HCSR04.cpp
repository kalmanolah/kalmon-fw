#include "HCSR04.h"

HCSR04::HCSR04(uint8_t trig_pin, uint8_t echo_pin) {
    this->trig_pin = trig_pin;
    this->echo_pin = echo_pin;

    pinMode(this->trig_pin, OUTPUT);
    pinMode(this->echo_pin, INPUT);
}

void HCSR04::read() {
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(this->trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(this->trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(this->trig_pin, LOW);

    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    this->duration = pulseIn(this->echo_pin, HIGH);

    // convert the time into a distance
    this->distance = this->duration / 58.2;
}
