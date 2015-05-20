#include "GenericVoltage.h"

void GenericVoltage::read() {
    uint8_t i;

    this->level = 0;

    for (i = 0; i < this->sample_count; i++) {
        this->level += analogRead(this->input_pin);
        delay(20);
    }

    // this->level /= this->sample_count;
}
