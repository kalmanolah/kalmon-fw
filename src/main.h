#include "ArduinoHeader.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Logging.h>
#include <Dht11.h>
#include <elapsedMillis.h>
#include <Sleep_n0m1.h>

#include "ConfigurationManager.h"
#include "CommandManager.h"
#include "HCSR04.h"

#define cfg ConfigurationManager
#define cmd CommandManager

//using namespace std;

const uint8_t POWER_LED = 13;

namespace PowerState {
    enum PowerState { ASLEEP, AWAKE };
}

static Sleep sleeper;

static uint8_t current_power_state = PowerState::AWAKE;

static elapsedMillis sensor_update_elapsed;
static elapsedMillis power_state_elapsed;

static Dht11 dht11_sensor(2);
static HCSR04 hcsr04_sensor(4, 6);

void initLogging();
void initCommands();
void initConfiguration();

void handlePowerState();
void handleSerialInput();
void handleSensorUpdates();

int getFreeMemory();

void printStats(char* = NULL);
void performReset(char* = NULL);
