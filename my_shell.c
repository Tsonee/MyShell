#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <termios.h>
#include "CTui.h"

#define BUFF_SIZE 256
#define MAX_TOKENS 256
#define BACKSPACE 127
#define CLEAR "\033[K"

char *const keywords[] = {
    "hello",
    "world",
    "hello world"
};

const int keywords_size = 3;

void enable_raw_mode(struct termios *original){
    struct termios raw;
    tcgetattr(STDIN_FILENO, original);
    raw = *original;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disable_raw_mode(struct termios *original){
    tcsetattr(STDIN_FILENO, TCSANOW, original);
}


int tokenize(char *str, char **tokens){

    int i = 0;
    char *token = strtok(str, " ");
    while(token){
        tokens[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    return i;
}

int suggest(char buffer[], int cursor, char suggestion[]){

    if(cursor == 0){
        suggestion[0] = 0;
        return 0;
    }

    int len_to_copy = 0;

    for(int i = 0; i < keywords_size; i++){
        if(strncmp(buffer, keywords[i], cursor) == 0){
            len_to_copy = strlen(keywords[i]) - cursor;
            strncpy(suggestion, keywords[i] + cursor, len_to_copy);
            break;
        }
    }

    return len_to_copy;
}

int file_contains(FILE *file, char *line){

}

void write_command(char *file_path,char command[], int len){
    FILE *file = fopen(file_path, "a+");

    fclose(file);
}

int main(){

    struct termios original;

    int command_mode = 0;
    int ready_to_exec = 0;
    char buff[BUFF_SIZE];
    int buff_cursor = 0;
    char suggestion[BUFF_SIZE];
    int suggestion_len;
    char *tokens[MAX_TOKENS];
    char c;


    for(;;){

        if(!command_mode && !ready_to_exec){
            prompt("At your service", COLOR_YELLOW, "-> ", COLOR_CYAN, 1);
            command_mode = 1;
            enable_raw_mode(&original);
        }

        if(command_mode && !ready_to_exec){
            c = getchar();

            if(c == '\n'){
                disable_raw_mode(&original);
                buff_cursor = 0;
                command_mode = 0;
                ready_to_exec = 1;
                printf("\n");//nakon izlaska iz raw moda zato sto nije stigao da ispise u buffer
                continue;
            }

            buff[buff_cursor++] = c;
            if(c == BACKSPACE){
                buff_cursor-=2;
            }
            buff[buff_cursor] = 0;
            suggestion_len = suggest(buff, buff_cursor, suggestion);
            suggestion[suggestion_len] = 0;

            printf("\r"CLEAR); //ocisti ekran
            prompt(0,0, "-> ", COLOR_CYAN, 0); // izbaci prompt
            printf("%s", buff); //ispise buffer
            if(suggestion_len) cprintf(COLOR_RED, STYLE_NONE, "%s\033[%dD", suggestion, suggestion_len); // ako ima sugestija onda je ispisi i pomeri kursor na levo
            continue;
        };

        int n = tokenize(buff, tokens);

        tokens[n] = NULL;

        printf("Tokens: ");
        for(int i = 0; tokens[i] != NULL; i++) printf("%s ", tokens[i]);
        printf("\n");

        int pid = fork();

        if(pid == 0){
            if(strncmp(tokens[0], "print", 256) == 0) execv("print", tokens);
            else if(strncmp(tokens[0], "sort", 256) == 0) execv("sort", tokens);
            else if(strncmp(tokens[0], "neofetch", 256) == 0){
                tokens[0] = "/usr/bin/neofetch";
                execv("/usr/bin/neofetch", tokens);
                printf("EXECV: error!\n");
                return 1;
            }
            else if(strncmp(tokens[0], "exit", 256) == 0) break;
            ready_to_exec = 0;
        }else{
            wait(0);
            if(strncmp(tokens[0], "exit", 256) == 0){
                break;
            }
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
