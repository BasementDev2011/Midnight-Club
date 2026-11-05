#ifndef TTY_HANDLE_H
#define TTY_HANDLE_H

#include <termios.h>

#include "actions.h"
#include "globals.h"

#define USER_INPUT_BUFFER_SIZE 16
#define NAME_EXECUTABLE_MAX_SIZE 1024
#define SETTLER_COMMAND_MAX_SIZE 1024

extern struct termios original_configuration;
extern struct termios modified_configuration;
extern char user_input_buffer[USER_INPUT_BUFFER_SIZE];
extern ssize_t selector;
extern void exec_action(int action, struct filesystem_object_t object);
//extern char user_input_buffer[USER_INPUT_BUFFER_SIZE];

void tty_reset(void) {
    tcsetattr(STDIN_FILENO,TCSANOW,&original_configuration);
}

void tty_setup(void) {
    tcgetattr(STDIN_FILENO,&original_configuration);
    modified_configuration = original_configuration;
    modified_configuration.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&modified_configuration);
}

void DEBUG_tty_input_output(void) {
    printf("DEBUG User inputs:\n");
    for (size_t i = 0; i < USER_INPUT_BUFFER_SIZE; i++) {
        printf("%d ",user_input_buffer[i]);
    }
    printf("\n");
}

void tty_identify(char data[], size_t data_size, struct filesystem_object_t object) {
    if (data[0] == TTY_ESC && data[1] == TTY_SQUARE_BRACKET) {
        switch(data[2]) {
            case TTY_EQB_UP: {
                selector--;
                break;
            }

            case TTY_EQB_DOWN: {
                selector++;
                break;
            }

            case TTY_EQB_LEFT: {
                //toggle ls -l quick reference to TRUE
            }

            case TTY_EQB_RIGHT: {
                //toggle ls -l quick reference to FALSE
            }
        }
    } else {
        switch (data[0]) {
            case TTY_SPACE:
                exec_action(ACTION_RUN_PROGRAM,object);
                break;

            case TTY_ENTER:
                if (object.type == DT_REG) {
                    exec_action(ACTION_OPEN_FILE,object);
                } else if (object.type == DT_DIR) {
                    exec_action(ACTION_GO_TO_DIR,object);
                }
                break;

            case TTY_ESC:
                exec_action(ACTION_QUIT,object);
                break;

            case TTY_BACKSPACE:
                exec_action(ACTION_GO_BACK,object);
                break;

            case TTY_Q:
                exec_action(ACTION_GET_METADATA_FROM_OBJECT,object);
                break;
            case TTY_W:
                exec_action(ACTION_SETTLE,object);
                break;
            case TTY_E:
                if (object.type == DT_REG) {
                    exec_action(ACTION_DELETE_FILE,object);
                } else if (object.type == DT_DIR) {
                    exec_action(ACTION_DELETE_FOLDER,object);
                }
                break;
        }
    }
    for (size_t i = 0; i < data_size; i++) {
        data[i] = 0x00;
    }
}

#endif /*TTY_HANDLE_H*/
