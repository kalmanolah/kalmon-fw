#include "ArduinoHeader.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Logging.h>

#include "ConfigurationManager.h"
#include "CommandManager.h"

//using namespace std;

// Namespaced enum containing power states.
namespace PowerState {
    enum PowerState { ASLEEP, AWAKE };
}

ConfigurationManager cfg;
CommandManager cmd;

void initLogging();
void initCommands();

void initConfiguration();
void loadConfiguration(char* = 0);
void saveConfiguration(char* = 0);

void determinePowerState();
void determineSleepState();
void goToSleep();
void wakeUp();
void handleSerialInput();
