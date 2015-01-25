#include "ArduinoHeader.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Logging.h>

#include "ConfigurationManager.h"
#include "CommandManager.h"

#define cfg ConfigurationManager
#define cmd CommandManager

//using namespace std;

// Namespaced enum containing power states.
namespace PowerState {
    enum PowerState { ASLEEP, AWAKE };
}

void initLogging();
void initCommands();
void initConfiguration();

void determinePowerState();
void determineSleepState();
void goToSleep();
void wakeUp();
void handleSerialInput();

int getFreeMemory();

void printStats(char* = NULL);
void performReset(char* = NULL);
