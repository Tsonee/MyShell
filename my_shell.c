#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <termios.h>
#include "CTui.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define BUFF_SIZE 256
#define MAX_TOKENS 256
#define BACKSPACE 127
#define CLEAR "\033[K"
#define CMD_BASE_PATH "/usr/bin/"
#define MAX_PATH 512

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

        // Check if it's a file and matches the name
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
void handle_home_dir(char path[]){

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
    char curr_working_dir[MAX_PATH];

    if(getcwd(curr_working_dir, sizeof(curr_working_dir)) == NULL){
        printf("ERROR opening curr_working_dir!!!\n");
        return 1;
    }

    for(;;){ // ovaj loop nije los

        if(!command_mode && !ready_to_exec){
            prompt(curr_working_dir, COLOR_YELLOW, "-> ", COLOR_CYAN, 1);
            command_mode = 1;
            enable_raw_mode(&original);
        }

        if(command_mode && !ready_to_exec){ //msm to je isti djavo
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
            if(c == BACKSPACE && buff_cursor>1){
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
