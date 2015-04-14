#include "ArduinoHeader.h"

#include <Logging.h>
#include <Dht11.h>
#include "Sensor/HCSR04.h"
#include "Sensor/KY038.h"
#include "Sensor/MNEBPTCMN.h"

#define MODULE_AVAILABLE_SLOTS 8

#define MODULE_TYPE_DHT11 1
#define MODULE_TYPE_HCSR04 2
#define MODULE_TYPE_KY038 3
#define MODULE_TYPE_MNEBPTCMN 4

class ModuleManager {
    public:
        static void registerModule(char*);
        static void updateModules();

    private:
        struct Module {
            uint8_t type;
            char object[16];
        };

        static uint8_t module_count;
        static Module modules[MODULE_AVAILABLE_SLOTS];
};
