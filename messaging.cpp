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
#include "messaging.h"

using namespace std;

Messaging::Messaging(){
	count = 0; //init count
	memset(key,0,KSIZE); //zero key
	memset(nonce,0,CSIZE); //zero nonce
	for(int i=0;i<CSIZE;i++){
		nonce[i] = 255u-i; //initialize nonce
	}
}

void Messaging::copy(unsigned char a[], unsigned char b[], int size){  //b values into a
	for(int i=0;i<size;i++){
		a[i] = b[i]; //copy over
	}
}

void Messaging::xorb(unsigned char m[], unsigned char c[]){
	for(int i=0;i<KSIZE;i++){
		m[i] = m[i] xor c[i]; //xor each element and apply to m
	}
}

int Messaging::generate_key(){
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

void Messaging::create_key(unsigned int keyval){
		srand(keyval); //seed randomizer with common start
		for(int i=0;i<KSIZE;i++){
			key[i] = rand()%256; //random byte
		}
}

void Messaging::revuchararr(unsigned char a[8]){
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

void Messaging::num2uchararray(long unsigned int v, unsigned char a[8]){
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

//may take a key later
struct enc_info Messaging::encrypt(unsigned char in[], unsigned char out[]){
	struct enc_info retval;
	retval.start = count; //decryption knows where to start now

	unsigned char counter[CSIZE]; //holds the counter in byte form
	unsigned char buff[KSIZE]; //buffer that holds nonce and counter

	memset(counter,0,CSIZE); //zero counter
	memset(buff,0,KSIZE); //zero counter
	bcopy(nonce,buff,CSIZE); //put nonce in first half of buff
	num2uchararray(count,counter); //convert c into byte array
	bcopy(counter,(buff+CSIZE),CSIZE); //put counter in second half of buff

	AES_KEY AESkey;
	AES_set_encrypt_key((const unsigned char *)key,BLOCK, &AESkey);

	int r;
	unsigned char block[KSIZE];
	unsigned char oblock[KSIZE];

	r = 0;
	while(in[r] != 0 && r<ILIM-1){ //more letters to read
		memset(block,0,KSIZE); //zero block
		block[0] = in[r]; //read one byte (should be in position 0

		AES_encrypt(buff,oblock,&AESkey); //encrypt buff
		xorb(block,oblock); //xor message with encrypted buff

		copy((out+KSIZE*r),block,KSIZE); //copy block into encrypt array
		memset(oblock,0,KSIZE); //zero oblock
		memset(counter,0,CSIZE); //zero counter
		count++; //increment c to get new buff value
		num2uchararray(count,counter); //convert c into byte array
		bcopy(counter,(buff+CSIZE),CSIZE); //put counter in second half of buff
		r++; //increment r
	}
	retval.len = r;
	return retval; //the encryption info
}

int Messaging::decrypt(unsigned char in[], unsigned char out[],
	long unsigned int start, long unsigned int len)
{
	unsigned char counter[CSIZE]; //holds the counter in byte form
	unsigned char buff[KSIZE]; //buffer that holds nonce and counter
	long unsigned int c = start; //raw count to be iterated

	memset(counter,0,CSIZE); //zero counter
	memset(buff,0,KSIZE); //zero counter
	bcopy(nonce,buff,CSIZE); //put nonce in first half of buff
	num2uchararray(c,counter); //convert c into byte array
	bcopy(counter,(buff+CSIZE),CSIZE); //put counter in second half of buff

	AES_KEY AESkey;
	AES_set_encrypt_key((const unsigned char *)key,BLOCK, &AESkey);

	unsigned int r;
	unsigned char block[KSIZE];
	unsigned char oblock[KSIZE];

	r = 0;
	while(r < len && r<ILIM-1 ){ //more letters to read
		AES_encrypt(buff,oblock,&AESkey); //encrypt buff
		memset(block,0,KSIZE); //zero block
		copy(block,(out+KSIZE*r),KSIZE); //copy cipher chunk into block
		xorb(block,oblock); //xor message with encrypted buff

		in[r] = block[0]; //read one byte (should be in position 0)
//		copy((out+KSIZE*r),block,KSIZE); //copy block into encrypt array
		memset(oblock,0,KSIZE); //zero oblock
		memset(counter,0,CSIZE); //zero counter
		c++; //increment c to get new buff value
		num2uchararray(c,counter); //convert c into byte array
		bcopy(counter,(buff+CSIZE),CSIZE); //put counter in second half of buff
		r++; //increment r
	}

	in[r] = 0; //null terminate the message	
	
	return r; //the size of the message
}

Messaging::~Messaging(){

}
/*
int _dmain(int argc, char *argv[]){

	return 0;
}
*/
