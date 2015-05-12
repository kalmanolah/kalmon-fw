#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include "ArduinoHeader.h"

#include <Logging.h>
#include <Dht11.h>
#include <Wire.h>
#include <ADXL345.h>

#include "Network.h"
#include "Sensor/HCSR04.h"
#include "Sensor/KY038.h"
#include "Sensor/MNEBPTCMN.h"

#define MODULE_AVAILABLE_SLOTS 8
#define MODULE_SENSORS_PER_MODULE 5

#define MODULE_TYPE_DHT11 1
#define MODULE_TYPE_HCSR04 2
#define MODULE_TYPE_KY038 3
#define MODULE_TYPE_MNEBPTCMN 4
#define MODULE_TYPE_ADXL345 5

class ModuleManager {
    public:
        static void registerModule(char*);
        static void updateModules();

    private:
        struct Module {
            uint8_t type;
            void* object;
        };

        static uint8_t module_count;
        static Module modules[MODULE_AVAILABLE_SLOTS];
        static void writeRegister8(uint8_t address, uint8_t reg, uint8_t value);
};

#endif
