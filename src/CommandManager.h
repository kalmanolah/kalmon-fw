#include "ArduinoHeader.h"

class CommandManager {
    public:
        const static uint8_t MAX_COMMAND_LENGTH = 16;
        const static uint8_t MAX_ARGUMENTS_LENGTH = 32;

        typedef void (*Callback)(char*);

        void register_handler(char*, Callback);
        bool handle_command(char*, char*);

    private:
        struct CommandHandler {
            char command[MAX_COMMAND_LENGTH];
            Callback callback;
        };

        uint8_t handler_count = 0;
        CommandHandler handlers[8] = {};
};
