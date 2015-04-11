#include "main.h"

/**
 * Constructor.
 *
 * @return void
 */
void setup()
{
    // Initialize logging with default configuration to capture initial debug output
    initLogging();

    // Initialize all the things
    initConfiguration();
    initLogging();
    initCommands();
    initModules();
    initPower();

    pinMode(POWER_LED, OUTPUT);
    digitalWrite(POWER_LED, HIGH);
}

/**
 * Loop.
 *
 * @return void
 */
void loop()
{
    handleSensorUpdates();
    handlePowerState();

    delay(cfg::getInteger(CFG_LOOP_DELAY));
}

/**
 * Initialize the logger.
 *
 * @return void
 */
void initLogging()
{
    if (Serial) {
        Serial.end();
    }

    Log.Init(
        cfg::getBoolean(CFG_DEBUG) ? LOG_LEVEL_DEBUG : LOG_LEVEL_INFOS,
        cfg::getInteger(CFG_SERIAL_BAUD_RATE)
    );
}

/**
 * Initialize the configuration manager.
 *
 * @return void
 */
void initConfiguration()
{
    cfg::initialize();
    cfg::load();
}

/**
 * Initialize the command manager.
 *
 * @return void
 */
void initCommands()
{
    // Register command handlers
    cmd::registerHandler("cfg_load", loadConfiguration);
    cmd::registerHandler("cfg_save", saveConfiguration);
    cmd::registerHandler("cfg_get", getConfigurationValue);
    cmd::registerHandler("cfg_set", setConfigurationValue);
    cmd::registerHandler("stats", printStats);
    cmd::registerHandler("reset", performReset);
}

/**
 * Initialize attached modules.
 *
 * @return void
 */
void initModules()
{
    for (int i = CFG_MODULE_1_CONFIGURATION; i < (CFG_MODULE_1_CONFIGURATION + MODULE_AVAILABLE_SLOTS); i++) {
        mod::registerModule(cfg::getString(i));
    }
}

/**
 * Initialize power management.
 *
 * @return void
 */
void initPower()
{
    sleeper.setCalibrationInterval(5);
}

/**
 * Determine whether or not sleeping is necessary and if so, initiate the sleep
 * sequence.
 *
 * @return void
 */
void handlePowerState() {
    // If we have a waking period and it has expired, go to sleep
    if (current_power_state == PowerState::AWAKE
        && cfg::getInteger(CFG_POWER_WAKE_DURATION) > 0
        && cfg::getInteger(CFG_POWER_SLEEP_DURATION) > 0
        && (power_state_elapsed / 1000) >= cfg::getInteger(CFG_POWER_WAKE_DURATION)) {
        Log.Debug(F("pwr: sleeping"CR));
        delay(200);

        // Set the power state to asleep, since this function could've been called
        // using a serial command
        current_power_state = PowerState::ASLEEP;

        digitalWrite(POWER_LED, LOW);

        sleeper.pwrDownMode();
        sleeper.sleepDelay((uint32_t) cfg::getInteger(CFG_POWER_SLEEP_DURATION) * 1000);

        digitalWrite(POWER_LED, HIGH);

        current_power_state = PowerState::AWAKE;
        power_state_elapsed = 0;
        Log.Debug(F("pwr: waking"CR));
    }
}

/**
 * Handler executed upon receiving serial input. This handler will append
 * received data to the buffer, and if a line ending is received, will trigger
 * the processing of the received input.
 *
 * @return void
 */
void serialEvent() {
    char ch;

    while (Serial.available()) {
        ch = (char) Serial.read();

        // If we receive a newline, break the loop
        if (ch == '\n' || ch == '\r') {
            // Try to handle a command if our buffer isn't empty
            serial_input.ready = serial_input.buffer.length() > 0;
            break;
        }

        if (serial_input.buffer.length() < cfg::getInteger(CFG_SERIAL_INPUT_BUFFER_SIZE)) {
            serial_input.buffer += ch;
        }
    }

    if (serial_input.ready) {
        handleSerialInput();
    }
}

/**
 * Handle serial input when it is ready for processing.
 *
 * @return void
 */
