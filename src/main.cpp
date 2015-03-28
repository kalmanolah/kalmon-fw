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
    handlePowerState();
    handleSensorUpdates();

    delay(cfg::data.loop_delay);
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
        cfg::data.debug ? LOG_LEVEL_DEBUG : LOG_LEVEL_INFOS,
        cfg::data.serial.baud_rate
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
    cmd::register_handler("cfg_load", cfg::load);
    cmd::register_handler("cfg_save", cfg::save);
    cmd::register_handler("stats", printStats);
    cmd::register_handler("reset", performReset);
}

/**
 * Determine whether or not sleeping is necessary and if so, initiate the sleep
 * sequence.
 *
 * @return void
 */
void handlePowerState() {
    // If we have a waking period and it has expired, go to sleep
    if (current_power_state == PowerState::AWAKE && ((power_state_elapsed / 1000) >= power_state_seconds.awake)) {
        Log.Debug(F("pwr: sleeping"CR));
        delay(200);

        // Set the power state to asleep, since this function could've been called
        // using a serial command
        current_power_state = PowerState::ASLEEP;

        digitalWrite(POWER_LED, LOW);

        sleeper.pwrDownMode();
        sleeper.sleepDelay(power_state_seconds.asleep * 1000);

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

        if (serial_input.buffer.length() < cfg::data.serial.input_buffer_size) {
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
    char arguments[cfg::data.serial.input_buffer_size - CommandManager::MAX_COMMAND_SIZE - 1];

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
    Log.Debug(F("free: b=%d"CR), getFreeMemory());
}

/**
 * Performs a soft reset.
 *
 * @return void
 */
void performReset(char* args) {
    Log.Debug(F("reset"CR));
    delay(200);

    asm volatile("  jmp 0");
}

/**
 * Handle sensor updates.
 *
 * @return void
 */
void handleSensorUpdates() {
    if (sensor_update_elapsed >= cfg::data.sensors.update_interval == true) {
        Log.Debug(F("updating sensors"CR));

        switch (dht11_sensor.read()) {
            case Dht11::OK:
                Log.Debug(F("humidity: %d%%"CR), dht11_sensor.getHumidity());
                Log.Debug(F("temperature: %d°C"CR), dht11_sensor.getTemperature());
                break;

            // case Dht11::ERROR_CHECKSUM:
            //     Log.Error(F("dht11: checksum error"CR));
            //     break;

            // case Dht11::ERROR_TIMEOUT:
            //     Log.Error(F("dht11: timeout error"CR));
            //     break;

            // default:
            //     Log.Error(F("dht11: unknown error"CR));
            //     break;

            default:
                Log.Error(F("dht11: error"CR));
                break;
        }

        hcsr04_sensor.read();
        Log.Debug(F("duration: %lμs"CR), hcsr04_sensor.getDuration());
        Log.Debug(F("distance: %lcm"CR), hcsr04_sensor.getDistance());

        sensor_update_elapsed = 0;
    }
}
