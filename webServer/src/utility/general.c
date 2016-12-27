#include "general.h"
/*
 * get string format of current time, not thread-safe.
 */
char* get_time() {
	time_t current_time = time(NULL);
	char *c_time_string;
	if (current_time == ((time_t)-1)) {
		fprintf(stderr, "Failure to obtain the current time.\n");
		exit(EXIT_FAILURE);
	}
	c_time_string = ctime(&current_time);
	if (c_time_string == NULL) {
		fprintf(stderr, "Failure to convert the current time.\n");
		exit(EXIT_FAILURE);
	}
	return c_time_string;
}

