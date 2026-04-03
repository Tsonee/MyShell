#include "util.h"
#include "suggestion.h"
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

void get_home_usr(char exp[MAX_TOKEN_SIZE]){
    char usr[MAX_TOKEN_SIZE];
    get_current_username(usr);
    snprintf(exp, MAX_TOKEN_SIZE, "/home/%s", usr);
}

void expand(char tokens[MAX_TOKENS][MAX_TOKENS], int n){

    char exp[MAX_TOKEN_SIZE];
    get_home_usr(exp);

    for(int i = 0; i < n; i++){
        if(tokens[i][0] == '~'){
            char temp[MAX_TOKEN_SIZE];
            strncpy(temp, tokens[i] + 1, MAX_TOKEN_SIZE - 1);
            snprintf(tokens[i], MAX_TOKEN_SIZE, "%s%s", exp, temp);
        }
    }
}
