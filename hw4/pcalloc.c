#include "sys/mman.h"
#include "stdio.h"
#include "signal.h"
#include "stdlib.h"
#include "unistd.h"
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
static void
handler(int sig, siginfo_t *si, void *unused)
{
    printf("Got SIGSEGV at address: 0x%lx\n",
            (long) si->si_addr);
    exit(EXIT_FAILURE);
}
void* calloc(size_t num, size_t size)
{
	int pagesize = sysconf(_SC_PAGE_SIZE);
	unsigned long offset = 0, padding = 0;
	void * Tempstr = NULL;
	Tempstr = mmap(NULL, (num*size+pagesize-1) , PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	printf("Start of region temp:    0x%lx\n",(long)Tempstr);
	struct sigaction sa;
   	sa.sa_flags = SA_SIGINFO;
   	sigemptyset(&sa.sa_mask);
   	sa.sa_sigaction = handler;
   	if (sigaction(SIGSEGV, &sa, NULL) == -1)
      	        handle_error("sigaction");
	char *Protect =  (char *)(((long)Tempstr +num*size + pagesize-1) & ~(pagesize - 1));
	printf("Start of region Protect: 0x%lx\n", (long)Protect);
	if(mprotect(Protect, pagesize,PROT_NONE) == -1)
		perror("The error in mprotect\n");
	offset = (long)Protect - (long)Tempstr;
	padding = offset - size*num;
	printf(" offset: 0x%lx padding :0x%lx\n", (unsigned long)offset, (unsigned long)padding);
	char *modify_address = (char *)Tempstr+padding;
	*((unsigned int*)(modify_address-4)) = padding;
	*((unsigned int*)(modify_address-8)) = num*size;
	return (void *)modify_address;

}
