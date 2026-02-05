#include <complex.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>
#include "CTui.h"


#define MOVE(a) "\033[" #a "D"
#define DELETE_CHARS(n)  "\033[" #n "P"
#define CLEAR "\033[K"
#define BACKSPACE 127

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

char *const keywords[] = {
    "hello world",
    "hello",
    "world"
};

const int keywords_size = 3;

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

int main(int argc, char **argv){

    struct termios original;

    char c;
    char buff[256];
    char suggestion[256];
    int suggestion_len = 0;
    int cursor = 0;
    suggestion[0] = 0;
    enable_raw_mode(&original);

    while((c = getchar()) != '\n'){
        buff[cursor++] = c;
        if(c == BACKSPACE){
            cursor-=2;
        }
        buff[cursor] = 0;
        suggestion_len = suggest(buff, cursor, suggestion);
        suggestion[suggestion_len] = 0;

        printf("\r"CLEAR"%s", buff);
        if(suggestion_len) cprintf(COLOR_RED, STYLE_NONE, "%s\033[%dD", suggestion, suggestion_len);
    }

    disable_raw_mode(&original);

    char *tokens[256];

    int n = tokenize(buff, tokens);

    printf("\nTokens: ");

    for(int i = 0; i < n; i++) printf("%s ", tokens[i]);

    return 0;
}
