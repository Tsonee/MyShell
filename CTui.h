#ifndef CTUI_H
#define CTUI_H

#include <stdio.h>
#include <stdarg.h>

#define COLOR_RESET "\033[0m"

typedef enum{
 COLOR_BLACK ,
 COLOR_RED ,
 COLOR_GREEN ,
 COLOR_YELLOW ,
 COLOR_BLUE ,
 COLOR_PURPLE ,
 COLOR_CYAN ,
 COLOR_WHITE,
}COLOR;

typedef enum style{
    STYLE_NONE = '0',
    STYLE_BOLD = '1',
    STYLE_UNDERLINED = '4'
}STYLE;

char colors[8][8] = {"\e[0;30m", "\e[0;31m", "\e[0;32m", "\e[0;33m", "\e[0;34m", "\e[0;35m", "\e[0;36m", "\e[0;37m"};

int cprintf(COLOR color, STYLE style, const char *fmt, ...){

    char *clr = colors[color];
    clr[2] = style;

    printf("%s", clr);

    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);


    printf("%s", COLOR_RESET);
}

void prompt(char *prompt_text, COLOR prompt_color, char *prompt_tag, COLOR tag_color, int nl){

    cprintf(prompt_color, STYLE_NONE, prompt_text);

    if(nl) printf("\n");

    cprintf(tag_color, STYLE_NONE, prompt_tag);

}


#endif
