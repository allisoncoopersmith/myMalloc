#ifndef _mymalloc_h_
#define _mymalloc_h_
#include <stdio.h>
#include <stdlib.h>

#define malloc(x) mymalloc(x, __FILE__ , __LINE__ )// definition for the redirect of malloc(x)
#define free(x) myfree(x, __FILE__ , __LINE__ )// definition for the redirect of free(x)

static char myblock[4096];

void* mymalloc(size_t sizeRequested, char *file, int line);
void myfree(void* memToFree, char *file,int line);

#endif
