#include "ArduinoHeader.h"
#include <EEPROMex.h>
#include <Logging.h>

// Configuration version to determine data integrity.
#define CONFIG_VERSION "001"

// Size of the configuration block memory pool.
#define CONFIG_MEMORY_SIZE 32

// EEPROM size. Bad things will happen if this isn't set correctly.
#define CONFIG_EEPROM_SIZE EEPROMSizeATmega328

// Configuration options
#define CFG_DEBUG 0
#define CFG_LOOP_DELAY 1
#define CFG_SERIAL_BAUD_RATE 2
#define CFG_SERIAL_INPUT_BUFFER_SIZE 3
#define CFG_SENSOR_UPDATE_INTERVAL 4
#define CFG_POWER_WAKE_DURATION 5
#define CFG_POWER_SLEEP_DURATION 6

class ConfigurationManager {
    private:
        // Config memory address, used to determine where to read and write data.
        static int configuration_address;

        struct Configuration {
            char version[4];
            uint16_t data[32];
        };

    public:
        static Configuration data;

        static void initialize();

        static void load(char* = NULL);
        static void save(char* = NULL);

        static uint16_t get(uint8_t key);
        static void set(uint8_t key, char* value);
};
