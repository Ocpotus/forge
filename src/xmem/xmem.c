#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "xmem.h"


void *xmalloc(size_t size) {
	void *result = malloc(size);

	if(result == NULL) {
		perror(__func__);
		exit(EXIT_FAILURE);
	}

	return result; 
}

void *xcalloc(size_t nmemb, size_t size) {
	void *result = calloc(nmemb, size);

	if(result == NULL) {
		perror(__func__);
		exit(EXIT_FAILURE);
	}

	return result;
}

void *xrealloc(void *ptr, size_t size) {
	void *result = realloc(ptr, size);

	if(result == NULL) {
		perror(__func__);
		exit(EXIT_FAILURE);
	}

	return result;
}

void *xfree(void *ptr) {
	free(ptr);

	return NULL;
}

void xfreev(void *ptr, ...) {
	va_list valist;

	va_start(valist, ptr);

	while(ptr != NULL) {
		free(ptr);
		ptr = va_arg(valist, void *);
	}

	va_end(valist);
}
