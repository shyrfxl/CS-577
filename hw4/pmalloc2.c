#define _GNU_SOURCE

#include "sys/mman.h"
#include "dlfcn.h"
#include "stdio.h"
//#include <linux/kernel.h>
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
	//printf("0x%x\n",pagesize);
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
	void *tempPtr = (* original_malloc)(size + 2*pagesize-1);
	printf("Start of region temp:    0x%lx\n", (long)tempPtr);
	char * Protect_over = (char *)(((long)tempPtr + size + 2*pagesize-1) & ~(pagesize - 1));
	char * Protect_under = (char *)(((long)tempPtr) & ~(pagesize - 1));
	printf("Start of region Protect_over: 0x%lx\n", (long)Protect_over);
	printf("Start of region Protect_under: 0x%lx\n", (long)Protect_under);
	if(mprotect(Protect_over, pagesize,PROT_NONE) == -1)
		perror("The error in mprotect\n");
	offset = (long)Protect_over - (long)tempPtr;
	padding = offset - size;
	printf(" offset: 0x%lx padding :0x%lx\n", (long)offset, (long)padding);
	char *modify_address = (char *)tempPtr+padding;
	memset(tempPtr, 0, padding);
	*((unsigned int*)(modify_address-8)) = padding;
	*((unsigned int*)(modify_address-12)) = size;
	*((char *)(modify_address-1)) = '.';
	//int i = (int)*(modify_address-12);
	//printf("%u\n",(unsigned int)i);
	//printf("%c\n",(char)*(modify_address-1));
	if(mprotect(Protect_under, pagesize,PROT_READ ) == -1)
		perror("The error in mprotect\n");
	return (void*)modify_address;
	
}
