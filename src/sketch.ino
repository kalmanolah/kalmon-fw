#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <EEPROMex.h>

/**
 * Configuration version, used for validating configuration integrity.
 * @type char*
 */
const char* CONFIG_VERSION = "1";

/**
 * Size of the configuration block memory pool.
 * @type int
 */
const int CONFIG_MEMORY_SIZE = 32;

/**
 * EEPROM size. Bad things will happen if this isn't set correctly.
 * @type int
 */
const int CONFIG_EEPROM_SIZE = EEPROMSizeATmega328;

/**
 * Configuration memory address, used to determine where to read and write
 * data.
 * @type int
 */
int config_address = 0;

/**
 * Configuration structure. Also contains the default configuration.
 */
struct ConfigurationStruct {
    /**
     * Delay between loops, in milliseconds.
     * @type int
     */
    int loop_delay;

    /**
     * Serial input buffer size, in bytes.
     * @type int
     */
    int serial_input_buffer_size;

    /**
     * Boolean indicating whether debug mode is enabled.
     * @type bool
     */
    bool debug;

    /**
     * Character string indicating configuration version.
     * @type char[4]
     */
    char* version[4];
} config = {
    50,
    32,
    false,
    (char*) CONFIG_VERSION
};

/**
 * Serial input structure.
 */
struct SerialInputStruct {
    /**
     * Boolean indicating whether the input buffer is ready for processing.
     * @type bool
     */
    bool ready;

    /**
     * Input buffer containing received serial data.
     * @type String|char*
     */
    String buffer;
} serial_input = {
    false,
    ""
};

/**
 * Power button pin number.
 * @type int
 */
const int POWER_BTN = 2;

/**
 * Power LED pin number.
 * @type int
 */
const int POWER_LED = 13;

/**
 * Byte containing states/state history of the power button.
 * @type byte
 */
byte POWER_BTN_STATES = B11111111;

/**
 * Enum containing power states.
 * @type enum
 */
enum PowerState {
    SLEEPING,
    AWAKE
};

/**
 * Boolean indicating current power state.
 * 0 = sleeping / powered down
 * 1 = awake / fully powered
 * @type int
 */
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
    determinePowerState();
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
    if (EEPROM.readByte(config_address + sizeof(config) - 1) == *config.version[3]
        && EEPROM.readByte(config_address + sizeof(config) - 2) == *config.version[2]
        && EEPROM.readByte(config_address + sizeof(config) - 3) == *config.version[1]
        && EEPROM.readByte(config_address + sizeof(config) - 4) == *config.version[0]) {
        Serial.println("Configuration matched!");
        EEPROM.readBlock(config_address, config);
    } else {
        Serial.println("Configuration did not match!");
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
    Serial.println(EEPROM.updateBlock(config_address, config));
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
