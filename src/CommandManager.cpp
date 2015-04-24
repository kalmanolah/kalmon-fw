#include "CommandManager.h"

// Handler count.
uint8_t CommandManager::handler_count = 0;

// Handler array.
CommandManager::CommandHandler CommandManager::handlers[COMMAND_AVAILABLE_SLOTS] = {};

/**
 * Register a handler for a command.
 *
 * @param uint8_t  command  Short representing command
 * @param Callback callback Pointer to a function to execute as a callback upon
 *                          receiving the command
 */
void CommandManager::registerHandler(uint8_t command, Callback callback)
{
    CommandHandler handler;

    handler.command = command;
    handler.callback = callback;

    handlers[handler_count] = handler;
    handler_count++;
}

/**
 * Handle a command.
 *
 * @param  uint8_t command   Short representing command to handle
 * @param  char*   arguments Character array representing command arguments
 * @return bool              Boolean indicating whether or not the command was
 *                           handled
 */
bool CommandManager::handleCommand(uint8_t command, char* arguments)
{
    uint8_t i;

    for (i = 0; i < handler_count; i++) {
        if (handlers[i].command == command) {
            reinterpret_cast<void(*)(char*)>(handlers[i].callback)(arguments);

            return true;
        }
    }

    return false;
}
