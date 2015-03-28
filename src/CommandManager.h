#include "ArduinoHeader.h"

#define COMMAND_AVAILABLE_SLOTS 8

class CommandManager {
    public:
        const static uint8_t MAX_COMMAND_SIZE = 9;

        typedef void (*Callback)(char*);

        static void register_handler(char*, Callback);
        static bool handle_command(char*, char*);

    private:
        struct CommandHandler {
            char command[MAX_COMMAND_SIZE];
            Callback callback;
        };

        static uint8_t handler_count;
        static CommandHandler handlers[COMMAND_AVAILABLE_SLOTS];
};
