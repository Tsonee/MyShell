#ifndef COMMAND_H
#define COMMAND_H

#include "util.h"

#define EXECUTE 1

typedef struct{
    char *name;
    char **args;
    unsigned int flags;
}command;

void init_command(command *cmd, char *name, char **args, unsigned int flags){
    cmd->name = name;
    cmd->args = args;
    cmd->flags = flags;
}

#endif
