#include "ConfigurationManager.h"

// Configuration address.
int ConfigurationManager::configuration_address = 0;

// Configuration container.
ConfigurationManager::Configuration ConfigurationManager::data = {
    KALMON_VERSION,
    {
        true   // debug
    },
    {
        50,    // loop delay
        9600,  // serial baud rate
        32,    // serial input buffer size
        15,    // sensor update interval
        25,    // awake duration
        1800,  // sleep duration
        0,     // interrupt options
        10     // default node address
    },
    {

    }
};

/**
 * Default constructor.
 *
 * @return void
 */
void ConfigurationManager::initialize() {
    EEPROM.setMemPool(CONFIG_MEMORY_START, CONFIG_EEPROM_SIZE);
    configuration_address = EEPROM.getAddress(sizeof(data));
}

/**
 * Load configuration from EEPROM into memory.
 *
 * @return void
 */
void ConfigurationManager::load() {
    char stored[4];
    uint8_t bytes;

    // Ensure the version string matches our version string; if it doesn't, we
    // should just use the default configuration
    EEPROM.readBlock(configuration_address, stored);
    Log.Debug(F("cfg: found; v=%s"CR), stored);

    if (strcmp(stored, data.version) != 0) {
        return;
    }

    bytes = EEPROM.readBlock(configuration_address, data);
    Log.Debug(F("cfg: loaded; v=%s, B=%d"CR), stored, bytes);
}

/**
 * Save the current configuration by writing the updated data in memory to
 * EEPROM.
 *
 * @return void
 */
void ConfigurationManager::save() {
    uint8_t bytes;

    bytes = EEPROM.updateBlock(configuration_address, data);
    Log.Debug(F("cfg: saved; v=%s, B=%d"CR), data.version, bytes);
}

/**
 * Return the value of a key.
 *
 * @return bool
 */
bool ConfigurationManager::getBoolean(uint8_t key) {
    return data.booleans[key - CONFIG_BOOLEANS_OFFSET];
}

/**
 * Return the value of a key.
 *
 * @return uint16_t
 */
uint16_t ConfigurationManager::getInteger(uint8_t key) {
    return data.integers[key - CONFIG_INTEGERS_OFFSET];
}

/**
 * Return the value of a key.
 *
 * @return char*
 */
char* ConfigurationManager::getString(uint8_t key) {
    return data.strings[key - CONFIG_STRINGS_OFFSET];
}


/**
 * Set the value for a key.
 *
 * @return void
 */
void ConfigurationManager::setBoolean(uint8_t key, bool value) {
    data.booleans[key - CONFIG_BOOLEANS_OFFSET] = value;
    save();
}

/**
 * Set the value for a key.
 *
 * @return void
 */
void ConfigurationManager::setInteger(uint8_t key, uint16_t value) {
    data.integers[key - CONFIG_INTEGERS_OFFSET] = value;
    save();
}

/**
 * Set the value for a key.
 *
 * @return void
 */
void ConfigurationManager::setString(uint8_t key, char* value) {
    strcpy(data.strings[key - CONFIG_STRINGS_OFFSET], value);
    save();
}
