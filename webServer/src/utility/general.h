#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))

char* get_time();

#endif
