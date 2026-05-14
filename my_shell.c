#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include "CTui.h"
#include "util.h"
#include "suggestion.h"
#include "CLParser.h"
#include "command.h"

#define BUFF_SIZE 1024
#define BACKSPACE 127
#define TAB 9
#define CLEAR "\033[K"
#define CMD_BASE_PATH "/usr/bin/"
#define HOME_PATH "/home/"
#define MAX_PATH 512


#define DEBUG_CLARGS

int main(){
    struct termios original;

    int command_mode = 0;
    int ready_to_exec = 0;
    char buff[BUFF_SIZE + 1];//zbog tokenizacije, terminirajuca nula
    int buff_cursor = 0;
    char *sugg = NULL;
    int suggestion_len = 0;
    char tokens[MAX_TOKENS][MAX_TOKEN_SIZE];
    char c;
    char curr_working_dir[MAX_PATH + 1];
    char user[LOGIN_NAME_MAX + 1];
    suggestion *suggestions = NULL;
    suggestions = malloc(sizeof(suggestion) * MAX_SUGGESTIONS);
    if(suggestions == NULL) return -1;
    int n_suggestions = 0;
    command_queue *cmd_queue = (command_queue *)malloc(sizeof(command_queue));

    get_current_username(user);

    if(getcwd(curr_working_dir, sizeof(curr_working_dir)) == NULL){
        printf("ERROR opening curr_working_dir!!!\n");
        return 1;
    }
    open_suggestion_file(suggestions, &n_suggestions);

    for(;;){ // ovaj loop nije los

        if(!command_mode && !ready_to_exec){
            char pmt[BUFF_SIZE];
            sprintf(pmt, "At your service master %s -> %s", user, curr_working_dir);
            prompt(pmt, COLOR_YELLOW, "-> ", COLOR_CYAN, 1);
            command_mode = 1;
            enable_raw_mode(&original);
        }

        //printf("BUFF CURSOR!: %d\n", buff_cursor);

        if(command_mode && !ready_to_exec){ //msm to je isti djavo
            c = getchar();
            if(c == '\n'){
                disable_raw_mode(&original);
                if(buff_cursor) ready_to_exec = 1;
                buff[buff_cursor] = '\n';
                command_mode = 0;
                printf("\n");//nakon izlaska iz raw moda zato sto nije stigao da ispise u buffer
                continue;
            }

            if(c == BACKSPACE){
                if(buff_cursor >= 1) buff_cursor--;
            }else if(c == TAB && sugg){
                strncpy(buff + buff_cursor, sugg, strlen(sugg));
                buff_cursor+=strlen(sugg);
                sugg = NULL;
                suggestion_len = 0;
                c = 0;
            }
            else buff[buff_cursor++] = c;

            buff[buff_cursor] = 0;
            suggestion_len = suggest(buff, buff_cursor, &sugg, suggestions, &n_suggestions);

            printf("\r"CLEAR); //ocisti ekran
            prompt(0,0, "-> ", COLOR_CYAN, 0); // izbaci prompt
            printf("%s", buff); //ispise buffer
            //if(suggestion_len) cprintf(COLOR_RED, STYLE_NONE, "%s\033[%dD", suggestion, suggestion_len); // ako ima sugestija onda je ispisi i pomeri kursor na levo
            if(suggestion_len) cprintf(COLOR_RED, STYLE_NONE, "%s\033[%dD", sugg, suggestion_len); // ako ima sugestija onda je ispisi i pomeri kursor na levo
            continue;
        }

        int flags = 0;
        int n = tokenize(buff, buff_cursor, tokens);
        char *tokens_ptr[MAX_TOKENS + 1];
        buff[buff_cursor] = 0;
        insert_suggestion(buff, buff_cursor, suggestions, &n_suggestions);
        expand(tokens, n);
        for(int i = 0; i < n; i++) tokens_ptr[i] = tokens[i];
        cmd_queue_clear(cmd_queue);
        fill_cmd_queue(cmd_queue, tokens_ptr, n);
        if(cmd_queue_is_empty(cmd_queue)){
            ready_to_exec = 0;
            buff_cursor = 0;
            continue;
        }
        //if(cmd.name[0] == '.') cmd.flags |= EXECUTE;


#ifdef DEBUG_CLARGS
        printf("tokens: ");
        for(int i = 0; i <= n; i++) printf("%s ", tokens_ptr[i]);
        printf("\n");
#endif
        tokens_ptr[n] = NULL;
        buff_cursor = 0;

//        printf("Tokens: ");
//        for(int i = 0; tokens[i] != NULL; i++) printf("%s ", tokens[i]);
//        printf("\n");

        if(strncmp(tokens[0], "exit", BUFF_SIZE) == 0){
            insert_suggestion(buff, buff_cursor, suggestions, &n_suggestions);
            break;
        }
        else if(strncmp(tokens[0], "cd", BUFF_SIZE) == 0){
            if(n != 2){
                printf("Wrong usage of cd!\n");
                ready_to_exec = 0;
                continue;
            }
            char temp[MAX_PATH];
            snprintf(temp, MAX_PATH, "%s/%s", curr_working_dir, tokens[1]);
            if(chdir(temp) != 0 && chdir(tokens[1]) != 0){
                printf("Unknown directory!\n");
                ready_to_exec = 0;
                continue;
            }


            if(getcwd(curr_working_dir, sizeof(curr_working_dir)) == NULL){
                printf("ERROR opening curr_working_dir!!!\n");
                return 1;
            }
            insert_suggestion(buff, buff_cursor, suggestions, &n_suggestions);
            ready_to_exec = 0;
            continue;
        }

        int prev_p[2] = {-1, -1};
        int next_p[2] = {-1, -1};
        // ls | wc
        while(!cmd_queue_is_empty(cmd_queue)){
             command *curr_cmd = cmd_queue_top(cmd_queue);
             cmd_queue_pop(cmd_queue);


            if(curr_cmd->flags & PIPE_IN){
                if(prev_p[0] != -1) close(prev_p[0]);
                prev_p[0] = next_p[0];
                close(next_p[1]);
            }

            if(curr_cmd->flags & PIPE_OUT){
                pipe(next_p);
            }

            int pid = fork();

            if(pid == 0){

                if(curr_cmd->flags & PIPE_OUT){
                    close(1);
                    dup(next_p[1]);
                }
                if(curr_cmd->flags & PIPE_IN){
                    close(0);
                    dup(prev_p[0]);
                }

                if(curr_cmd->flags & EXECUTE){
                execv(curr_cmd->name, curr_cmd->args);
                printf("Error: exec failed!\n");
                exit(-1);
                };

                if(!traverse_directory(CMD_BASE_PATH, tokens[0])){
                    printf("Unknown command!\n");
                    ready_to_exec = 0;
                    return 1;
                }

                char cmd_to_exec[sizeof(CMD_BASE_PATH) + strlen(curr_cmd->name)];
                strncpy(cmd_to_exec, CMD_BASE_PATH, sizeof(CMD_BASE_PATH));
                strncat(cmd_to_exec, curr_cmd->name, sizeof(CMD_BASE_PATH) + strlen(curr_cmd->name));

#ifdef DEBUG_CLARGS
                printf("cmd_to_exec: %s\n", cmd_to_exec);
                for(int i = 0; curr_cmd->args[i] != 0; i++) printf("%s ", curr_cmd->args[i]);
                printf("\n");
#endif

                execv(cmd_to_exec, curr_cmd->args);
                printf("Error while execing!!!\n");
                ready_to_exec = 0;
                return 1;
            }
        }

        while(wait(0) > 0);
        close(next_p[1]);
        close(prev_p[0]);

        memset(buff, 0, BUFF_SIZE);
        buff_cursor = 0;
        ready_to_exec = 0;
        if(prev_p[0] != -1 && prev_p[1] != -1){
           close(prev_p[0]);
           close(prev_p[1]);
        }

    }

    close_suggestion_file(suggestions, &n_suggestions);
    free(cmd_queue);
    free(suggestions);

    return 0;
}

/*int main(){

    char str[] = "print 1 2 3";
    char *tokens[20];

    int n = tokenize(str, tokens);

    for(int i = 0; i < n; i++){
        printf("Token %d: %s\n", i, tokens[i]);
    }

    return 0;
}
*/
