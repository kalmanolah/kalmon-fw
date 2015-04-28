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
            case MODULE_TYPE_DHT11:
                {
                    uint8_t pin = strtol(strsep(&string, ","), NULL, 10);

                    if (pin > 0) {
                        Dht11 object = Dht11(pin);
                        memcpy(module.object, &object, sizeof(object));

                        presentSensor(module_count, 0, S_HUM);
                        presentSensor(module_count, 1, S_TEMP);
                    }
                }

                break;
            case MODULE_TYPE_HCSR04:
                {
                    uint8_t trig_pin = strtol(strsep(&string, ","), NULL, 10);
                    uint8_t echo_pin = strtol(strsep(&string, ","), NULL, 10);

                    if (trig_pin > 0 && echo_pin > 0) {
                        HCSR04 object = HCSR04(trig_pin, echo_pin);
                        memcpy(module.object, &object, sizeof(object));

                        presentSensor(module_count, 0, S_DISTANCE);
                    }
                }

                break;

            case MODULE_TYPE_KY038:
                {
                    uint8_t pin = strtol(strsep(&string, ","), NULL, 10);

                    if (pin > 0) {
                        KY038 object = KY038(pin);
                        memcpy(module.object, &object, sizeof(object));

                        presentSensor(module_count, 0, S_CUSTOM);
                    }
                }

                break;

            case MODULE_TYPE_MNEBPTCMN:
                {
                    uint8_t pin = strtol(strsep(&string, ","), NULL, 10);

                    if (pin > 0) {
                        MNEBPTCMN object = MNEBPTCMN(pin);
                        memcpy(module.object, &object, sizeof(object));

                        presentSensor(module_count, 0, S_LIGHT_LEVEL);
                    }
                }

                break;

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

            case MODULE_TYPE_HCSR04:
                {
                    HCSR04* object = reinterpret_cast<HCSR04*>(modules[i].object);
                    object->read();
                    Log.Debug(F("duration: %lμs"CR), object->getDuration());
                    Log.Debug(F("distance: %lcm"CR), object->getDistance());

                    submitSensorValue(i, 0, V_DISTANCE, object->getDistance());
                }

                break;

            case MODULE_TYPE_KY038:
                {
                    KY038* object = reinterpret_cast<KY038*>(modules[i].object);
                    object->read();
                    Log.Debug(F("sound: %d"CR), object->getLevel());

                    submitSensorValue(i, 0, V_VAR1, object->getLevel());
                }

                break;

            case MODULE_TYPE_MNEBPTCMN:
                {
                    MNEBPTCMN* object = reinterpret_cast<MNEBPTCMN*>(modules[i].object);
                    object->read();
                    Log.Debug(F("light: %d"CR), object->getLevel());

                    submitSensorValue(i, 0, V_LIGHT_LEVEL, object->getLevel());
                }

                break;

            default:
                break;
        }
    }
}
