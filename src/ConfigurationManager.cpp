#include "ConfigurationManager.h"

/**
 * Default constructor.
 *
 * @return void
 */
ConfigurationManager::ConfigurationManager() {
    EEPROM.setMemPool(CONFIG_MEMORY_SIZE, CONFIG_EEPROM_SIZE);
    this->configuration_address = EEPROM.getAddress(sizeof(this->data));
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
    EEPROM.readBlock(this->configuration_address, stored);
    Log.Debug("cfg: found; v=%s"CR, stored);

    if (strcmp(stored, this->data.version) != 0) {
        return;
    }

    bytes = EEPROM.readBlock(this->configuration_address, this->data);
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

    bytes = EEPROM.updateBlock(this->configuration_address, this->data);
    Log.Debug("cfg: saved; v=%s, b=%d"CR, this->data.version, bytes);
}
