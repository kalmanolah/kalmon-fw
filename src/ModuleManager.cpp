#include "ModuleManager.h"

// Module count.
uint8_t ModuleManager::module_count = 0;

// Module array.
ModuleManager::Module ModuleManager::modules[MODULE_AVAILABLE_SLOTS] = {};

/**
 * Register a module.
 *
 * @param char* configuration Character array containing module
 *                            configuration.
 *
 * @return void
 */
void ModuleManager::registerModule(char* configuration)
{
    Module module;
    uint8_t type;

    char* string;
    char* to_free;

    to_free = string = strdup(configuration);

    // First parsed token is the type
    type = strtol(strsep(&string, ","), NULL, 10);

    if (type > 0) {
        module.type = type;

        switch (module.type) {
            #ifdef MODULE_TYPE_DHT11
            case MODULE_TYPE_DHT11:
                {
                    uint8_t pin = strtol(strsep(&string, ","), NULL, 10);

                    if (pin > 0) {
                        Dht11 object = Dht11(pin);
                        module.object = malloc(sizeof(object));
                        memcpy(module.object, &object, sizeof(object));

                        presentSensor(module_count, 0, S_HUM);
                        presentSensor(module_count, 1, S_TEMP);
                    }
                }

                break;
            #endif

            #ifdef MODULE_TYPE_HCSR04
            case MODULE_TYPE_HCSR04:
                {
                    uint8_t trig_pin = strtol(strsep(&string, ","), NULL, 10);
                    uint8_t echo_pin = strtol(strsep(&string, ","), NULL, 10);

                    if (trig_pin > 0 && echo_pin > 0) {
                        HCSR04 object = HCSR04(trig_pin, echo_pin);
                        module.object = malloc(sizeof(object));
                        memcpy(module.object, &object, sizeof(object));

                        presentSensor(module_count, 0, S_DISTANCE);
                    }
                }

                break;
            #endif

            #ifdef MODULE_TYPE_KY038
            case MODULE_TYPE_KY038:
                {
                    uint8_t pin = strtol(strsep(&string, ","), NULL, 10);

                    if (pin > 0) {
                        KY038 object = KY038(pin);
                        module.object = malloc(sizeof(object));
                        memcpy(module.object, &object, sizeof(object));

                        presentSensor(module_count, 0, S_CUSTOM);
                    }
                }

                break;
            #endif

            #ifdef MODULE_TYPE_MNEBPTCMN
            case MODULE_TYPE_MNEBPTCMN:
                {
                    uint8_t pin = strtol(strsep(&string, ","), NULL, 10);

                    if (pin > 0) {
                        MNEBPTCMN object = MNEBPTCMN(pin);
                        module.object = malloc(sizeof(object));
                        memcpy(module.object, &object, sizeof(object));

                        presentSensor(module_count, 0, S_LIGHT_LEVEL);
                    }
                }

                break;
            #endif

            #ifdef MODULE_TYPE_ADXL345
            case MODULE_TYPE_ADXL345:
                {
                    uint8_t options[6];
                    uint8_t i;

                    // Load options
                    for (i = 0; i < 3; i++) {
                        options[i] = strtol(strsep(&string, ","), NULL, 10);
                    }

                    ADXL345 object = ADXL345();

                    if (!object.begin()) {
                        Log.Error(F("Error loading ADXL345"CR));
                    }

                    // Configure power control mode
                    // For link and auto-slide alongside measurements, use 0b00111000 / 0x38
                    // writeRegister8(ADXL345_ADDRESS, ADXL345_REG_POWER_CTL, 0x00);
                    writeRegister8(ADXL345_ADDRESS, ADXL345_REG_POWER_CTL, options[5] > 0 ? options[5] : 0x08); // Default to measurement mode
                    writeRegister8(ADXL345_ADDRESS, ADXL345_REG_FIFO_CTL, /*0b10000000*/ 0x80);
                    // writeRegister8(ADXL345_ADDRESS, ADXL345_REG_POWER_CTL, /*0b00111000*/ 0x38);

                    // Configure activity detection
                    if (options[0] > 0) {
                        object.setActivityXYZ(1);
                        object.setActivityThreshold(options[0]);
                    }

                    // Configure inactivity detection
                    if (options[1] > 0) {
                        object.setInactivityXYZ(1);
                        object.setInactivityThreshold(options[1]);
                    }

                    // Configure inactivity time
                    object.setTimeInactivity(options[2] > 0 ? options[2] : 5);

                    // Configure sensitivity
                    object.setRange(/*options[3] > 0 ? options[3] :*/ ADXL345_RANGE_16G);

                    // Configure data rate
                    object.setDataRate(/*options[4] > 0 ? options[4] :*/ ADXL345_DATARATE_100HZ);

                    // Set correct interrupt to use
                    object.useInterrupt(ADXL345_INT2);

                    // Present accelerometer
                    presentSensor(module_count, 0, CS_ACCELEROMETER);

                    // If activity or inactivity detection is enabled
                    if (options[0] > 0 || options[1] > 0) {
                        // If activity detection is enabled, present a motion sensor in addition
                        // to the accelerometer
                        presentSensor(module_count, 1, S_MOTION);
                    }

                    // Bind interrupt
                    // attachInterrupt(1, onADXL345Interrupt, RISING);

                    module.object = malloc(sizeof(object));
                    memcpy(module.object, &object, sizeof(object));
                }

                break;
            #endif

            default:
                break;
        }

        if (module.object != NULL) {
            Log.Debug(F("mod: slot=%d, type=%d, configuration=%s"CR), module_count, module.type, configuration);
            modules[module_count] = module;
            module_count++;
        }
    }

    // free(to_free);
}

