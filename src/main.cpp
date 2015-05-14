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
    initConnection();
    initCommands();
    initInterrupts();
    initModules();
}

/**
 * Loop.
 *
 * @return void
 */
void loop()
{
    handleConnection();

    handleSensorUpdates();
    handlePowerState();

    if (interrupt) {
        handleInterrupt();
    }

    gateway.wait(cfg::getInteger(CFG_LOOP_DELAY));
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
        //cfg::getInteger(CFG_SERIAL_BAUD_RATE)
        BAUD_RATE
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
 * Initialize the connection with the gateway.
 *
 * @return void
 */
void initConnection()
{
    gateway.begin(NULL, !cfg::getInteger(CFG_NODE_ADDRESS) ? AUTO : cfg::getInteger(CFG_NODE_ADDRESS));
    gateway.sendSketchInfo(KALMON_NAME, KALMON_VERSION, true);
}

/**
 * Handle updates to the connection with the gateway.
 *
 * @return void
 */
void handleConnection()
{
    gateway.process();
}

/**
 * Initialize the command manager.
 *
 * @return void
 */
void initCommands()
{
    // Register command handlers
    cmd::registerHandler(21, printStats);
    cmd::registerHandler(22, performReset);

    cmd::registerHandler(41, loadConfiguration);
    cmd::registerHandler(42, saveConfiguration);
    cmd::registerHandler(43, getConfigurationValue);
    cmd::registerHandler(44, setConfigurationValue);
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
 * Initialize interrupt logic.
 *
 * @return void
 */
void initInterrupts()
{
    uint8_t int_options;
    uint8_t int0_options;
    uint8_t int1_options;

    int_options = cfg::getInteger(CFG_POWER_INTERRUPT_OPTIONS);
    int0_options = (int_options & 0b1110) >> 1; // Bit 2 - Bit 4 contain the mode
    int1_options = (int_options & 0b11100000) >> 5; // Bit 5 - Bit 7 contain the mode

    if (int_options & POWER_INT0_ENABLED) {
        attachInterrupt(0, onInterrupt, int0_options);
    }

    if (int_options & POWER_INT1_ENABLED) {
        attachInterrupt(1, onInterrupt, int1_options);
    }
}

/**
 * Determine whether or not sleeping is necessary and if so, initiate the sleep
 * sequence.
 *
 * @return void
 */
void handlePowerState() {
    uint32_t sleep_duration;
    uint8_t int_options;
    uint8_t int0_options;
    uint8_t int1_options;
    uint8_t retval;

    // If we have a waking period and it has expired, go to sleep
    if (current_power_state == PowerState::AWAKE
        && cfg::getInteger(CFG_POWER_WAKE_DURATION) > 0
        && ((cfg::getInteger(CFG_POWER_SLEEP_DURATION) > 0) || ((cfg::getInteger(CFG_POWER_INTERRUPT_OPTIONS) & POWER_INT0_INT1_ENABLED) > 0))
        && (power_state_elapsed / 1000) >= cfg::getInteger(CFG_POWER_WAKE_DURATION)) {
        Log.Debug(F("pwr: sleeping"CR));
        gateway.wait(200);

        sleep_duration = (uint32_t) cfg::getInteger(CFG_POWER_SLEEP_DURATION) * 1000;
        int_options = cfg::getInteger(CFG_POWER_INTERRUPT_OPTIONS);
        int0_options = (int_options & 0b1110) >> 1; // Bit 2 - Bit 4 contain the mode
        int1_options = (int_options & 0b11100000) >> 5; // Bit 5 - Bit 7 contain the mode

        // Set the power state to asleep, since this function could've been called
        // using a serial command
        current_power_state = PowerState::ASLEEP;

        if ((int_options & POWER_INT0_INT1_ENABLED) == POWER_INT0_INT1_ENABLED) {
            retval = gateway.sleep(0, int0_options, 1, int1_options, sleep_duration);
            interrupt = retval != -1;
        } else if (int_options & POWER_INT0_ENABLED) {
            retval = gateway.sleep(0, int0_options, sleep_duration);
            interrupt = retval == true;
        } else if (int_options & POWER_INT1_ENABLED) {
            retval = gateway.sleep(1, int1_options, sleep_duration);
            interrupt = retval == true;
        } else {
            gateway.sleep(sleep_duration);
        }

        current_power_state = PowerState::AWAKE;

        // Reset all counting timers after a wakeup
        power_state_elapsed = 0;
        sensor_update_elapsed = 0;

        // Re-initialize interrupts after a wakeup
        initInterrupts();

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
    uint8_t command;
    char arguments[cfg::getInteger(CFG_SERIAL_INPUT_BUFFER_SIZE) - 2];

    // Try to find the index of the first space
    // If no space was found, we set the index to the length of the string - 1
    space_index = serial_input.buffer.indexOf(' ');

    if (space_index == -1) {
        space_index = serial_input.buffer.length() - 1;
    }

    // Pass our command handler everything up to the first space converted to
    // int (command identifier) along with everything after the first space as
    // args
    command = serial_input.buffer.substring(0, space_index).toInt();
    serial_input.buffer.substring(space_index + 1).toCharArray(arguments, sizeof(arguments));

    Log.Debug(F("cmd: \"%d\"; args: \"%s\""CR), command, arguments);

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

        // Submit the battery level and some other stats while we're at it
        gateway.sendBatteryLevel(getBatteryLevel(), NETWORK_REQUEST_ACK);
        sendCustomData(NODE_SENSOR_ID, CV_AVAILABLE_MEMORY, getFreeMemory());

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
 * Returns the current battery level as a percentage.
 *
 * @return uint8_t
 */
uint8_t getBatteryLevel() {
    // @TODO Add support for measuring battery voltage
    return 100;
}

/**
 * Prints stats.
 *
 * @return void
 */
void printStats(char* args) {
    Log.Info(F("free: %dB"CR), getFreeMemory());
    Log.Info(F("battery: %d%%"CR), getBatteryLevel());
}

/**
 * Performs a soft reset.
 *
 * @return void
 */
void performReset(char* args) {
    Log.Info(F("reset"CR));
    gateway.wait(200);

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

/**
 * Triggered on interrupt.
 *
 * @return void
 */
void onInterrupt()
{
    interrupt = true;
}

/**
 * Handles an interrupt.
 *
 * @return void
 */
void handleInterrupt()
{
    // Trigger a sensor update
    mod::updateModules();

    interrupt = false;
}
