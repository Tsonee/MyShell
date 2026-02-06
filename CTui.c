#include "CTui.h"

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
