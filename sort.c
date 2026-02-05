#include <stdio.h>
#include "CTui.h"
#include <stdlib.h>

int main(int argc, char **argv){

    if(argc < 2){
        fprintf(stderr, "usage: sort <array>\n");
        return 1;
    }


    int len = argc - 1;

    int a[len];

    for(int i = 1; i < argc; i++) a[i - 1] = atoi(argv[i]);

    for(int i = 0; i < len - 1; i++){
        for(int j = i + 1; j < len; j++){
            if(a[i] > a[j]){
                int temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }

    for(int i = 0; i < len; i++) cprintf(COLOR_RED, STYLE_NONE, "%d ", a[i]);

    printf("\n");

    return 0;
}
