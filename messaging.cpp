/*
	messaging.cpp implements interface defined in messaging.h
	DeVonte' Applewhite
	12/05/14 CSE40622
	references used:
	http://tldp.org/LDP/LG/issue87/vinayak.html
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <openssl/aes.h>
#include <iostream>
#include "messaging.h"

using namespace std;

Messaging::Messaging(){ //constructor
	count = 0; //init count
	memset(key,0,KSIZE); //zero key
}

//copy b into a
void Messaging::copy(unsigned char a[], unsigned char b[], int size){
	for(int i=0;i<size;i++){
		a[i] = b[i]; //copy over
	}
}

//xor bytes of c with m
void Messaging::xorb(unsigned char m[], unsigned char c[]){
	for(int i=0;i<KSIZE;i++){
		m[i] = m[i] xor c[i]; //xor each element and apply to m
	}
}

//generate random key
int Messaging::generate_key(){
	int fd;

	if ((fd = open("/dev/random", O_RDONLY)) == -1){ //open /dev/random
		perror ("open error");
	}
	if ((read(fd, key, KSIZE)) == -1){ //read KSIZE bytes from /dev/random
		perror ("read key error");
	}

	close (fd); //close file
	return 0;
}

//create key using seed value
void Messaging::create_key(unsigned int keyval){
		srand(keyval); //seed randomizer with common start
		for(int i=0;i<KSIZE;i++){
			key[i] = rand()%256; //random byte
		}
}

//reverses an array of unsigned char values
void Messaging::revuchararr(unsigned char a[16]){
	if(a != NULL){
		int low = 0;
		int high = 15;
		while(low < high){
			unsigned char temp = a[low];
			a[low] = a[high];
			a[high] = temp;
			low++;
			high--;
		}
	}
}

//turn a uint128 into an 8 byte char array
void Messaging::num2uchararray(uint128 v, unsigned char a[16]){
	uint128 x = v;
	int it = 0;
	while(x>0 && it<16){
		int temp = x%256;
		a[it] = temp;
		x /= 256;
		it++;
	}
	revuchararr(a);
}

//turn an 8 byte char array into a uint128
uint128 Messaging::uchararray2num(unsigned char a[16]){
	uint128 ans = 0; //set up answer
	uint128 base = 256; //unsigned char is 8 bytes so use base 256
	int i;
	for(i=0;i<16;i++){
		uint128 adjbase = 1; //adjusted base for iteration
		int j;
		for(j=0;j<16-i-1;j++){
			adjbase *= base; //scale adjbase up to 256^(i-1)
		}
		ans += adjbase * a[i]; //update answer based on a[i]'s value
	}
	return ans;
}

//encrypt message and send back message length and counter start value
int Messaging::encrypt(unsigned char in[], unsigned char out[]){
	unsigned char counter[CSIZE]; //holds the counter in byte form
	unsigned char buff[KSIZE]; //buffer that holds a counter

	memset(counter,0,CSIZE); //zero counter
	memset(buff,0,KSIZE); //zero counter
	num2uchararray(count,counter); //convert c into byte array
	bcopy(counter,buff,CSIZE); //put counter in buff
	copy(buff,(out+CSIZE*(ILIM-1)),CSIZE); //put counter in back of out
	AES_KEY AESkey;
	AES_set_encrypt_key((const unsigned char *)key,BLOCK, &AESkey);

	int r;
	unsigned char block[KSIZE]; //intermediate buffer to be encrypted
	unsigned char oblock[KSIZE]; //result of encryption

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
		bcopy(counter,buff,CSIZE); //put counter in second half of buff
		r++; //increment r
	}

	return r; //the message length
}

//decrypt message, put result in in[], and return length
int Messaging::decrypt(unsigned char in[], unsigned char out[],
	long unsigned int len)
{
	unsigned char counter[CSIZE]; //holds the counter in byte form
	unsigned char buff[KSIZE]; //buffer that holds a counter
	uint128 c; //raw count to be iterated

	memset(counter,0,CSIZE); //zero counter
	memset(buff,0,KSIZE); //zero buff

	copy(counter,(out+CSIZE*(ILIM-1)),CSIZE); //put initial count in counter
	c = uchararray2num(counter); //convert counter into a uint128
	bcopy(counter,buff,CSIZE); //put counter in second half of buff

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
		bcopy(counter,buff,CSIZE); //put counter in second half of buff
		r++; //increment r
	}

	in[r] = 0; //null terminate the message	
	
	return r; //the size of the message
}

Messaging::~Messaging(){ //destructor (not used)

}
