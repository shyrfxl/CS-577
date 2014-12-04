#include <stdio.h>
#include <stdlib.h>
#include<string.h>
int main()
{
   char *str;

   /* Initial memory allocation */
   str = (char *) malloc(15);
   strcpy(str,"AAAAAAAA");
   printf("String = %s\n", str);

   /* Reallocating memory */
   str = (char *) realloc(str, 25);
   strcat(str, "AAAAAA");
   printf("String = %s\n", str);

   //free(str);
   
   return(0);
}
