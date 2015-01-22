#include "ArduinoHeader.h"
#include <EEPROMex.h>
#include <Logging.h>

// Configuration version to determine data integrity.
#define CONFIG_VERSION "000"

// Size of the configuration block memory pool.
#define CONFIG_MEMORY_SIZE 32

// EEPROM size. Bad things will happen if this isn't set correctly.
#define CONFIG_EEPROM_SIZE EEPROMSizeATmega328

class ConfigurationManager {
    private:
        // Config memory address, used to determine where to read and write data.
        int configuration_address = 0;

        struct Configuration {
            char version[4];
            bool debug;
            uint8_t loop_delay;

            struct {
                uint8_t input_buffer_size;
                uint16_t baud_rate;
            } serial;
        } DEFAULT_CONFIGURATION = {
            CONFIG_VERSION,
            true,
            50,
            {
                50,
                9600
            }
        };

    public:
        Configuration data = DEFAULT_CONFIGURATION;

        ConfigurationManager();
        void load(char* = 0);
        void save(char* = 0);
};
