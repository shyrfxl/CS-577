#define _GNU_SOURCE

#include "sys/mman.h"
#include "dlfcn.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "signal.h"
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
static void *(*original_malloc)(size_t size) = NULL;
static void mtrace_init(void)
{
	original_malloc = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc");
	if (NULL == original_malloc)
	 {
        	fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    	}
}
static void
handler(int sig, siginfo_t *si, void *unused)
{
    printf("Got SIGSEGV at address: 0x%lx\n",
            (long) si->si_addr);
    exit(EXIT_FAILURE);
}
void *malloc(size_t size)
{
	int pagesize = sysconf(_SC_PAGE_SIZE);
	unsigned long offset = 0, padding = 0;
	printf("0x%x\n",pagesize);
	struct sigaction sa;
   	sa.sa_flags = SA_SIGINFO;
   	sigemptyset(&sa.sa_mask);
   	sa.sa_sigaction = handler;
   	if (sigaction(SIGSEGV, &sa, NULL) == -1)
      	        handle_error("sigaction");
	if(original_malloc==NULL) 
	{
       	 	mtrace_init();
    	}
	void *tempPtr = (* original_malloc)(size + pagesize-1);
	printf("Start of region temp:    0x%lx\n", (long)tempPtr);
	char * Protect = (char *)(((long)tempPtr + size + pagesize-1) & ~(pagesize - 1));
	printf("Start of region Protect: 0x%lx\n", (long)Protect);
	if(mprotect(Protect, pagesize,PROT_NONE) == -1)
		perror("The error in mprotect\n");
	offset = (long)Protect - (long)tempPtr;
	padding = offset - size;
	printf(" offset: 0x%lx padding :0x%lx\n", (long)offset, (long)padding);
	char *modify_address = (char *)tempPtr+padding;
	memset(tempPtr, 0, padding);
	*((unsigned int*)(modify_address-4)) = padding;
	*((unsigned int*)(modify_address-8)) = size;
	//printf("%lu\n",(unsigned long)*((unsigned long*)(modify_address-4)));
	return (void*)modify_address;
}
/*
static void  (*original_free)(void* ptr) = NULL;
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
	//printf("0x%d ", padding);
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
*/