void handleSerialInput() {
    uint8_t space_index;
    char command[COMMAND_MAX_SIZE];
    char arguments[cfg::getInteger(CFG_SERIAL_INPUT_BUFFER_SIZE) - COMMAND_MAX_SIZE - 1];

    // Try to find the index of the first space
    // If no space was found, we set the index to the length of the string - 1
    space_index = serial_input.buffer.indexOf(' ');

    if (space_index == -1) {
        space_index = serial_input.buffer.length() - 1;
    }

    // Pass our command handler everything up to the first space converted to
    // int (command identifier) along with everything after the first space as
    // args
    serial_input.buffer.substring(0, space_index).toCharArray(command, sizeof(command));
    serial_input.buffer.substring(space_index + 1).toCharArray(arguments, sizeof(arguments));

    Log.Debug(F("cmd: \"%s\"; args: \"%s\""CR), command, arguments);

    if (!cmd::handleCommand(command, arguments)) {
        Log.Error(F("cmd: invalid"CR));
    }

    serial_input.buffer = "";
    serial_input.ready = false;
}

/**
 * Handle sensor updates.
 *
 * @return void
 */
void handleSensorUpdates() {
    if (cfg::getInteger(CFG_SENSOR_UPDATE_INTERVAL) > 0
        && (sensor_update_elapsed / 1000) >= cfg::getInteger(CFG_SENSOR_UPDATE_INTERVAL)) {
        Log.Debug(F("mod: updating"CR));
        mod::updateModules();
        sensor_update_elapsed = 0;
    }
}

/**
 * Returns the amount of free memory in bytes.
 *
 * @return int
 */
int getFreeMemory() {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/**
 * Prints stats.
 *
 * @return void
 */
void printStats(char* args) {
    Log.Info(F("free: %dB"CR), getFreeMemory());
}

/**
 * Performs a soft reset.
 *
 * @return void
 */
void performReset(char* args) {
    Log.Info(F("reset"CR));
    delay(200);

    asm volatile("  jmp 0");
}

/**
 * Load configuration.
 *
 * @return void
 */
void loadConfiguration(char* args) {
    cfg::load();
}

/**
 * Save configuration.
 *
 * @return void
 */
void saveConfiguration(char* args) {
    cfg::save();
}

/**
 * Retrieve and print a configuration value.
 *
 * @return void
 */
void getConfigurationValue(char* args) {
    uint8_t key;
    char* errstr;

    key = strtol(args, &errstr, 10);

    if (key >= (CONFIG_STRINGS_AVAILABLE_SLOTS + CONFIG_STRINGS_OFFSET)) {
        Log.Error(F("cfg: key out of bounds; max=%d"CR), (CONFIG_STRINGS_AVAILABLE_SLOTS + CONFIG_STRINGS_OFFSET - 1));
    } else if (*errstr) {
        Log.Error(F("cfg: error converting key; part=%s"CR), errstr);
    } else {
        if (key >= CONFIG_STRINGS_OFFSET) {
            Log.Info(F("cfg: %s=%s"CR), args, cfg::getString(key));
        } else if (key >= CONFIG_INTEGERS_OFFSET) {
            Log.Info(F("cfg: %s=%d"CR), args, cfg::getInteger(key));
        } else {
            Log.Info(F("cfg: %s=%d"CR), args, cfg::getBoolean(key));
        }
    }
}

/**
 * Set a configuration value.
 *
 * @return void
 */
void setConfigurationValue(char* args) {
    uint8_t key;
    uint16_t value;
    char* errstr;
    char* key_tok;
    char* value_tok;

    key_tok = strtok(args, " ");
    value_tok = strtok(NULL, " ");

    if (*key_tok && *value_tok) {
        key = strtol(key_tok, &errstr, 10);

        if (key >= (CONFIG_STRINGS_AVAILABLE_SLOTS + CONFIG_STRINGS_OFFSET - 1)) {
            Log.Error(F("cfg: key out of bounds; max=%d"CR), (CONFIG_STRINGS_AVAILABLE_SLOTS + CONFIG_STRINGS_OFFSET - 1));
        } else if (*errstr) {
            Log.Error(F("cfg: error converting key; part=%s"CR), errstr);
        } else {
            if (key >= CONFIG_STRINGS_OFFSET) {
                cfg::setString(key, value_tok);
                Log.Info(F("cfg: %d=%s"CR), key, value_tok);
            } else if (key >= CONFIG_INTEGERS_OFFSET) {
                value = strtol(value_tok, &errstr, 10);

                if (*errstr) {
                    Log.Error(F("cfg: error converting value; part=%s"CR), errstr);
                } else {
                    cfg::setInteger(key, value);
                    Log.Info(F("cfg: %d=%d"CR), key, value);
                }
            } else {
                cfg::setBoolean(key, value_tok[0] == '1');
                Log.Info(F("cfg: %d=%d"CR), key, value_tok[0] == '1');
            }
        }
    }
}
