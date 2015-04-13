#include "KY038.h"

KY038::KY038(uint8_t analog_input_pin) {
    this->analog_input_pin = analog_input_pin;
}

void KY038::read() {
    delay(500);
    Serial.println(analogRead(this->analog_input_pin));
    this->level = analogRead(this->analog_input_pin);
}
