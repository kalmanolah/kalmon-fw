#include "ArduinoHeader.h"

#include <Logging.h>
#include <Dht11.h>
#include "HCSR04.h"
#include "KeyesMicrophone.h"

#define MODULE_AVAILABLE_SLOTS 8

#define MODULE_TYPE_DHT11 1
#define MODULE_TYPE_HCSR04 2
#define MODULE_TYPE_KEYESMICROPHONE 3

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
