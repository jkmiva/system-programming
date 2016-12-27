#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "../utility/general.h"
#include "../utility/socket_wrapper.h"

void log_init();
void log_record(const char *fmt, ...);
void log_close();

#endif
