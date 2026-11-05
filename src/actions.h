#ifndef ACTIONS_H
#define ACTIONS_H

#include "globals.h"
#include "tty_handle.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

void exec_action(int action, struct filesystem_object_t object) {
    char c = '\0';
    switch (action) {
        case ACTION_SETTLE:
            printf("Settling into '%s'\n",current_working_directory);
            tty_reset();
            char arg[NAME_EXECUTABLE_MAX_SIZE];
            sprintf(arg,"cd '%s' && exec bash",current_working_directory);
            execl("/bin/bash", "bash", "-c", arg, (char *)NULL);
            exit(0);
            break;
        case ACTION_RUN_PROGRAM:
            pid_t pid_run_program = fork();
            tty_reset();
            if (pid_run_program == 0) {
                printf("[Midnight Club] Running program '%s'\n",object.name);
                char executable_name[NAME_EXECUTABLE_MAX_SIZE];
                sprintf(executable_name,"./%s",object.name);

                execl(executable_name, object.name, NULL);
                _exit(1);
            }

            if (pid_run_program > 0) {
                waitpid(pid_run_program, NULL, 0);
                tty_setup();
                printf("[Midnight Club] Press any key to continue browsing\n");
                char c = '\0';
                tty_setup();
                read(STDIN_FILENO,&c,1);
            }
            break;
        case ACTION_OPEN_FILE:
            pid_t pid_open_file = fork();

            if (pid_open_file == 0) {
                char *args[] = {"nano", object.name, NULL};

                execvp(args[0], args);
                _exit(1);
            }

            if (pid_open_file > 0) {
                waitpid(pid_open_file, NULL, 0);
            }
            break;
        case ACTION_QUIT:
            tty_reset();
            exit(0);
            break;
        case ACTION_GO_BACK:
            chdir("..");
            break;
        case ACTION_GO_TO_DIR:
            chdir(object.name);
            break;
        case ACTION_GET_METADATA_FROM_OBJECT:
            printf("\033[2J\033[H");
            pid_t pid_stat_file = fork();

            if (pid_stat_file == 0) {
                tty_reset();
                char *args[] = {"stat", object.name, NULL};

                execvp(args[0], args);
                _exit(1);
            }
            fflush(stdout);

            if (pid_stat_file > 0) {
                waitpid(pid_stat_file, NULL, 0);
                printf("[Midnight Club] Press any key to continue browsing\n");
                tty_setup();
                read(STDIN_FILENO,&c,1);
            }
            break;
        case ACTION_DELETE_FILE:
            printf("[Midnight Club] Press E again to remove this file\n");
            read(STDIN_FILENO,&c,1);
            if (c == TTY_E) {
                remove(object.name);
                printf("Removing file '%s\n",object.name);
            } else {
                printf("Not removing file '%s', Press any key to continue\n",object.name);
                read(STDIN_FILENO,&c,1);
            }
            break;
        case ACTION_DELETE_FOLDER:
            printf("[Midnight Club] Press E again to remove this folder\n");
            read(STDIN_FILENO,&c,1);
            if (c == TTY_E) {
                tty_reset();
                printf("Removing folder '%s\n",object.name);
                printf("\033[2J\033[H");
                pid_t pid_stat_file = fork();

                if (pid_stat_file == 0) {
                    char *args[] = {"rm","-rf","-v", object.name, NULL};

                    execvp(args[0], args);
                    _exit(1);
                }
                fflush(stdout);

                if (pid_stat_file > 0) {
                    waitpid(pid_stat_file, NULL, 0);
                    printf("[Midnight Club] Press any key to continue browsing\n");
                    tty_setup();
                    read(STDIN_FILENO,&c,1);
                }
            } else {
                printf("Not removing folder '%s', Press any key to continue\n",object.name);
                tty_setup();
                read(STDIN_FILENO,&c,1);
            }

            break;
    }
}


#endif
