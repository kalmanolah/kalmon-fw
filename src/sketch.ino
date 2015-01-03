#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <EEPROMex.h>

// Configuration version, used for validating configuration integrity.
#define CONFIG_VERSION "001"

// Size of the configuration block memory pool.
const int CONFIG_MEMORY_SIZE = 32;

// EEPROM size. Bad things will happen if this isn't set correctly.
const int CONFIG_EEPROM_SIZE = EEPROMSizeATmega328;

// Config memory address, used to determine where to read and write data.
int config_address = 0;

// Configuration structure. Also contains the default configuration.
struct ConfigurationStruct {
    // Character string indicating configuration version.
    char version[4];

    // Delay between loops, in milliseconds.
    int loop_delay;

    // Serial input buffer size, in bytes.
    int serial_input_buffer_size;

    // Boolean indicating whether debug mode is enabled.
    bool debug;
} config = {
    CONFIG_VERSION,
    50,
    32,
    false
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

// Enum containing power states.
enum PowerState {
    SLEEPING,
    AWAKE
};

// Current power state
int POWER_STATE = AWAKE;

/**
 * Constructor.
 *
 * @return void
 */
void setup()
{
    Serial.begin(9600);
    while (!Serial) {} // Wait for serial port to be ready

    EEPROM.setMemPool(CONFIG_MEMORY_SIZE, CONFIG_EEPROM_SIZE);
    config_address = EEPROM.getAddress(sizeof(ConfigurationStruct));
    loadConfiguration();

    serial_input.buffer.reserve(config.serial_input_buffer_size);

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
 * Load configuration from EEPROM into memory.
 *
 * @return void
 */
void loadConfiguration() {
    // Ensure the version string matches our version string; if it doesn't, we
    // should just use the default configuration
    char stored_version[4];
    EEPROM.readBlock(config_address, stored_version);

    if (strcmp(stored_version, config.version) == 0) {
        int bytes_read = EEPROM.readBlock(config_address, config);

        Serial.print("Loaded configuration; version: ");
        Serial.print(stored_version);
        Serial.print("; bytes read: ");
        Serial.println(bytes_read);
    } else {
        Serial.print("Not loading old configuration; version: ");
        Serial.println(stored_version);
    }
}

/**
 * Save the current configuration by writing the updated data in memory to
 * EEPROM.
 *
 * @return void
 */
void saveConfiguration() {
    Serial.println("Saving configuration");

    int bytes_written = EEPROM.updateBlock(config_address, config);

    Serial.print("Configuration saved; version: ");
    Serial.print(config.version);
    Serial.print("; bytes written: ");
    Serial.println(bytes_written);
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
    if (POWER_STATE == SLEEPING) {
        return;
    }

    POWER_BTN_STATES <<= 1;
    POWER_BTN_STATES |= digitalRead(POWER_BTN);

    if ((byte) (POWER_BTN_STATES << 6) == B10000000) {
        POWER_STATE = POWER_STATE == AWAKE ? SLEEPING : AWAKE;
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
    if (POWER_STATE != SLEEPING) {
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
    Serial.println("Entering sleep mode");
    delay(200);

    // Set the power state to asleep, since this function could've been called
    // using a serial command
    POWER_STATE = SLEEPING;

    digitalWrite(POWER_LED, LOW);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    attachInterrupt(0, wakeUp, CHANGE);

    sleep_mode();

    detachInterrupt(0);
    sleep_disable();
    digitalWrite(POWER_LED, HIGH);

    POWER_STATE = AWAKE;
    Serial.println("Exiting sleep mode");
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
    Serial.print("Handling input: ");
    Serial.println(serial_input.buffer);

    if (serial_input.buffer == "sleep") {
        goToSleep();
    } else if (serial_input.buffer == "load") {
        loadConfiguration();
    } else if (serial_input.buffer == "save") {
        saveConfiguration();
    } else {
        Serial.println("Invalid input");
    }

    serial_input.buffer = "";
    serial_input.ready = false;
}
