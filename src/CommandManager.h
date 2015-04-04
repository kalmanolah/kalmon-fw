#include "ArduinoHeader.h"

#define COMMAND_AVAILABLE_SLOTS 8
#define COMMAND_MAX_SIZE 9

class CommandManager {
    public:
        typedef void (*Callback)(char*);

        static void register_handler(char*, Callback);
        static bool handle_command(char*, char*);

    private:
        struct CommandHandler {
            char command[COMMAND_MAX_SIZE];
            Callback callback;
        };

        static uint8_t handler_count;
        static CommandHandler handlers[COMMAND_AVAILABLE_SLOTS];
};
