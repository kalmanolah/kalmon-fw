#include "GenericAnalogSensor.h"

void GenericAnalogSensor::read()
{
    this->level = analogRead(this->input_pin);
}
