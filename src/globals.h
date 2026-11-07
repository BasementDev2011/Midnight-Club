#ifndef GLOBALS_H
#define GLOBALS_H

struct filesystem_object_t {
    unsigned char type;
    char name[NAME_MAX];
};

char current_working_directory[NAME_MAX];

#define TTY_ESC 27
#define TTY_SQUARE_BRACKET 91
#define TTY_Q 113

#define TTY_EQB_UP 65
#define TTY_EQB_DOWN 66
#define TTY_EQB_LEFT 68
#define TTY_EQB_RIGHT 67
#define TTY_ENTER 10
#define TTY_BACKSPACE 127
#define TTY_W 119
#define TTY_E 101
#define TTY_A 97
#define TTY_S 115
#define TTY_D 100

#define TTY_SPACE 32

enum {
    ACTION_SETTLE,
    ACTION_RUN_PROGRAM,
    ACTION_OPEN_FILE,
    ACTION_QUIT,
    ACTION_GO_BACK,
    ACTION_GO_TO_DIR,
    ACTION_GET_METADATA_FROM_OBJECT,
    ACTION_DELETE_FILE,
    ACTION_DELETE_FOLDER,
    ACTION_CREATE_FILE,
    ACTION_CREATE_FOLDER,
    ACTION_RENAME_OBJECT
};

#endif
