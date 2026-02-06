#include "util.h"
#include <stdio.h>
#include <string.h>

void get_current_username(char user[]) {
    struct passwd *pw = getpwuid(getuid());

    if(pw){
        strncpy(user, pw->pw_name, LOGIN_NAME_MAX);
    }else{
        printf("Cannot get the current user!\n");
        exit(1);
    }
}

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


int tokenize(char buff[], int buff_cursor, char *tokens[]){

    /*int i = 0;
    char *token = strtok(str, " ");
    while(token){
        tokens[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    */

    char *token;
    int token_len = 0;
    int num_of_tokens = 0;
    int in_single_quote = 0;
    int in_double_quote = 0;

    for(int i = 0; i <= buff_cursor; i++){
// "tok"t t1 "cool c.c"
        if(buff[i] == '\'' && !in_double_quote){
            in_single_quote = !in_single_quote;
            if(i+1 <= buff_cursor && buff[i+1] == ' ' || buff[i+1] == 0) buff[i] = 0;
            continue;
        }

        if(buff[i] == '\"' && !in_single_quote){
            in_double_quote = !in_double_quote;
            if(i+1 <= buff_cursor && buff[i+1] == ' ' || buff[i+1] == 0) buff[i] = 0;
            continue;
        }

        if(buff[i] == ' ' && !in_single_quote && !in_double_quote){
            if(!token) continue;
            token[token_len] = 0;
            tokens[num_of_tokens] = token;
            num_of_tokens++;
            token_len = 0;
            token = NULL;
            continue;
        }else if (!token) {
            token = buff + i;
        }

        token_len++;
    }

    if(token){
        token[token_len] = 0;
        tokens[num_of_tokens] = token;
        num_of_tokens++;
    }

    return num_of_tokens;
}

int traverse_directory(const char *path, const char *target_name) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Cannot open directory");
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Construct full path
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Check if it's a file and matches the name~
        struct stat statbuf;
        if (stat(full_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
            if (strcmp(entry->d_name, target_name) == 0) {
                return 1;
            }
        }

        if (S_ISDIR(statbuf.st_mode)) {
            continue;
        }
    }

    return 0;
}
