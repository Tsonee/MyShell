#include "CLParser.h"

PTAG char_to_tag(char c){
    switch (c) {
        case '\'': return SINGLE_QUOTE;
        case '\"': return DOUBLE_QUOTE;
        case '\\': return ESCAPE;
        case ' ': return SPACE;
        case '\n': return NEW_LINE;
        case ';': return SEMICOLON;
        case '|': return PIPE;

        default: return ELSE;
    }
}


int tokenize(char buff[], int buff_cursor, char tokens[MAX_TOKENS][MAX_TOKEN_SIZE]){
    int num_of_tokens = 0;
    int token_len = 0;
    PSTATE state = UNQUOTED;
    PTAG tag;
    int escaped = 0;


    for(int i = 0; i <= buff_cursor; i++){
        tag = char_to_tag(buff[i]);
        if(tag == ESCAPE && !escaped){
            escaped = 1;
            continue;
        }

        switch (state) {
            case UNQUOTED:
                if(escaped) escaped = 0;
                else if(tag == SEMICOLON){
                    if(token_len){
                        tokens[num_of_tokens][token_len] = 0;
                        num_of_tokens++;
                        token_len = 0;
                    }

                    tokens[num_of_tokens][token_len++] = ';';
                    tokens[num_of_tokens][token_len] = 0;
                    num_of_tokens++;
                    token_len = 0;
                }else if(tag == PIPE){
                    if(token_len){
                        tokens[num_of_tokens][token_len] = 0;
                        num_of_tokens++;
                        token_len = 0;
                    }

                    tokens[num_of_tokens][token_len++] = '|';
                    tokens[num_of_tokens][token_len] = 0;
                    num_of_tokens++;
                    token_len = 0;
                }
                else if(tag == SINGLE_QUOTE) state = SINGLE_QUOTED;
                else if(tag == DOUBLE_QUOTE) state = DOUBLE_QUOTED;
                else if(tag == SPACE || tag == NEW_LINE){
                    if(token_len){
                        tokens[num_of_tokens][token_len] = 0;
                        num_of_tokens++;
                        token_len = 0;
                    }
                }else tokens[num_of_tokens][token_len++] = buff[i];
            break;
            case SINGLE_QUOTED:
                if(escaped){
                    tokens[num_of_tokens][token_len++] = '\\';
                    tokens[num_of_tokens][token_len++] = buff[i];
                    escaped = 0;
                }
                else if(tag == SINGLE_QUOTE) state = UNQUOTED;
                else if(tag == NEW_LINE){
                    tokens[num_of_tokens][token_len] = 0;
                    num_of_tokens++;
                }else tokens[num_of_tokens][token_len++] = buff[i];
            break;
            case DOUBLE_QUOTED:
                if(escaped){
                    switch (buff[i]) {
                        case '\"': case '\\': case '$': case '`': tokens[num_of_tokens][token_len++] = buff[i];
                        break;
                        default:
                            tokens[num_of_tokens][token_len++] = '\\';
                            tokens[num_of_tokens][token_len++] = buff[i];
                    }
                    escaped = 0;
                }
                else if(tag == DOUBLE_QUOTE) state = UNQUOTED;
                else if(tag == NEW_LINE){
                    tokens[num_of_tokens][token_len] = 0;
                    num_of_tokens++;
                }else tokens[num_of_tokens][token_len++] = buff[i];
            break;


        }

    }

    return num_of_tokens;
}
