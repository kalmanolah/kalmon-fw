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
    // Initialize logging with default configuration to capture initial debug
    // output
    initLogging();

    // Initialize all the things
    initConfiguration();
    initLogging();
    initCommands();

    serial_input.buffer.reserve(cfg.data.serial.input_buffer_size);

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

    delay(cfg.data.loop_delay);
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
        cfg.data.debug ? LOG_LEVEL_DEBUG : LOG_LEVEL_INFOS,
        cfg.data.serial.baud_rate
    );
}

/**
 * Initialize the configuration manager.
 *
 * @return void
 */
void initConfiguration()
{
    cfg = ConfigurationManager();
    cfg.load();
}

/**
 * Load configuration.
 *
 * This is a wrapper function created because dealing with a
 * pointer-to-function is a hell of a lot easier than dealing with a
 * pointer-to-instance-method.
 *
 * @return void.
 */
void loadConfiguration(char* args)
{
    cfg.load();
}

/**
 * Save configuration.
 *
 * This is a wrapper function created because dealing with a
 * pointer-to-function is a hell of a lot easier than dealing with a
 * pointer-to-instance-method.
 *
 * @return void.
 */
void saveConfiguration(char* args)
{
    cfg.save();
}

/**
 * Initialize the command manager.
 *
 * @return void
 */
void initCommands()
{
    cmd = CommandManager();

    // Register command handlers
    cmd.register_handler("cfg_load", loadConfiguration);
    cmd.register_handler("cfg_save", saveConfiguration);
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
    Log.Debug("pwr: sleeping"CR);
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
    Log.Debug("pwr: awake"CR);
}

/**
 * Interrupt executed upon waking of the device.
 *
 * @return void
 */
void wakeUp() {
    Log.Debug("pwd: waking"CR);
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

        serial_input.buffer += ch;
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
    char command[CommandManager::MAX_COMMAND_LENGTH];
    char arguments[CommandManager::MAX_ARGUMENTS_LENGTH];

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

    Log.Debug("cmd: \"%s\"; args: \"%s\""CR, command, arguments);

    if (!cmd.handle_command(command, arguments)) {
        Log.Error("cmd: invalid"CR);
    }

    serial_input.buffer = "";
    serial_input.ready = false;
}
