#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include <unistd.h>
#include <dirent.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>


#if defined(__linux__)
	#if defined(_GNU_SOURCE)
		#include <limits.h>
		#define USE_GNU_LINUX
	#else
		#include <linux/limits.h>
	#endif
#else
	#define PATH_MAX 4096
#endif

#include "util.h"


#if !defined(USE_GNU_LINUX)
char *strdup(const char *s) {
	char *result = NULL;
	size_t len = strlen(s) + 1;

	result = malloc(len * sizeof(*result));

	if(result != NULL) {
		result = memcpy(result, s, len);
	}

	return result;
}

char *strndup(const char *s, size_t n) {
	char *result = malloc(n * sizeof(*result));

	if(result != NULL) {
		result = memcpy(result, s, n);
	}

	return result;
}
#endif

#if !defined(USE_GNU_LINUX)
char *basename(char *s) {
	int i;

	for(i = strlen(s); s[i] != '/'; i -= 1);

	return s + i + 1;
}

char *dirname(const char *s) {
	int i;

	for(i = strlen(s); s[i] != '/'; i -= 1);

	return strndup(s, i);
}
#endif

int shell_execute(char *program, char **args) {
	int result = 0;
	int status = 0;
	pid_t p = fork();

	if(p < 0) {
		exit(EXIT_FAILURE);
	} else if(p == 0) {
		if(execvp(program, args) == -1) {
			exit(EXIT_FAILURE);
		}
	} else {
		if(waitpid(p, &status, WUNTRACED) == -1) {
			exit(EXIT_FAILURE);
		}
	}

	if(WIFEXITED(status)) {
		result = WEXITSTATUS(status);
	}

	return result;
}

char *get_dir_name() {
	char *result = NULL;
	char absPath[PATH_MAX] = { 0 };


	if(getcwd(absPath, PATH_MAX) != NULL) {
		size_t i;

		for(i = strlen(absPath); i > 0 && absPath[i] != '/'; i -= 1);

		result = strdup(absPath + i + 1);
	}

	return result;
}

char *strjoin(const char *s1, const char *s2) {
	char *result = NULL;
	size_t len = strlen(s1) + strlen(s2) + 2;

	result = malloc(len * sizeof(*result));

	if(result == NULL) {
		goto EXIT;
	}

	snprintf(result, len, "%s%s", s1, s2);
EXIT:
	return result;
}
