#include "main.h"


// Serial input structure.
struct SerialInputStruct {
    // Boolean indicating whether the input buffer is ready for processing.
    bool ready;

    // Input buffer containing received serial data.
    String buffer;
} serial_input = {
    false,
    ""
};


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

    delay(cfg::get(CFG_LOOP_DELAY));
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
        (bool) cfg::get(CFG_DEBUG) ? LOG_LEVEL_DEBUG : LOG_LEVEL_INFOS,
        cfg::get(CFG_SERIAL_BAUD_RATE)
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
    cmd::register_handler("cfg_load", loadConfiguration);
    cmd::register_handler("cfg_save", saveConfiguration);
    cmd::register_handler("cfg_get", getConfigurationValue);
    cmd::register_handler("cfg_set", setConfigurationValue);
    cmd::register_handler("stats", printStats);
    cmd::register_handler("reset", performReset);
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
        && cfg::get(CFG_POWER_WAKE_DURATION) > 0
        && cfg::get(CFG_POWER_SLEEP_DURATION) > 0
        && (power_state_elapsed / 1000) >= cfg::get(CFG_POWER_WAKE_DURATION)) {
        Log.Debug(F("pwr: sleeping"CR));
        delay(200);

        // Set the power state to asleep, since this function could've been called
        // using a serial command
        current_power_state = PowerState::ASLEEP;

        digitalWrite(POWER_LED, LOW);

        sleeper.pwrDownMode();
        sleeper.sleepDelay((uint32_t) cfg::get(CFG_POWER_SLEEP_DURATION) * 1000);

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

        if (serial_input.buffer.length() < cfg::get(CFG_SERIAL_INPUT_BUFFER_SIZE)) {
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
    char command[CommandManager::MAX_COMMAND_SIZE];
    char arguments[cfg::get(CFG_SERIAL_INPUT_BUFFER_SIZE) - CommandManager::MAX_COMMAND_SIZE - 1];

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

    if (!cmd::handle_command(command, arguments)) {
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
    if (cfg::get(CFG_SENSOR_UPDATE_INTERVAL) > 0
        && (sensor_update_elapsed / 1000) >= cfg::get(CFG_SENSOR_UPDATE_INTERVAL)) {
        Log.Debug(F("updating sensors"CR));

        switch (dht11_sensor.read()) {
            case Dht11::OK:
                Log.Debug(F("humidity: %d%%"CR), dht11_sensor.getHumidity());
                Log.Debug(F("temperature: %d°C"CR), dht11_sensor.getTemperature());
                break;

            case Dht11::ERROR_CHECKSUM:
                Log.Error(F("dht11: checksum error"CR));
                break;

            case Dht11::ERROR_TIMEOUT:
                Log.Error(F("dht11: timeout error"CR));
                break;

            default:
                Log.Error(F("dht11: unknown error"CR));
                break;

            // default:
            //     Log.Error(F("dht11: error"CR));
            //     break;
        }

        hcsr04_sensor.read();
        Log.Debug(F("duration: %lμs"CR), hcsr04_sensor.getDuration());
        Log.Debug(F("distance: %lcm"CR), hcsr04_sensor.getDistance());

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

    if (key >= CONFIG_AVAILABLE_SLOTS) {
        Log.Error(F("cfg: key out of bounds; max=%d"CR), CONFIG_AVAILABLE_SLOTS - 1);
    } else if (*errstr) {
        Log.Error(F("cfg: error converting key; part=%s"CR), errstr);
    } else {
        Log.Info(F("cfg: %s=%d"CR), args, cfg::get(key));
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

        if (key >= CONFIG_AVAILABLE_SLOTS) {
            Log.Error(F("cfg: key out of bounds; max=%d"CR), CONFIG_AVAILABLE_SLOTS - 1);
        } else if (*errstr) {
            Log.Error(F("cfg: error converting key; part=%s"CR), errstr);
        } else {
            value = strtol(value_tok, &errstr, 10);

            if (*errstr) {
                Log.Error(F("cfg: error converting value; part=%s"CR), errstr);
            } else {
                cfg::set(key, value);
                Log.Info(F("cfg: %d=%d"CR), key, value);
            }
        }
    }
}
