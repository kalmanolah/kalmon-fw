#define MAIN

#include "ArduinoHeader.h"
#include "KalmonVersion.h"

#include <Logging.h>
#include <elapsedMillis.h>

#include "Network.h"
#include "ConfigurationManager.h"
#include "CommandManager.h"
#include "ModuleManager.h"

#define cfg ConfigurationManager
#define cmd CommandManager
#define mod ModuleManager

#define POWER_INT0_INT1_ENABLED 0b00010001

#define POWER_INT0_ENABLED 0b00000001
#define POWER_INT0_LOW_ENABLED 0b00000001
#define POWER_INT0_CHANGE_ENABLED 0b00000011
#define POWER_INT0_FALLING_ENABLED 0b00000101
#define POWER_INT0_RISING_ENABLED 0b00000111

#define POWER_INT1_ENABLED 0b00010000
#define POWER_INT1_LOW_ENABLED 0b00010000
#define POWER_INT1_CHANGE_ENABLED 0b00110000
#define POWER_INT1_FALLING_ENABLED 0b01010000
#define POWER_INT1_RISING_ENABLED 0b01110000

namespace PowerState {
    enum PowerState { ASLEEP, AWAKE };
}

static uint8_t current_power_state = PowerState::AWAKE;

static struct {
    bool ready;
    String buffer;
} serial_input = {
    false,
    ""
};

static elapsedMillis sensor_update_elapsed;
static elapsedMillis power_state_elapsed;

void initLogging();
void initCommands();
void initConfiguration();
void initConnection();
void initModules();

void handlePowerState();
void handleSerialInput();
void handleSensorUpdates();
void handleConnection();

int getFreeMemory();
uint8_t getBatteryLevel();

void printStats(char* = NULL);
void performReset(char* = NULL);

void loadConfiguration(char* = NULL);
void saveConfiguration(char* = NULL);
void getConfigurationValue(char* args);
void setConfigurationValue(char* args);
