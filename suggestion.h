#ifndef SUGGESTION_H
#define SUGGESTION_H

#include "util.h"
#include <string.h>

#define SUGGESTION_FILE_NAME ".suggestions"

#define MAX_SUGGESTIONS 10001

#define MAX_SUGGESTION_SIZE 1024

#define SUGGESTION_DELETE_SIZE 2000

typedef struct{
    int freq;
    char val[MAX_SUGGESTION_SIZE];
}suggestion;

extern char *const keywords[];

extern const int keywords_size;

int suggest(char [], int , char **, suggestion[], int *);

int open_suggestion_file(suggestion [], int *);

void insert_suggestion(char [], int, suggestion[], int *);

void insert_suggestion_back(suggestion *, suggestion [], int *);

int suggestion_exists(char [], int, suggestion[], int*, int);

int lex_compare(char *, char *, int n);

int close_suggestion_file(suggestion [], int *);

int onlimit_delete_suggestions(suggestion [], int *, int);

void sort_suggestions_by_freq(suggestion [], int*);

#endif
