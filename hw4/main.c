#include "stdio.h"
#include "malloc.h"
#include "stdlib.h"
#include "string.h"
int main(int argc, char **argv)
{
	char * s = (char *)malloc(20);
	char d[20];
	int i = 0;
	//printf("Start of region:        0x%lx\n", (long)s);
	memcpy(d, argv[1], 30);
	//s = (char *)realloc(s, 28);
	printf("Start of region:        0x%lx\n", (long)s);
	for(i =0;i<30;i++)
	{
		s[i]=d[i];
	}
	printf("%d\n",i);
	free(s);
	return 0;
}
