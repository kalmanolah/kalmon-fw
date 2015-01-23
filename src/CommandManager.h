#include "ArduinoHeader.h"

class CommandManager {
    public:
        const static uint8_t MAX_COMMAND_LENGTH = 16;
        const static uint8_t MAX_ARGUMENTS_LENGTH = 32;

        typedef void (*Callback)(char*);

        static void register_handler(char*, Callback);
        static bool handle_command(char*, char*);

    private:
        struct CommandHandler {
            char command[MAX_COMMAND_LENGTH];
            Callback callback;
        };

        static uint8_t handler_count;
        static CommandHandler handlers[8];
};
