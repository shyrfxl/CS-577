#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SHA_BLOCKSIZE 64
#define SHA_DIGESTSIZE 20
  void hmac_generate(unsigned char* msg, unsigned long msglen, unsigned long key, unsigned char* digest)
{
	SHA_CTX os,is;
	char buf[SHA_BLOCKSIZE] = "\0";
	char Key[SHA_BLOCKSIZE] = "\0";
	unsigned char isha[SHA_DIGESTSIZE];
	int i = 0, lk = 0;
	sprintf(Key,"%lu",key);
	lk = strlen(Key);

	for (i = 0; i < lk; i++)
		buf[i] = Key[i]^0x36;
	for (i = lk; i < SHA_BLOCKSIZE; i++ )
		buf[i] = 0x36;

	SHA1_Init(&is);
	SHA1_Update(&is,(unsigned char *)buf,SHA_BLOCKSIZE);
	SHA1_Update(&is,msg,msglen);
	SHA1_Final(isha,&is);
	SHA1_Init(&os);

	for (i = 0; i<lk; i++) 
		buf[i] = Key[i]^0x5c;

	for (i = lk; i < SHA_BLOCKSIZE; i++)
		buf[i] = 0x5c;

	SHA1_Update(&os,(unsigned char*)buf,SHA_BLOCKSIZE);
	SHA1_Update(&os,isha,SHA_DIGESTSIZE);
	SHA1_Final(digest,&os);

	for (i=0;i<20;i++)
		printf("%.2x",(int)digest[i]);
	printf("\n");
} 

