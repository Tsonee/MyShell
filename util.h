#ifndef UTIL_H
#define UTIL_H

#include <bits/posix1_lim.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <termios.h>
#include "CTui.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <stdlib.h>
#include <errno.h>
#include <locale.h>

#define MAX_TOKEN_SIZE 256
#define MAX_TOKENS 256

void get_current_username(char user[]);

void enable_raw_mode(struct termios *original);

void disable_raw_mode(struct termios *original);

int traverse_directory(const char *path, const char *target_name);

#endif
