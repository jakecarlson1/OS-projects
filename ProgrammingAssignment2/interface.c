/**
 * interface.c
 * By: Jake Carlson
 * 2017-02-11
 * This program implements a shell interface. The user can enter commands when
 * this process is running and they will be executed. A brief history is
 * maintained that allows the user quick access to the previous 10 commands
 */

// includes
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

// maximum command length
#define MAX_LINE 80
#define HISTORY_CAPACITY 10

// history entry structure
// cmd - points to user entered command
// args - points to an array of c-string pointers to cmd arguments
struct Hist_Entry
{
    char* cmd;
    char** args;
};

// function prototypes
char** parse_args(char*);
void add_command_to_history(struct Hist_Entry*, char*, char**, int*);
void print_history(struct Hist_Entry*, int);
bool one_hist_ref(struct Hist_Entry*, int);

// main driver
int main(void)
{
    char *cmd = (char*) malloc(sizeof(char)*(MAX_LINE+1));
    struct Hist_Entry history[HISTORY_CAPACITY];
    int should_run = 1;
    int command_count = 0;
    int* count_ref = &command_count;
    pid_t pid;

    // main control loop
    while (should_run)
    {
        int should_wait = 1;

        // print prompt and flush stdout
        printf("osh>");
        fflush(stdout);
        fflush(stdin);

        // read into cmd char array
        fgets(cmd, MAX_LINE, stdin);

        // remove trailing newline character
        if (cmd[strlen(cmd)-1] == '\n')
            cmd[strlen(cmd)-1] = '\0';

        // if the command has an ampersand should_wait is set to false
        if (cmd[strlen(cmd)-1] == '&')
        {
            should_wait = 0;
            // null is placed at space before ampersand
            cmd[strlen(cmd)-2] = '\0';
        }

        // quit program
        if (cmd[0] == 'q')
        {
            should_run = 0;
        }
        // display history
        else if (strcmp(cmd, "history") == 0)
        {
            print_history(history, command_count);
        }
        else
        {
            char **args = NULL;
            if (cmd[0] == '!')
            {
                if (cmd[1] == '!')
                {
                    // run most recent command
                    args = history[command_count-1 % HISTORY_CAPACITY].args;
                }
                else if (isdigit(cmd[1]))
                {
                    // run command with entered number
                    int command_to_run = atoi(++cmd);
                    args = history[command_to_run % HISTORY_CAPACITY].args;
                }
            }
            else
            {
                // parse command into separate arguments
                args = parse_args(cmd);
            }

            // fork process
            pid = fork();
            // child process executes command
            if (pid == 0)
            {
                execvp(args[0], args);
                exit(0);
            }
            // fork failed
            else if (pid < 0)
            {
                return 1;
            }
            // if there is no ampersand the parent process waits for the child
            if (should_wait)
            {
                wait(NULL);
            }
            // command is added to history
            add_command_to_history(history, cmd, args, count_ref);
            // a new array is allocated for cmd
            cmd = (char*) malloc(sizeof(char)*(MAX_LINE+1));
        }
    }

    return 0;
}

// parses arguments out of command and returns an array of character pointers to
// the arguments in c-strings
char** parse_args(char *command)
{
    // count command arguments
    int count = 0;
    int i = 0;
    for (i = 0; command[i] != '\0'; i++)
    {
        if (command[i] == ' ')
            count++;
    }

    // allocate new space for args char* array
    char **args = (char**) malloc(sizeof(char*)*(count + 1));
    i = 0;
    int j = 0;
    int arg_index = 0;
    // iterate over command, replace spaces with NULLs and set pointer to each
    // argument
    for (i = 0; command[i] != '\0'; i++)
    {
        if (command[i] == ' ')
        {
            args[arg_index] = &command[j];
            command[i] = '\0';
            j = i + 1;
            arg_index++;
        }
    }
    // capture last arg, set final char* to NULL
    args[arg_index] = &command[j];
    args[arg_index + 1] = NULL;

    return args;
}

// adds a command to the history circular array
void add_command_to_history(struct Hist_Entry* hist, char* cmd, char** args, int* count)
{
    // if the argument count has exceeded capacity, space must be created in the
    // circular array. The memory pointed to by the history entry is freed
    if (*count >= HISTORY_CAPACITY)
    {
        // add check not to remove entry if there are two entries pointing to
        // the same cmd/args arrays
        if (one_hist_ref(hist, *count%HISTORY_CAPACITY))
        {
            free(hist[*count % HISTORY_CAPACITY].cmd);
            free(hist[*count % HISTORY_CAPACITY].args);
        }
    }

    // history entry is updated with cmd and args, count is incremented
    hist[*count % HISTORY_CAPACITY].cmd = cmd;
    hist[*count % HISTORY_CAPACITY].args = args;
    *count = *count + 1;
}

// iterates over the circular histroy array and prints entries to console
void print_history(struct Hist_Entry* hist, int count)
{
    for (int i = count-1; i >= 0 && i >= count - HISTORY_CAPACITY; i--)
    {
        printf("%i | ", i);
        fflush(stdout);

        int j = 0;
        while (hist[(i) % HISTORY_CAPACITY].args[j] != NULL)
        {
            printf("%s ", hist[(i) % HISTORY_CAPACITY].args[j]);
            j++;
        }
        printf("\n");
    }
}

// returns true if there is only one reference to the blocks of memory pointed
// to by the history entry at pos. Will only be called if the history table is
// full
bool one_hist_ref(struct Hist_Entry* hist, int pos)
{
    for(int i = 0; i < HISTORY_CAPACITY; i++)
    {
        if (i != pos && hist[i].args == hist[pos].args)
            return 0;
    }
    return 1;
}
