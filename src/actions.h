#ifndef MIDNIGHT_CLUB_ACTIONS_H
#define MIDNIGHT_CLUB_ACTIONS_H

#include "globals.h"
#include "tty_handle.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
/*Drilly come home*/
#define NAME_GENERAL_SIZE 1024
#define COMMAND_STRING_SIZE 1024
#define COMMAND_TOKEN_SIZE 64
#define COMMAND_TOKENS 128

char previous_command[COMMAND_STRING_SIZE];
char user_command[COMMAND_STRING_SIZE];

void tokenizer_fallback(void) {
    /*very simple tokenizer fallback*/
    printf("You seem to not have a shell. Utilizing Shell Fallback (SF) v1.0");
    int token_index = 0;
    char *tmp_token_pointers[COMMAND_TOKENS];
    tmp_token_pointers[token_index] = strtok(user_command," ");
    //printf("token: %s\n",tmp_token_pointers[token_index]);
    while (tmp_token_pointers[token_index] != NULL) {
        token_index++;
        tmp_token_pointers[token_index] = strtok(NULL," ");
        //printf("token: %s\n",tmp_token_pointers[token_index]);
    }
    tty_reset();
    pid_t pid_custom_command = fork();

    if (pid_custom_command == 0) {
        printf("[Midnight Club] Executing command.\n");
        execvp(tmp_token_pointers[0], tmp_token_pointers);
        _exit(1);
    }
    fflush(stdout);

    if (pid_custom_command > 0) {
        waitpid(pid_custom_command, NULL, 0);
        strcpy(user_command,previous_command);
        tty_setup();
    }
}

void exec_action(int action, struct filesystem_object_t object) {
    char c = '\0';
    char name[NAME_GENERAL_SIZE];
    size_t length = 0;
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

                if ( execl(executable_name, object.name, NULL) == -1) {
                    perror("execl");
                    printf("[Midnight Club] Invalid program\n");
                }
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
            printf("\033[2J\033[H");
            fflush(stdout);
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
        case ACTION_CREATE_FILE:
            printf("\033[2J\033[H");
            fflush(stdout);

            tty_reset();

            printf("[Midnight Club] Creating a new file. Select file name\n");
            if (read(STDIN_FILENO,name,NAME_GENERAL_SIZE) < 0) {
                perror("read");
                exit(1);
            }

            size_t length = strlen(name);
            name[strcspn(name, "\n")] = '\0';
            FILE *open_file = fopen(name,"wx");

            if (open_file == NULL) {
                perror("fopen");
                tty_setup();
                printf("Press any key to continue\n");
                read(STDIN_FILENO,&c,1);
            } else {
                fclose(open_file);
            }

            tty_setup();

            break;
        case ACTION_CREATE_FOLDER:
            printf("\033[2J\033[H");
            fflush(stdout);

            tty_reset();

            printf("[Midnight Club] Creating a new folder. Select folder name\n");
            if (read(STDIN_FILENO,name,NAME_GENERAL_SIZE) < 0) {
                perror("read");
                exit(1);
            }

            length = strlen(name);
            name[strcspn(name, "\n")] = '\0';

            if (mkdir(name,0755) != 0) {
                tty_setup();
                perror("mkdir");
                printf("Press any key to continue\n");
                read(STDIN_FILENO,&c,1);
            }

            tty_setup();

            break;
        case ACTION_RENAME_OBJECT:
            printf("\033[2J\033[H");
            fflush(stdout);

            tty_reset();

            printf("[Midnight Club] Renaming filesystem object. Type in name.\n");
            if (read(STDIN_FILENO,name,NAME_GENERAL_SIZE) < 0) {
                perror("read");
                exit(1);
            }

            length = strlen(name);
            name[strcspn(name, "\n")] = '\0';

            rename(object.name,name);

            tty_setup();

            break;

        case ACTION_SYS_COMMAND_PREVIOUS:
            printf("\033[2J\033[H");
            tty_reset();
            printf("[Midnight Club] Executing previous shell command\n");

            strcpy(previous_command,user_command);

            if (system(NULL) != 0) {
                system(user_command);
            }

            strcpy(user_command,previous_command);

            fflush(stdout);
            tty_setup();
            printf("[Midnight Club] Your command has completed running, Press anything to continue browsing\n");
            read(STDIN_FILENO,&c,1);
            break;
        case ACTION_SYS_SHELL_COMMAND:
            tty_reset();

            printf("[Midnight Club] Type your shell command.\n");
            read(STDIN_FILENO,user_command,sizeof(user_command));
            user_command[strcspn(user_command,"\n")] = '\0';
            printf("[Midnight Club] Executing shell command\n");

            strcpy(previous_command,user_command);

            if (system(NULL) != 0) {
                system(user_command);
            }

            strcpy(user_command,previous_command);

            fflush(stdout);
            tty_setup();
            printf("[Midnight Club] Your command has completed running, Press anything to continue browsing\n");
            read(STDIN_FILENO,&c,1);

            break;
    }
}


#endif
