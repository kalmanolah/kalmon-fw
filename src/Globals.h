#ifndef GLOBALS_H
#define GLOBALS_H

#include <MySensor.h>

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN MySensor gateway;
EXTERN MyMessage gatewaySensorMessage;

#endif
