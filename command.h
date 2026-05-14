#ifndef COMMAND_H
#define COMMAND_H

#include "util.h"

#define EXECUTE (1 << 0)
#define PIPE_IN (1 << 1)
#define PIPE_OUT (1 << 2)

#define MAX_COMMANDS 256

typedef struct{
    char *name;
    char **args;
    unsigned int flags;
}command;

typedef struct{
    command cmds[MAX_COMMANDS];
    int front;
    int back;
}command_queue;

void init_command(command *, char *, char **, unsigned int);

void fill_cmd_queue(command_queue *cmd_queue, char **, int);

int cmd_queue_is_empty(command_queue *);
int cmd_queue_is_full(command_queue *);
int cmd_queue_push(command_queue *, command *);
command* cmd_queue_top(command_queue *);
int cmd_queue_pop(command_queue *);
void cmd_queue_clear(command_queue *);

#endif
