#include <termios.h>
#include "CTui.h"
#include "util.h"
#include "suggestion.h"

#define BUFF_SIZE 256
#define BACKSPACE 127
#define CLEAR "\033[K"
#define CMD_BASE_PATH "/usr/bin/"
#define HOME_PATH "/home/"
#define MAX_PATH 512

int main(){
    struct termios original;

    int command_mode = 0;
    int ready_to_exec = 0;
    char buff[BUFF_SIZE + 1];//zbog tokenizacije, terminirajuca nula
    int buff_cursor = 0;
    char suggestion[BUFF_SIZE + 1];
    int suggestion_len;
    char *tokens[MAX_TOKENS];
    char c;
    char curr_working_dir[MAX_PATH + 1];
    char user[LOGIN_NAME_MAX + 1];

    get_current_username(user);

    if(getcwd(curr_working_dir, sizeof(curr_working_dir)) == NULL){
        printf("ERROR opening curr_working_dir!!!\n");
        return 1;
    }

    for(;;){ // ovaj loop nije los

        if(!command_mode && !ready_to_exec){
            char pmt[BUFF_SIZE];
            sprintf(pmt, "At your service master %s -> %s", user, curr_working_dir);
            prompt(pmt, COLOR_YELLOW, "-> ", COLOR_CYAN, 1);
            command_mode = 1;
            enable_raw_mode(&original);
        }

        if(command_mode && !ready_to_exec){ //msm to je isti djavo
            c = getchar();
            if(c == '\n'){
                disable_raw_mode(&original);
                if(buff_cursor) ready_to_exec = 1;
                buff[buff_cursor] = 0;
                command_mode = 0;
                printf("\n");//nakon izlaska iz raw moda zato sto nije stigao da ispise u buffer
                continue;
            }

            if(c == BACKSPACE){
                if(buff_cursor >= 1) buff_cursor--;
            }else buff[buff_cursor++] = c;

            buff[buff_cursor] = 0;
            suggestion_len = suggest(buff, buff_cursor, suggestion);
            suggestion[suggestion_len] = 0;

            printf("\r"CLEAR); //ocisti ekran
            prompt(0,0, "-> ", COLOR_CYAN, 0); // izbaci prompt
            printf("%s", buff); //ispise buffer
            //if(suggestion_len) cprintf(COLOR_RED, STYLE_NONE, "%s\033[%dD", suggestion, suggestion_len); // ako ima sugestija onda je ispisi i pomeri kursor na levo
            if(suggestion_len) cprintf(COLOR_RED, STYLE_NONE, "%s\033[%dD", suggestion, suggestion_len); // ako ima sugestija onda je ispisi i pomeri kursor na levo
            continue;
        };

        int n = tokenize(buff, buff_cursor, tokens);
        tokens[n] = NULL;
        buff_cursor = 0;

//        printf("Tokens: ");
//        for(int i = 0; tokens[i] != NULL; i++) printf("%s ", tokens[i]);
//        printf("\n");

        if(strncmp(tokens[0], "exit", BUFF_SIZE) == 0) break;
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
            ready_to_exec = 0;
            continue;
        }

        int pid = fork();

        if(pid == 0){

            if(!traverse_directory("/usr/bin/", tokens[0])){
                printf("Unknown command!\n");
                ready_to_exec = 0;
                return 1;
            }

            char cmd_to_exec[sizeof(CMD_BASE_PATH) + strlen(tokens[0])];
            strncpy(cmd_to_exec, CMD_BASE_PATH, sizeof(CMD_BASE_PATH));
            strncat(cmd_to_exec, tokens[0], sizeof(CMD_BASE_PATH) + strlen(tokens[0]));

            execv(cmd_to_exec, tokens);
            printf("Error while execing!!!\n");
            ready_to_exec = 0;
            return 1;
        }else{
            wait(0);
            memset(buff, 0, BUFF_SIZE);
            buff_cursor = 0;
            ready_to_exec = 0;
        }

    }

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
