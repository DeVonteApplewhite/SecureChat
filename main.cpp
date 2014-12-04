#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <openssl/aes.h>
#include <iostream>
#include "messaging.h"

using namespace std;

int main(void){
	unsigned char plain[ILIM]; //plaintext container
	unsigned char cipher[OLIM]; //ciphertext container
	memset(plain,0,ILIM);
	memset(cipher,0,OLIM);

	Messaging A;
	Messaging B;
	srand(20);
	unsigned int commonkey = rand();

	A.create_key(commonkey);
	B.create_key(commonkey);

	strncpy((char *)plain,"Stronger than Hi? I think not ;)",ILIM-1);
	printf("%s\n",plain);
	struct enc_info e = A.encrypt(plain,cipher);
	int j = e.len * KSIZE;

	//for(int i=0;i<j;i++){
	//	printf("|%03u|",cipher[i]);
	//}

//	printf("\n");
	memset(plain,0,ILIM);
	int end = B.decrypt(plain,cipher,e.start,e.len);
	printf("%s\n",plain);
	return 0;
}
