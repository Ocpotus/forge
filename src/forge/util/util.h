#ifndef FORGE_UTIL_H
#define FORGE_UTIL_H

#include <stddef.h>

#ifndef _GNU_SOURCE
char *strdup(const char *s);
char *strndup(const char *s, size_t n);
char *basename(char *s);
char *dirname(const char *s);
#endif


int shell_execute(char *program, char **args);
char *get_dir_name();
char *strjoin(const char *s1, const char *s2);


#endif
