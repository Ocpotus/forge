#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "xerror.h"


void xerror(const char *s) {
	if(s != NULL) {
		fprintf(stderr, "%s: %s\n", s, strerror(errno));
	}

	exit(EXIT_FAILURE);
}

void xerrorf(const char *s, const char *format, ...) {
	va_list valist;

	va_start(valist, format);
	fprintf(stderr, "%s: ", s);
	vfprintf(stderr, format, valist);
	va_end(valist);
	exit(EXIT_FAILURE);
}
