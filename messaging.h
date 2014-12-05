//messaging.h
//main header file used for the AES counter mode stream cipher messaging

#ifndef MESSAGING_H
#define MESSAGING_H

#define BLOCK 128 //number of bytes in a block
#define KSIZE 16 //key size
#define CSIZE 16 //size of nonce and counter
#define ILIM 256 //holds the message
#define OLIM 4096 //holds the encrypted data

typedef __uint128_t uint128; //for the counter

class Messaging { //class for sending messages
	public:

	Messaging(); // default constructor
	void copy(unsigned char[],unsigned char[],int);
	void xorb(unsigned char[],unsigned char[]);
	int generate_key();
	void create_key(unsigned int);
	void revuchararr(unsigned char[]);
	void num2uchararray(uint128,unsigned char[]);
	uint128 uchararray2num(unsigned char[16]);
	int encrypt(unsigned char[],unsigned char[]);
	int decrypt(unsigned char[],unsigned char[],
	long unsigned int);
	~Messaging(); // default deconstructor

	private:

	uint128 count; //running count for chat session
	unsigned char key[KSIZE]; //holds the secret key
};

#endif
