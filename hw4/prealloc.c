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
static void
handler(int sig, siginfo_t *si, void *unused)
{
    printf("Got SIGSEGV at address: 0x%lx\n",
            (long) si->si_addr);
    exit(EXIT_FAILURE);
}
static void*  (*original_realloc)(void* ptr, size_t size) = NULL;
static void etrace_init(void)
{
	original_realloc = dlsym(RTLD_NEXT, "realloc");
	if (original_realloc == NULL)
	 {
        	fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    	}
}
void* realloc(void * ptr, size_t size)
{
	unsigned long padding = 0;
	unsigned int rsize = 0;
	unsigned long offset = 0;
	int pagesize = sysconf(_SC_PAGE_SIZE);
	printf("0x%lx ", padding);
	//printf("%s\n",(char *)ptr);
	struct sigaction sa;
   	sa.sa_flags = SA_SIGINFO;
   	sigemptyset(&sa.sa_mask);
   	sa.sa_sigaction = handler;
   	if (sigaction(SIGSEGV, &sa, NULL) == -1)
      	        handle_error("sigaction");
	if(original_realloc == NULL) 
	{
       	 	etrace_init();
    	}
	padding = *(unsigned long*)(ptr-4);
	rsize = *(unsigned int*)(ptr-8);
	printf("%lu\n", padding);
	if(mprotect(ptr+rsize, pagesize,PROT_READ | PROT_WRITE) == -1)
		perror("The error in mprotect\n");
	void *tempPtr = original_realloc((char *)ptr - padding, size+pagesize-1);
	char * Protect = (char *)(((long)tempPtr + size + pagesize-1) & ~(pagesize - 1));
	printf("Start of region Protect: 0x%lx\n", (long)Protect);
	if(mprotect(Protect, pagesize,PROT_NONE) == -1)
		perror("The error in mprotect\n");
	offset = (long)Protect - (long)tempPtr;
	padding = offset - size;
	printf(" offset: 0x%lx padding :0x%lx\n", (long)offset, (long)padding);
	char *modify_address = (char *)tempPtr+padding;
	*((unsigned int*)(modify_address-4)) = padding;
	*((unsigned int*)(modify_address-8)) = size;
	//printf("%lu\n",(unsigned long)*((unsigned long*)(modify_address-4)));
	return (void*)modify_address;

	
}
