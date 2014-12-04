#define _GNU_SOURCE

#include "dlfcn.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/mman.h"

static void (*original_free)(void* ptr) = NULL;
static void etrace_init(void)
{
	original_free = dlsym(RTLD_NEXT, "free");
	if (original_free == NULL)
	 {
        	fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    	}
}
void free(void * ptr)
{
	unsigned int padding = 0;
	unsigned int size = 0;
	int pagesize = sysconf(_SC_PAGE_SIZE);
	printf("0x%d ", pagesize);
	printf("%s\n",(char *)ptr);
	if(original_free == NULL) 
	{
       	 	etrace_init();
    	}
	padding = *(unsigned int*)(ptr-4);
	padding = *(unsigned int*)(ptr-8);
	printf("%u\n", padding);
	if(mprotect(ptr+size, pagesize,PROT_READ | PROT_WRITE) == -1)
		perror("The error in mprotect\n");
	original_free((char *)ptr - padding);
}
