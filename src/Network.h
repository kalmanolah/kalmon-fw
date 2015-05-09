#ifndef NETWORK_H
#define NETWORK_H

#define NETWORK_REQUEST_ACK true
#define NETWORK_DEFAULT_MESSAGE_DELAY 250
#define NETWORK_SENSOR_PRESENT_DELAY 500
#define NETWORK_SENSOR_VALUE_SUBMIT_DELAY NETWORK_SENSOR_PRESENT_DELAY

#include <MySensor.h>

// Custom sensor types
#define CS_ACCELEROMETER 128

// Custom value types
#define CV_AVAILABLE_MEMORY 128
#define CV_ACCELERATION_X 129
#define CV_ACCELERATION_Y 130
#define CV_ACCELERATION_Z 131

#include "ModuleManager.h"

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN MySensor gateway;
EXTERN MyMessage gatewayMessage;

void presentSensor(uint8_t module_index, uint8_t sensor_index, uint8_t sensor_type);
void sendCustomData(uint8_t sensor_id = NODE_SENSOR_ID, uint8_t type = V_VAR1, uint16_t value = NULL);

void submitSensorValue(uint8_t module_index, uint8_t sensor_index, uint8_t value_type, uint16_t value);
void submitSensorValue(uint8_t module_index, uint8_t sensor_index, uint8_t value_type, int16_t value);
void submitSensorValue(uint8_t module_index, uint8_t sensor_index, uint8_t value_type, float value);

#endif
