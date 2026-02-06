#include "suggestion.h"

char *const keywords[] = {
    "hello",
    "world",
    "hello world"
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
