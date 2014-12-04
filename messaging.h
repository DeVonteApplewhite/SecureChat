//messaging.h
//main header file used for the AES counter mode stream cipher messaging

#ifndef MESSAGING_H
#define MESSAGING_H

#define BLOCK 128
#define KSIZE 16
#define CSIZE 8
#define ILIM 256
#define OLIM 4096

struct enc_info{
	int len;
	unsigned int start; 
};

class Messaging { //class for sending messages
	public:

	Messaging(); // default constructor
	void copy(unsigned char[],unsigned char[],int);
	void xorb(unsigned char[],unsigned char[]);
	int generate_key();
	void create_key(unsigned int);
	void revuchararr(unsigned char[]);
	void num2uchararray(long unsigned int,unsigned char[]);
	struct enc_info encrypt(unsigned char[],unsigned char[]);
	int decrypt(unsigned char[],unsigned char[],
	long unsigned int,long unsigned int);
	~Messaging(); // default deconstructor

	private:
	unsigned long int count;
	unsigned char nonce[CSIZE];
	unsigned char key[KSIZE];

};

#endif
