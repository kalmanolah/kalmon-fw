#include "Network.h"

/**
 * Present a module's sensor to the gateway.
 *
 * @param module_index Module index.
 * @param sensor_index Sensor index.
 * @param sensor_type  Sensor type.
 *
 * @return void
 */
void presentSensor(uint8_t module_index, uint8_t sensor_index, uint8_t sensor_type)
{
    gateway.present((module_index * MODULE_SENSORS_PER_MODULE) + sensor_index, sensor_type, NETWORK_REQUEST_ACK);
    gateway.wait(NETWORK_SENSOR_PRESENT_DELAY);
}

/**
 * Submit a sensor's value to the gateway.
 *
 * @param module_index      Index of sensor within module to send value for. Starts at 0.
 * @param sensor_index      Index of module the sensor belongs to. Starts at 0.
 * @param sensor_value_type Type of the value sent to the gateway.
 * @param sensor_value      Value sent to the gateway.
 *
 * @return void
 */
void submitSensorValue(uint8_t module_index, uint8_t sensor_index, uint8_t sensor_value_type, uint8_t sensor_value)
{
    gatewayMessage
        .setSensor((module_index * MODULE_SENSORS_PER_MODULE) + sensor_index)
        .setType(sensor_value_type)
        .set(sensor_value);

    gateway.send(gatewayMessage, NETWORK_REQUEST_ACK);
    gateway.wait(NETWORK_SENSOR_VALUE_SUBMIT_DELAY);
}

void sendCustomData(uint8_t sensor_id, uint8_t type, uint16_t value)
{
    gatewayMessage
        .setSensor(sensor_id)
        .setType(type)
        .set(value);

    gateway.send(gatewayMessage, NETWORK_REQUEST_ACK);
    gateway.wait(NETWORK_SENSOR_VALUE_SUBMIT_DELAY);
    // gateway.wait(NETWORK_DEFAULT_MESSAGE_DELAY);
}
