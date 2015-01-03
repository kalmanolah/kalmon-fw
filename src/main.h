#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <EEPROMex.h>
#include <Logging.h>

using namespace std;

// Configuration version, used for validating configuration integrity.
#define CONFIG_VERSION "001"

// Size of the configuration block memory pool.
#define CONFIG_MEMORY_SIZE 32

// EEPROM size. Bad things will happen if this isn't set correctly.
#define CONFIG_EEPROM_SIZE EEPROMSizeATmega328

// Namespaced enum containing power states.
namespace PowerState {
    enum PowerState { ASLEEP, AWAKE };
}

void initLogging();
void loadConfiguration();
void saveConfiguration();
void determinePowerState();
void determineSleepState();
void goToSleep();
void wakeUp();
void handleSerialInput();
