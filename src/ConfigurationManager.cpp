#include "ConfigurationManager.h"

// Configuration address.
int ConfigurationManager::configuration_address = 0;

// Configuration container.
ConfigurationManager::Configuration ConfigurationManager::data = {
    CONFIG_VERSION,
    true,
    50,
    {
        50,
        9600
    }
};

/**
 * Default constructor.
 *
 * @return void
 */
void ConfigurationManager::initialize() {
    EEPROM.setMemPool(CONFIG_MEMORY_SIZE, CONFIG_EEPROM_SIZE);
    configuration_address = EEPROM.getAddress(sizeof(data));
}

/**
 * Load configuration from EEPROM into memory.
 *
 * @return void
 */
void ConfigurationManager::load(char* arguments) {
    char stored[4];
    uint8_t bytes;

    // Ensure the version string matches our version string; if it doesn't, we
    // should just use the default configuration
    EEPROM.readBlock(configuration_address, stored);
    Log.Debug("cfg: found; v=%s"CR, stored);

    if (strcmp(stored, data.version) != 0) {
        return;
    }

    bytes = EEPROM.readBlock(configuration_address, data);
    Log.Debug("cfg: loaded; v=%s, b=%d"CR, stored, bytes);
}

/**
 * Save the current configuration by writing the updated data in memory to
 * EEPROM.
 *
 * @return void
 */
void ConfigurationManager::save(char* arguments) {
    uint8_t bytes;

    bytes = EEPROM.updateBlock(configuration_address, data);
    Log.Debug("cfg: saved; v=%s, b=%d"CR, data.version, bytes);
}
