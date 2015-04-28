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
