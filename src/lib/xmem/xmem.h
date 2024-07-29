#ifndef XMEM_H
#define XMEM_H

#include <stddef.h>


/*
 * xmalloc/xcalloc/xrealloc, just like malloc/calloc/realloc,
 * (re)allocates a block of memory. However on failure,
 * malloc/calloc/realloc returning NULL, perror is called with
 * __func__ and the process is exited with EXIT_FAILURE.
 */
void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
void *xrealloc(void *ptr, size_t size);

/*
 * xfree, just like free, frees a block of memory, but
 * it returns NULL.
 */
void *xfree(void *ptr);

/*
 * xfreev frees, with free, a variadic list of pointers.
 * The last pointer must be NULL.
 */
void xfreev(void *ptr, ...);


#endif
