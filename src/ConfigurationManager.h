#include "ArduinoHeader.h"
#include <EEPROMex.h>
#include <Logging.h>

// Configuration version to determine data integrity.
#define CONFIG_VERSION "001"

// Size of the configuration block memory pool.
#define CONFIG_MEMORY_SIZE 32

// EEPROM size. Bad things will happen if this isn't set correctly.
#define CONFIG_EEPROM_SIZE EEPROMSizeATmega328

// Amount of available configuration options.
#define CONFIG_AVAILABLE_SLOTS 32

// Configuration options
#define CFG_DEBUG 0
#define CFG_LOOP_DELAY 1
#define CFG_SERIAL_BAUD_RATE 2
#define CFG_SERIAL_INPUT_BUFFER_SIZE 3
#define CFG_SENSOR_UPDATE_INTERVAL 4
#define CFG_POWER_WAKE_DURATION 5
#define CFG_POWER_SLEEP_DURATION 6
#define CFG_NODE_IDENTIFIER 7
#define CFG_HCSR04_SENSOR_TRIG_PIN 8
#define CFG_HCSR04_SENSOR_ECHO_PIN 9
#define CFG_DHT11_SENSOR_PIN 10
#define CFG_KEYESMICROPHONE_SENSOR_PIN 11

class ConfigurationManager {
    private:
        // Config memory address, used to determine where to read and write data.
        static int configuration_address;

        struct Configuration {
            char version[4];
            uint16_t data[CONFIG_AVAILABLE_SLOTS];
        };

    public:
        static Configuration data;

        static void initialize();

        static void load();
        static void save();

        static uint16_t get(uint8_t key);
        static void set(uint8_t key, uint16_t value);
};
