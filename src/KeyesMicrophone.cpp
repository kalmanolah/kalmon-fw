#include "KeyesMicrophone.h"

KeyesMicrophone::KeyesMicrophone(uint8_t analog_input_pin) {
    this->analog_input_pin = analog_input_pin;
}

void KeyesMicrophone::read() {
    this->level = analogRead(this->analog_input_pin);
}
