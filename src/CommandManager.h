#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include "ArduinoHeader.h"

#define COMMAND_AVAILABLE_SLOTS 16

class CommandManager {
    public:
        typedef void (*Callback)(char*);

        static void registerHandler(uint8_t, Callback);
        static bool handleCommand(uint8_t, char*);

    private:
        struct CommandHandler {
            uint8_t command;
            Callback callback;
        };

        static uint8_t handler_count;
        static CommandHandler handlers[COMMAND_AVAILABLE_SLOTS];
};

#endif
