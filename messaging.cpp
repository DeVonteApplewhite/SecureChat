/*
	DeVonte' Applewhite
	09/30/14 CSE40622
	references used:
	http://tldp.org/LDP/LG/issue87/vinayak.html
*/

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

#define BLOCK 128
#define KSIZE 16
#define CSIZE 8
#define ILIM 256
#define OLIM 4096

using namespace std;

void xorb(unsigned char m[], unsigned char c[]){
	for(int i=0;i<KSIZE;i++){
		m[i] = m[i] xor c[i]; //xor each element and apply to m
	}
}

int generate_key(unsigned char key[], unsigned char nonce[]){
	int fd;

	if ((fd = open("/dev/random", O_RDONLY)) == -1){
		perror ("open error");
	}
	if ((read(fd, key, KSIZE)) == -1){
		perror ("read key error");
	}
	if ((read (fd, nonce, CSIZE)) == -1){
		perror ("read iv error");
	}
	close (fd);
	return 0;
}

void revuchararr(unsigned char a[8]){
	if(a != NULL){
		int low = 0;
		int high = 7;
		while(low < high){
			unsigned char temp = a[low];
			a[low] = a[high];
			a[high] = temp;
			low++;
			high--;
		}
	}
}

void num2uchararray(long unsigned int v, unsigned char a[8]){
	int x = v;
	int it = 0;
	while(x>0 && it<8){
		int temp = x%256;
		a[it] = temp;
		x /= 256;
		it++;
	}
	revuchararr(a);
}

int encryptCNT(unsigned char in[], unsigned char out[],
	unsigned char key[]){ //may take a key later
	int rfd,wfd;
	//unsigned char key[KSIZE]; //symmetric key
	unsigned char nonce[CSIZE]; //nonce vector
	unsigned char counter[CSIZE]; //holds the counter in byte form
	unsigned char buff[KSIZE]; //buffer that holds nonce and counter
	long unsigned int c = 0; //raw count to be iterated

	//generate_key(key,nonce); //get random key and nonce vector
	memset(counter,0,CSIZE); //zero counter
	memset(buff,0,KSIZE); //zero counter
	bcopy(nonce,buff,CSIZE); //put nonce in first half of buff
	num2uchararray(c,counter); //convert c into byte array
	bcopy(counter,(buff+CSIZE),CSIZE); //put counter in second half of buff

	AES_KEY AESkey;
	AES_set_encrypt_key((const unsigned char *)key,BLOCK, &AESkey);

	int r;
	unsigned char block[KSIZE];
	unsigned char oblock[KSIZE];
	//int jj = 1;
	r = 0;
	while(in[r] != 0){ //more letters to read
		memset(block,0,KSIZE); //zero block
		block[0] = in[r]; //read one byte (should be in position 0
/*
		printf("|buff||nonce||counter|\n");
		for(int aa=0;aa<KSIZE;aa++){
			printf("|%03u|",buff[aa]);
			if(aa<CSIZE){
				printf("|%03u||%03u|\n",nonce[aa],counter[aa]);
			}else{
				printf("\n");
			}
		}
*/
/*
		if(jj == 1){
			printf("plaintext:\n");
			for(int k=0;k<KSIZE;k++){
				printf("|%03u|",block[k]);
			}
			printf("\n\n");
		}
*/
		AES_encrypt(buff,oblock,&AESkey); //encrypt buff
		xorb(block,oblock); //xor message with encrypted buff
/*
		if(jj == 1){
			printf("ciphertext:\n");
			for(int k=0;k<KSIZE;k++){
				printf("|%03u|",block[k]);
			}
			printf("\n\n");
			//jj = 0;
		}
*/
		write(wfd,block,KSIZE);
		memset(oblock,0,KSIZE); //zero oblock
		memset(counter,0,CSIZE); //zero counter
		c++; //increment c to get new buff value
		num2uchararray(c,counter); //convert c into byte array
		bcopy(counter,(buff+CSIZE),CSIZE); //put counter in second half of buff
	}

	close(rfd);
	close(wfd);
	if ((wfd = open("key", O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO)) == -1){
		perror ("open error for output file");
	}
	write(wfd,key,KSIZE); //write key to file
	close(wfd);
	if ((wfd = open("nonce", O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO)) == -1){
		perror ("open error for output file");
	}
	write(wfd,nonce,KSIZE); //write nonce to file
	close(wfd);

	return 0;
}

int decryptCNT(char *ifile, char *ofile, char *keyfile, char *noncefile)
{
	int rfd,wfd,kfd,nfd;
	if((kfd = open(keyfile,O_RDONLY)) == -1){
		perror("open error for key file");
	}
	unsigned char key[KSIZE]; //key for decryption
	read(kfd,key,KSIZE); //read in key
	AES_KEY AESkey;
	AES_set_encrypt_key((const unsigned char *)key,BLOCK, &AESkey);

	if((nfd = open(noncefile,O_RDONLY)) == -1){
		perror("open error for nonce file");
	}
	unsigned char nonce[CSIZE]; //initialization vector
	read(nfd,nonce,CSIZE); //read in nonce
	if ((rfd = open(ifile, O_RDONLY)) == -1){
		perror ("open error for input file");
	}
	if ((wfd = open(ofile, O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO)) == -1){
		perror ("open error for output file");
	}

	int r;
	unsigned char block[KSIZE];
	unsigned char oblock[KSIZE]; //block to be written to a file
	unsigned char counter[CSIZE]; //holds the current count value of c
	unsigned char buff[KSIZE]; //buffer to hold nonse and counter
	long unsigned int d = 0; //starting count

	memset(counter,0,CSIZE); //zero counter
	memset(buff,0,KSIZE); //zero counter
	bcopy(nonce,buff,CSIZE); //put nonce in first half of buff

	//int jj = 1;
	r = -1;
	while(r != 0){
		memset(block,0,KSIZE);
		r = read(rfd,block,KSIZE); //read 16 bytes
		num2uchararray(d,counter); //convert c into byte array
		bcopy(counter,(buff+CSIZE),CSIZE); //put counter in second half of buff
		//printf("%d\n",r);
		if(r == -1){
			printf("error:%s\n",strerror(errno));
		}else{
			if(r != 0){ //valid 16-bytes to process
/*
				if(jj == 1){
					printf("plaintext:\n");
					for(int k=0;k<KSIZE;k++){
						printf("|%03u|",block[k]);
					}
					printf("\n\n");
				}
*/
				AES_encrypt(buff,oblock,&AESkey); //encrypt buff
				xorb(block,oblock); //cipher xor encrypted buff
/*
				if(jj == 1){
					printf("ciphertext:\n");
					for(int k=0;k<KSIZE;k++){
						printf("|%03u|",block[k]);
					}
					printf("\n\n");
					//jj = 0;
				}
*/
				write(wfd,block,1); //write 1st byte of block to file
				memset(oblock,0,KSIZE); //zero oblock
				memset(counter,0,CSIZE); //zero counter
				d++; //increment c to get new buff value
			}
		}
	}

	close(rfd);
	close(wfd);
	close(kfd);
	close(nfd);
	return 0;
}

int main(int argc, char *argv[]){
	if(argc == 4){
		if(!strcmp("1",argv[3])){
			encryptCNT(argv[1],argv[2]);
		}
	}else if(argc == 6){
		if(!strcmp("1",argv[5])){
			decryptCNT(argv[1],argv[2],argv[3],argv[4]);
		}
	}else{
		printf("incorrect number of arguments\n");
		return 1;
	}

	return 0;
}
