#include "CommandManager.h"

/**
 * Register a handler for a command.
 *
 * @param char*    command  Character array representing command to handle
 * @param Callback callback Pointer to a function to execute as a callback upon
 *                          receiving the command
 */
void CommandManager::register_handler(char* command, Callback callback)
{
    CommandHandler handler;

    strcpy(handler.command, command);
    handler.callback = callback;

    handlers[handler_count] = handler;
    handler_count++;
}

/**
 * Handle a command.
 *
 * @param  char* command   Character array representing command to handle
 * @param  char* arguments Character array representing command arguments
 * @return bool            Boolean indicating whether or not the command was
 *                         handled
 */
bool CommandManager::handle_command(char* command, char* arguments)
{
    for (int i = 0; i < handler_count; i++) {
        if (strcmp(handlers[i].command, command) == 0) {
            reinterpret_cast<void(*)(char*)>(handlers[i].callback)(arguments);

            return true;
        }
    }

    return false;
}
