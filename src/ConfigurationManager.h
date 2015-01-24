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
        static int configuration_address;

        struct Configuration {
            char version[4];
            bool debug;
            uint16_t loop_delay;

            struct {
                uint8_t input_buffer_size;
                uint32_t baud_rate;
            } serial;
        };

    public:
        static Configuration data;

        static void initialize();
        static void load(char* = 0);
        static void save(char* = 0);
};
