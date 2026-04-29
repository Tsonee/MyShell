#ifndef CLPARSER_H
#define CLPARSER_H

#include "util.h"

typedef enum{
    ELSE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    ESCAPE,
    SPACE,
    NEW_LINE,
    SEMICOLON
}PTAG;

typedef enum{
    UNQUOTED,
    SINGLE_QUOTED,
    DOUBLE_QUOTED,
}PSTATE;

int tokenize(char[], int , char [MAX_TOKENS][MAX_TOKEN_SIZE]);


PTAG char_to_tag(char);

#endif
