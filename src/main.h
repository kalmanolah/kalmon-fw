#include "ArduinoHeader.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Logging.h>
#include <elapsedMillis.h>
#include <Sleep_n0m1.h>

#include "ConfigurationManager.h"
#include "CommandManager.h"
#include "ModuleManager.h"

#define cfg ConfigurationManager
#define cmd CommandManager
#define mod ModuleManager

//using namespace std;

const uint8_t POWER_LED = 13;

namespace PowerState {
    enum PowerState { ASLEEP, AWAKE };
}

static uint8_t current_power_state = PowerState::AWAKE;

static Sleep sleeper;

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
void initModules();
void initPower();

void handlePowerState();
void handleSerialInput();
void handleSensorUpdates();

int getFreeMemory();

void printStats(char* = NULL);
void performReset(char* = NULL);

void loadConfiguration(char* = NULL);
void saveConfiguration(char* = NULL);
void getConfigurationValue(char* args);
void setConfigurationValue(char* args);
