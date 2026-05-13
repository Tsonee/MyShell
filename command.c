#include "command.h"
#include "util.h"
#include <string.h>


void init_cmd_queue(command_queue *q){
    memset(q, 0, MAX_COMMANDS*sizeof(command));
    q->front = 0;
    q->back = 0;
}

void init_command(command *cmd, char *name, char **args, unsigned int flags){
    cmd->name = name;
    cmd->args = args;
    cmd->flags = flags;
}

void fill_cmd_queue(command_queue *q, char **tokens_ptr, int num_of_tokens){


    int j = 0;

    for(int i = 0; i < num_of_tokens; i++){
        unsigned int flags = 0;
        if(tokens_ptr[i][0] == '.') flags |= EXECUTE;

        j = i;

        while(j < num_of_tokens && tokens_ptr[j][0] != ';'){
            j++;
        }

        //if(j == num_of_tokens) break;

        tokens_ptr[j] = 0;

        if(i == j) continue;

        command cmd;
        init_command(&cmd, tokens_ptr[i], tokens_ptr + i, flags);

        cmd_queue_push(q, &cmd);

        i = j;
    }

}

int cmd_queue_is_empty(command_queue *q){
    return q->back == q->front;
}


int cmd_queue_is_full(command_queue *q){
    return q->back == MAX_COMMANDS;
}

int cmd_queue_push(command_queue *q, command *cmd){

    if(cmd_queue_is_full(q)) return -1;

    //strcpy(q->cmds[q->back].name, cmd->name);
    q->cmds[q->back].name = cmd->name;
    q->cmds[q->back].args = cmd->args;
    q->cmds[q->back].flags = cmd->flags;

    (q->back)++;

    return 0;
}

command* cmd_queue_top(command_queue *q){

    if(cmd_queue_is_empty(q)) return NULL;

    return q->cmds + q->front;
}

int cmd_queue_pop(command_queue *q){

    if(cmd_queue_is_empty(q)) return -1;

    q->front++;

    return 0;
}
