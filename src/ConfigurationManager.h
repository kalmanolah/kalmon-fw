#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include "ArduinoHeader.h"
#include "KalmonVersion.h"

#include <EEPROMex.h>
#include <Logging.h>

// Size of the configuration block memory pool.
//#define CONFIG_MEMORY_SIZE 192
#define CONFIG_MEMORY_START 512

// EEPROM size. Bad things will happen if this isn't set correctly.
#define CONFIG_EEPROM_SIZE EEPROMSizeATmega328

#define CONFIG_BOOLEANS_AVAILABLE_SLOTS 8
#define CONFIG_BOOLEANS_OFFSET 0
#define CFG_DEBUG 0

#define CONFIG_INTEGERS_AVAILABLE_SLOTS 16
#define CONFIG_INTEGERS_OFFSET 8
#define CFG_LOOP_DELAY 8
#define CFG_SERIAL_BAUD_RATE 9
#define CFG_SERIAL_INPUT_BUFFER_SIZE 10
#define CFG_SENSOR_UPDATE_INTERVAL 11
#define CFG_POWER_WAKE_DURATION 12
#define CFG_POWER_SLEEP_DURATION 13
#define CFG_POWER_INTERRUPT_OPTIONS 14
#define CFG_NODE_ADDRESS 15

#define CONFIG_STRINGS_AVAILABLE_SLOTS 8
#define CONFIG_STRINGS_OFFSET 24
#define CONFIG_STRINGS_MAX_SIZE 11
#define CFG_MODULE_1_CONFIGURATION 24
#define CFG_MODULE_2_CONFIGURATION 25
#define CFG_MODULE_3_CONFIGURATION 26
#define CFG_MODULE_4_CONFIGURATION 27
#define CFG_MODULE_5_CONFIGURATION 28
#define CFG_MODULE_6_CONFIGURATION 29
#define CFG_MODULE_7_CONFIGURATION 30
#define CFG_MODULE_8_CONFIGURATION 31

class ConfigurationManager {
    private:
        // Config memory address, used to determine where to read and write data.
        static int configuration_address;

        struct Configuration {
            char version[4];
            bool booleans[CONFIG_BOOLEANS_AVAILABLE_SLOTS];
            uint16_t integers[CONFIG_INTEGERS_AVAILABLE_SLOTS];
            char strings[CONFIG_STRINGS_AVAILABLE_SLOTS][CONFIG_STRINGS_MAX_SIZE];
        };

    public:
        static Configuration data;

        static void initialize();

        static void load();
        static void save();

        static bool getBoolean(uint8_t key);
        static uint16_t getInteger(uint8_t key);
        static char* getString(uint8_t key);

        static void setBoolean(uint8_t key, bool value);
        static void setInteger(uint8_t key, uint16_t value);
        static void setString(uint8_t key, char* value);
};

#endif
