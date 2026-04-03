#include "suggestion.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

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
        suggestions[i].val[strlen(suggestions[i].val) - 1] = 0;
        //fscanf(file, "%s %d\n", &suggestions[i].val, &suggestions[i].freq);
    }

    fclose(file);
    return 1;
}

int close_suggestion_file(suggestion suggestions[MAX_SUGGESTIONS], int *n_suggestions){
    char usr[MAX_TOKEN_SIZE];
    get_current_username(usr);
    char file_path[MAX_TOKEN_SIZE];

    snprintf(file_path, MAX_TOKEN_SIZE, "/home/%s/%s", usr, SUGGESTION_FILE_NAME);

    FILE *file = fopen(file_path, "w");

    if(file == NULL) return -1;

    for(int i = 0; i < *n_suggestions; i++) printf("%s->%d ", suggestions[i].val, suggestions[i].freq); printf("\n");

    fprintf(file, "%d\n", *n_suggestions);
    for(int i = 0; i < *n_suggestions; i++)
        fprintf(file, "%d %s\n", suggestions[i].freq, suggestions[i].val);

    fclose(file);

    return 1;
}

//file -> buff -> .... -> file
int suggest(char buffer[], int cursor, char **sugg, suggestion suggestions[], int *n_suggestions){
    if(cursor == 0){
        *sugg = NULL;
        return 0;
    }

    int i = suggestion_exists(buffer, cursor, suggestions, n_suggestions, cursor - 1);

    if(i == -1){
        *sugg = NULL;
        return 0;
    }

    *sugg = suggestions[i].val + cursor;


    return strlen(*sugg);
}

// 1 -> prvi veci, 0 isti, -1 priv manji
int lex_compare(char *str1, char *str2, int n){

    int i = 0;

    while(i < n && str1[i] != 0 && str2[i] != 0 && str1[i] == str2[i]) i++;

    if(str1[i] > str2[i]) return 1;
    else if(str2[i] > str1[i]) return -1;
    else return 0;
}

int suggestion_exists(char buff[], int buff_len, suggestion suggestions[], int *n_suggestions, int compare_len){

    int l = 0, r = *n_suggestions - 1, mid;
    int cmp;

    while(l <= r){
        mid = l + (r - l)/2;

        cmp = lex_compare(suggestions[mid].val, buff, compare_len);

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

    if((*n_suggestions) == 0){
        strncpy(suggestions[0].val, buff, MAX_SUGGESTIONS);
        suggestions[0].freq = 1;
        (*n_suggestions)++;
        return;
    }

    int s = suggestion_exists(buff, buff_len, suggestions, n_suggestions, MAX_SUGGESTION_SIZE);

    if(s != -1){
        suggestions[s].freq++;
        return;
    }

    int pos = *n_suggestions - 1;

    for(; pos >= 0; pos--){
        int cmp = lex_compare(suggestions[pos].val, buff, MAX_SUGGESTION_SIZE);

        if(cmp == 1){
            strncpy(suggestions[pos + 1].val, suggestions[pos].val, MAX_SUGGESTION_SIZE);
            //printf("sug[pos + 1]=%s sug[pos]=%s\n", suggestions[pos+1].val, suggestions[pos].val);
            suggestions[pos+1].freq = suggestions[pos].freq;
        }else break;
    }

    strncpy(suggestions[pos + 1].val, buff, MAX_SUGGESTION_SIZE);
    suggestions[pos + 1].freq = 1;

    (*n_suggestions)++;
};
