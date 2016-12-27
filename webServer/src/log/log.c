#include "log.h"

extern char *logfile;
FILE *p;

/*
 * initialize and open logfile
 */
void log_init() {
    char* c_time_string;
	int log_rename;
	// backup old logfile
	if (access(logfile, F_OK) == 0) {
		c_time_string = get_time();
		char newName[14];
		char *tail = ".log";
		memcpy(newName, c_time_string, 10);
		memcpy(newName+10, tail, 4);
		log_rename = rename(logfile, newName);
		if (log_rename == -1) {
			fprintf(stderr, "Failure to backup old logfile.\n");
			exit(EXIT_FAILURE);
		}
	}
	if ((p = fopen(logfile, "a")) == NULL) {
		fprintf(stderr,"can't create logfile, error: %s",strerror(errno));
		exit(EXIT_FAILURE);
	}
}
/*
 * write to logfile
 * param: output format
 * return: nothing
 */
void log_record(const char *fmt, ...) {
	char* c_time_string;
	c_time_string = get_time();
	fprintf(p,"%s", c_time_string);
	va_list ap;
	va_start(ap, fmt);
	vfprintf(p, fmt, ap);
	fprintf(p, "\n");
	va_end(ap);
	fflush(p);
}

/*
 * close logfile
 */
void log_close() {
	fclose(p);
}
