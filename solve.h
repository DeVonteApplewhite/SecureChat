//c4col.h
//main header file used to hold the columns class of connect 4

#ifndef C4COL_H
#define C4COL_H

// note preprocessing directive wrapper above

#include <iostream>
#include <openssl/rsa.h>
#include <cstring>
#include "messaging.h"

using namespace std;

class human
{
	public:
		human();	//constructor
		unsigned int choosePaint();
	
		unsigned int chosenPaint;
		//unsigned int partnersPaint;	// sent over as g_val
		unsigned int sharedPaint;
		//unsigned int startingPaint;	// provided by the program
		unsigned long int g_val_sent;

		unsigned int k;	// k is a random string used for authentication
		unsigned int k_partner;
		RSA *keypair;

		void printcipher(int);

		struct send 
		{
			char *encr_K;	// ping 1, ping 2
			string name;	// ping 1, ping 2
			unsigned long int g_val;	// ping 1, ping 2
			string HMAC;	// ping 2, ping 3
			string p_HMAC;	// ping 2, ping 3	
		};

		send ping;
		Messaging chat;

		unsigned char plain[ILIM]; //plaintext container
		unsigned char cipher[OLIM]; //ciphertext container
};

#endif
