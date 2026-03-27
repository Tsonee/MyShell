#include "suggestion.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

char *const keywords[] = {
    "hello",
    "world",
    "hello world"
};

const int keywords_size = 3;

int open_suggestion_file(suggestion suggestions[MAX_SUGGESTIONS], int *n_suggestions){
    char usr[MAX_TOKEN_SIZE];
    get_current_username(usr);
    char file_path[MAX_TOKEN_SIZE];

    snprintf(file_path, MAX_TOKEN_SIZE, "/home/%s/%s", usr, SUGGESTION_FILE_NAME);

    FILE *file = fopen(file_path, "r");

    if(file == NULL){
        file = fopen(file_path, "w");
        fprintf(file,"0\n");
        fclose(file);
        return 1;
    }

    fscanf(file, "%d\n", n_suggestions);

    for(int i = 0; i < *n_suggestions; i++){
        //printf("%s debug!!\n", file_path);
        fscanf(file, "%d ", &suggestions[i].freq);
        fgets(suggestions[i].val, MAX_SUGGESTIONS, file);
        //fscanf(file, "%s %d\n", &suggestions[i].val, &suggestions[i].freq);
    }

    fclose(file);
    return 1;
}

int close_suggestion_file(suggestion suggestions[MAX_SUGGESTIONS], int *n_suggestions){
    printf("sug[0] = %s\n", suggestions[0].val);
    char usr[MAX_TOKEN_SIZE];
    get_current_username(usr);
    char file_path[MAX_TOKEN_SIZE];

    snprintf(file_path, MAX_TOKEN_SIZE, "/home/%s/%s", usr, SUGGESTION_FILE_NAME);

    FILE *file = fopen(file_path, "w");

    if(file == NULL) return -1;

    fprintf(file, "%d\n", *n_suggestions);
    for(int i = 0; i < *n_suggestions; i++)
        fprintf(file, "%d %s\n", suggestions[i].freq, suggestions[i].val);

    fclose(file);

    return 1;
}

//file -> buff -> .... -> file
int suggest(char buffer[], int cursor, char suggestion[]){
    if(cursor == 0){
        suggestion[0] = 0;
        return 0;
    }

    /*int len_to_copy = 0;

    for(int i = 0; i < keywords_size; i++){
        if(strncmp(buffer, keywords[i], cursor) == 0){
            len_to_copy = strlen(keywords[i]) - cursor;
            strncpy(suggestion, keywords[i] + cursor, len_to_copy);
            break;
        }
    }*/

    return 0;
}

// 1 -> prvi veci, 0 isti, -1 priv manji
int lex_compare(char *str1, char *str2){

    int i = 0;

    while(str1[i] != 0 && str2[i] != 0 && str1[i] == str2[i]) i++;

    if(str1[i] > str2[i]) return 1;
    else if(str2[i] > str1[i]) return -1;
    else return 0;
}

int suggestion_exists(char buff[], int buff_len, suggestion suggestions[], int *n_suggestions){

    int l = 0, r = *n_suggestions - 1, mid;
    int cmp;

    while(l < r){
        mid = l + (r - l)/2;

        cmp = lex_compare(suggestions[mid].val, buff);

        if(cmp == 0) return mid;

        if(cmp == 1){
            r = mid - 1;
        }else{
            l = mid + 1;
        }
    }

    return -1;
}

void insert_suggestion(char buff[], int buff_len, suggestion suggestions[], int *n_suggestions){

    if(*n_suggestions > MAX_SUGGESTIONS) return; //dodati deo za brisanje sugestija

    if(*n_suggestions == 0){
        strncpy(suggestions[0].val, buff, MAX_SUGGESTIONS);
        suggestions[0].freq = 1;
        (*n_suggestions)++;
        return;
    }

    int s = suggestion_exists(buff, buff_len, suggestions, n_suggestions);

    if(s != -1){
        suggestions[s].freq++;
        return;
    }

    int pos = *n_suggestions-1;

    for(; pos >= 0; pos--){
        int len = strlen(suggestions[pos].val);
        // -> break
        int cmp = lex_compare(suggestions[pos].val, buff);

        if(cmp == 1){
            strncpy(suggestions[pos + 1].val, suggestions[pos].val, MAX_SUGGESTIONS);
            suggestions[pos + 1].freq = suggestions[pos].freq;
        }else{
            strncpy(suggestions[pos + 1].val, buff, MAX_SUGGESTIONS);
            suggestions[pos + 1].freq = 1;
            break;
        }

    }

    (*n_suggestions)++;
};
