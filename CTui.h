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

extern char colors[8][8];

int cprintf(COLOR, STYLE , const char *, ...);

void prompt(char *, COLOR , char *, COLOR, int );


#endif
