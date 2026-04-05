#include "suggestion.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
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
        fgets(suggestions[i].val, MAX_SUGGESTION_SIZE, file);
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

void sort_suggestions_by_freq(suggestion suggestions[], int *n_suggestions){

    //inefficient, but ok for this case

    for(int i = 0; i < *n_suggestions - 1; i++){
        int swap_index = i;
        for(int j = i + 1; j < *n_suggestions; j++){
            if(suggestions[j].freq < suggestions[swap_index].freq){
                swap_index = j;
            }
        }

        if(swap_index != i){
            suggestion temp;
            memcpy(&temp, &suggestions[i], sizeof(suggestion));
            memcpy(&suggestions[i], &suggestions[swap_index], sizeof(suggestion));
            memcpy(&suggestions[swap_index], &temp, sizeof(suggestion));
        }
    }

};

int onlimit_delete_suggestions(suggestion suggestions[], int *n_suggestion, int n){

    suggestion *temp = malloc(MAX_SUGGESTIONS*sizeof(suggestion));


    memcpy(temp, suggestions, MAX_SUGGESTIONS*sizeof(suggestion));
    memset(suggestions, 0, MAX_SUGGESTIONS*sizeof(suggestion));

    sort_suggestions_by_freq(temp, n_suggestion);

    //for(int i = 0; i < *n_suggestion; i++) printf("%s %d\n", temp[i].val, temp[i].freq);

    *n_suggestion = 0;

    for(int i = n; i < MAX_SUGGESTIONS; i++) insert_suggestion_back(&temp[i], suggestions, n_suggestion);

    free(temp);

    return -1;
};

void insert_suggestion_back(suggestion *sugg, suggestion suggestions[], int *n_suggestions){

    int pos = *n_suggestions - 1;

    for(; pos >= 0; pos--){
        int cmp = lex_compare(suggestions[pos].val, sugg->val, MAX_SUGGESTION_SIZE);

        if(cmp == 1){
            memcpy(suggestions + pos + 1, sugg, sizeof(suggestion));
        }else break;
    }

    memcpy(suggestions + pos + 1, sugg, sizeof(suggestion));

    (*n_suggestions)++;
}

void insert_suggestion(char buff[], int buff_len, suggestion suggestions[], int *n_suggestions){


/*    if((*n_suggestions) == 0){
        strncpy(suggestions[0].val, buff, MAX_SUGGESTIONS);
        suggestions[0].freq = 1;
        (*n_suggestions)++;
        return;
    }
*/

    int s = suggestion_exists(buff, buff_len, suggestions, n_suggestions, MAX_SUGGESTION_SIZE);


    if(s != -1){
        suggestions[s].freq++;
        return;
    }

    if(*n_suggestions >= MAX_SUGGESTIONS) onlimit_delete_suggestions(suggestions, n_suggestions, SUGGESTION_DELETE_SIZE); //dodati deo za brisanje sugestija

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
