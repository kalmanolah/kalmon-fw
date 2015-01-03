#include "main.h"

// Config memory address, used to determine where to read and write data.
int config_address = 0;

// Configuration structure. Also contains the default configuration.
struct ConfigurationStruct {
    // Character string indicating configuration version.
    char version[4];

    // Boolean indicating whether debug mode is enabled.
    bool debug;

    // Delay between loops, in milliseconds.
    int loop_delay;

    struct {
        // Serial input buffer size, in bytes.
        int input_buffer_size;

        // Serial baud rate.
        int baud_rate;
    } serial;
} config = {
    CONFIG_VERSION,
    true,
    50,
    {
        50,
        9600
    }
};


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


// Power button pin number.
const int POWER_BTN = 2;

// Power LED pin number.
const int POWER_LED = 13;

// Byte containing states/state history of the power button.
byte POWER_BTN_STATES = B11111111;

// Current power state
int POWER_STATE = PowerState::AWAKE;


/**
 * Constructor.
 *
 * @return void
 */
void setup()
{
    // Initialize logging with default configuration to capture error output
    initLogging();

    EEPROM.setMemPool(CONFIG_MEMORY_SIZE, CONFIG_EEPROM_SIZE);
    config_address = EEPROM.getAddress(sizeof(ConfigurationStruct));
    loadConfiguration();

    // Properly initialize logger this time
    initLogging();

    serial_input.buffer.reserve(config.serial.input_buffer_size);

    pinMode(POWER_BTN, INPUT);
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
    // determinePowerState();
    determineSleepState();

    delay(config.loop_delay);
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

    Log.Init(config.debug ? LOG_LEVEL_DEBUG : LOG_LEVEL_INFOS,
        config.serial.baud_rate);
}

/**
 * Load configuration from EEPROM into memory.
 *
 * @return void
 */
void loadConfiguration() {
    char stored[4];
    int bytes;

    Log.Debug("Loading config"CR);

    // Ensure the version string matches our version string; if it doesn't, we
    // should just use the default configuration
    EEPROM.readBlock(config_address, stored);
    Log.Debug("Found config, version=%s"CR, stored);

    if (strcmp(stored, config.version) != 0) {
        return;
    }

    bytes = EEPROM.readBlock(config_address, config);
    Log.Debug("Config loaded, version=%s, bytes=%d"CR, stored, bytes);
}

/**
 * Save the current configuration by writing the updated data in memory to
 * EEPROM.
 *
 * @return void
 */
void saveConfiguration() {
    int bytes;

    Log.Debug("Saving config"CR);

    bytes = EEPROM.updateBlock(config_address, config);
    Log.Debug("Config saved, version=%s, bytes=%d"CR, config.version, bytes);
}

/**
 * Determine whether or not the power button has been pressed. if so, toggle
 * the power state.
 *
 * @return void
 */
void determinePowerState() {
    // The button shouldn't do anything while we're sleeping, because the
    // interrupt which will wake the device is a much more low-level thing
    if (POWER_STATE == PowerState::ASLEEP) {
        return;
    }

    POWER_BTN_STATES <<= 1;
    POWER_BTN_STATES |= digitalRead(POWER_BTN);

    if ((byte) (POWER_BTN_STATES << 6) == B10000000) {
        POWER_STATE = POWER_STATE == PowerState::AWAKE ?
            PowerState::ASLEEP : PowerState::AWAKE;
    }
}

/**
 * Determine whether or not sleeping is necessary and if so, initiate the sleep
 * sequence.
 *
 * @return void
 */
void determineSleepState() {
    // If we're awake, we obviously don't want to sleep
    if (POWER_STATE != PowerState::ASLEEP) {
        return;
    }

    goToSleep();
}

/**
 * Enter sleep mode, conserving battery usage. An interrupt will be attached to
 * changes in the state of digital pin 2.
 *
 * @return void
 */
void goToSleep() {
    Log.Debug("Entering sleep mode"CR);
    delay(200);

    // Set the power state to asleep, since this function could've been called
    // using a serial command
    POWER_STATE = PowerState::ASLEEP;

    digitalWrite(POWER_LED, LOW);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    attachInterrupt(0, wakeUp, CHANGE);

    sleep_mode();

    detachInterrupt(0);
    sleep_disable();
    digitalWrite(POWER_LED, HIGH);

    POWER_STATE = PowerState::AWAKE;
    Log.Debug("Exiting sleep mode"CR);
}

/**
 * Interrupt executed upon waking of the device.
 *
 * @return void
 */
void wakeUp() {
}

/**
 * Handler executed upon receiving serial input. This handler will append
 * received data to the buffer, and if a line ending is received, will trigger
 * the processing of the received input.
 *
 * @return void
 */
void serialEvent() {
    while (Serial.available()) {
        char ch = (char) Serial.read();

        if (ch == '\n' || ch == '\r') {
            serial_input.ready = true;
            break;
        }

        serial_input.buffer += ch;
    }

    if (serial_input.ready) {
        handleSerialInput();
    }
}

/**
 * Handle a received command and clear the buffer afterwards.
 *
 * @return void
 */
void handleSerialInput() {
    char input[32];
    bool handled = false;

    serial_input.buffer.toCharArray(input, sizeof(input));
    Log.Debug("Handling input: %s"CR, input);

    if (serial_input.buffer.startsWith("cfg ")) {
        if (serial_input.buffer.substring(4) == "save") {
            handled = true;
            saveConfiguration();
        } else if (serial_input.buffer.substring(4) == "load") {
            handled = true;
            loadConfiguration();
        }
    }

    if (!handled) {
        Log.Error("Invalid input: %s"CR, input);
    }

    serial_input.buffer = "";
    serial_input.ready = false;
}