/**
 * Perform updates for modules.
 *
 * @return void
 */
void ModuleManager::updateModules()
{
    for (int i = 0; i < MODULE_AVAILABLE_SLOTS; i++) {
        if (!modules[i].type) {
            continue;
        }

        switch (modules[i].type) {
            #ifdef MODULE_TYPE_DHT11
            case MODULE_TYPE_DHT11:
                {
                    Dht11* object = reinterpret_cast<Dht11*>(modules[i].object);
                    //object->read();

                    switch (object->read()) {
                        case Dht11::OK:
                            Log.Debug(F("humidity: %d%%"CR), object->getHumidity());
                            Log.Debug(F("temperature: %d°C"CR), object->getTemperature());

                            submitSensorValue(i, 0, V_HUM, object->getHumidity());
                            submitSensorValue(i, 1, V_TEMP, object->getTemperature());

                            break;

                        case Dht11::ERROR_CHECKSUM:
                            Log.Error(F("dht11: checksum error"CR));
                            break;

                        case Dht11::ERROR_TIMEOUT:
                            Log.Error(F("dht11: timeout error"CR));
                            break;

                        default:
                            Log.Error(F("dht11: unknown error"CR));
                            break;

                        // default:
                        //     Log.Error(F("dht11: error"CR));
                        //     break;
                    }
                }

                break;
            #endif

            #ifdef MODULE_TYPE_HCSR04
            case MODULE_TYPE_HCSR04:
                {
                    HCSR04* object = reinterpret_cast<HCSR04*>(modules[i].object);
                    object->read();
                    Log.Debug(F("duration: %lμs"CR), object->getDuration());
                    Log.Debug(F("distance: %lcm"CR), object->getDistance());

                    submitSensorValue(i, 0, V_DISTANCE, (uint16_t) object->getDistance());
                }

                break;
            #endif

            #ifdef MODULE_TYPE_KY038
            case MODULE_TYPE_KY038:
                {
                    KY038* object = reinterpret_cast<KY038*>(modules[i].object);
                    object->read();
                    Log.Debug(F("sound: %d"CR), object->getLevel());

                    submitSensorValue(i, 0, V_VAR1, object->getLevel());
                }

                break;
            #endif

            #ifdef MODULE_TYPE_MNEBPTCMN
            case MODULE_TYPE_MNEBPTCMN:
                {
                    MNEBPTCMN* object = reinterpret_cast<MNEBPTCMN*>(modules[i].object);
                    object->read();
                    Log.Debug(F("light: %d"CR), object->getLevel());

                    submitSensorValue(i, 0, V_LIGHT_LEVEL, object->getLevel());
                }

                break;
            #endif

            #ifdef MODULE_TYPE_ADXL345
            case MODULE_TYPE_ADXL345:
                {
                    ADXL345* object = reinterpret_cast<ADXL345*>(modules[i].object);

                    Vector norm = object->readNormalize();
                    Log.Debug(F("acceleration: x=%d, y=%d, z=%d"CR), norm.XAxis, norm.YAxis, norm.ZAxis);

                    submitSensorValue(i, 0, CV_ACCELERATION_X, norm.XAxis);
                    submitSensorValue(i, 0, CV_ACCELERATION_Y, norm.YAxis);
                    submitSensorValue(i, 0, CV_ACCELERATION_Z, norm.ZAxis);

                    // If activity or inactivity detection is enabled, also submit motion sensor data
                    if (object->getActivityX() || object->getInactivityX()) {
                        Activites activ = object->readActivites();
                        Log.Debug(F("activity: %T"CR), activ.isActivity && !activ.isInactivity);

                        submitSensorValue(i, 1, V_TRIPPED, activ.isActivity && !activ.isInactivity);
                    }

                    // Serial.println("sleepy");
                    // delay(200);
                    // pinMode(3, INPUT);
                    // int retval = gateway.sleep(1, RISING, 0);

                    // if (retval) {
                    //     onADXL345Interrupt();
                    // }
                    // delay(200);
                    // Serial.println("wakey");
                }

                break;
            #endif

            default:
                break;
        }
    }
}

/**
 * Write a value to a register by register + address.
 */
void ModuleManager::writeRegister8(uint8_t address, uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// void ModuleManager::onADXL345Interrupt()
// {
//     Serial.println("interrupt");

//     for (int i = 0; i < MODULE_AVAILABLE_SLOTS; i++) {
//         if (!modules[i].type) {
//             continue;
//         }

//         switch (modules[i].type) {
//             #ifdef MODULE_TYPE_ADXL345
//             case MODULE_TYPE_ADXL345:
//                 {
//                     ADXL345* object = reinterpret_cast<ADXL345*>(modules[i].object);

//                     Activites activ = object->readActivites();
//                 }

//                 break;
//             #endif

//             default:
//                 break;
//         }
//     }
// }
