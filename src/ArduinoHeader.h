#ifndef ARDUINO_HEADER_H
#define ARDUINO_HEADER_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#endif
