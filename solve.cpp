#include <stdio.h>
#include <stdlib.h>
#include "solve.h"
#include <iomanip>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <gcrypt.h>
#include <openssl/rsa.h>
#include <cstring>
#include <openssl/hmac.h>
#include <string.h>
#include <openssl/sha.h>
#include <cstdlib>
#include "messaging.h"

using namespace std;

human::human()				// default constructor
{
	// Assign Alice and Bob a public key/private key key pair
	keypair = RSA_generate_key(2048,3,NULL,NULL);
	memset(plain,0,ILIM); //zero buffers
	memset(cipher,0,OLIM);
}

unsigned int human::choosePaint()	// assign random 9 digit int
{
	return (rand()%1000000);
}

string doHMAC(string key, string message)
{
	unsigned char md[EVP_MAX_MD_SIZE + 1];
	md[EVP_MAX_MD_SIZE] = '\0';
	unsigned int md_len;

 	HMAC(EVP_sha1(), (const void *)key.c_str(),
      		key.size(), (unsigned char *)message.c_str(), message.size(), md, &md_len);

	char mdString[EVP_MAX_MD_SIZE*2+1];
	mdString[EVP_MAX_MD_SIZE*2] = '\0';
	for(int i = 0; i < md_len; i++)
		sprintf(&mdString[i*2], "%02x", (unsigned int)md[i]);
	string ans(mdString);
	return ans;
}

string doSHA1(unsigned int k_sum)
{

	char string[256];
	sprintf(string, "%d", k_sum);	

	unsigned char digest[SHA_DIGEST_LENGTH];
	//char string[] = "hello world";

	SHA1((unsigned char*)&string, strlen(string), (unsigned char*)&digest);    

	char mdString[SHA_DIGEST_LENGTH*2+1];

	for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
	 	sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
	}

	std::string ans(mdString);
	return ans;

	//printf("SHA1 digest: %s\n", mdString);

}

unsigned long int power(unsigned long a,unsigned long n,unsigned long m){
	unsigned long e = n;
	unsigned long y = 1;
	unsigned long z = a;

	while(1){
		if(e%2 == 1){ //e is odd
			y = (y*z)%m;
		}
		if(e<=1){
			return y; //answer
		}

		z = (z*z) % m;
		e = e >> 1;
	}
	return 0;
}

unsigned int findPrime()
{
	while(1)
	{
		unsigned int start = ((rand()%50000)*2)+1;

		for(start; start<=999999; start++)
		{
			int count = 0;
			for(int i=2; i<=start/2; i++)
			{
				if(start%i==0)
				{
					count++;
					break;
				}	char mdString[SHA_DIGEST_LENGTH*2+1];
			}
		
			if(count==0 && start != 1)
			{
				return start;
			}
		}
	}
}

unsigned int findRoot(unsigned int prime)
{
	unsigned int s = prime-1;
	vector<unsigned int> primeFactors;

	// Calculate prime factors
	for(unsigned int start=2; start<=s; start++)
	{
		int count = 0;
		for(int i=2; i<=start/2; i++)
		{
			if(start%i==0)
			{
				count++;
				break;
			}
		}
	
		if((count==0) &&  (s%start==0))
		{
			primeFactors.push_back(start);

		}
	}

	// Check guess against all prime factors
	while(1)
	{
		int counter = 0;
		unsigned long int guess = (rand()%prime);
		for(int i=0; i<primeFactors.size(); i++)
		{
			if(power(guess,s/primeFactors[i],prime) != 1)
			{
				counter++;
			}
			else
			{
				break;
			}
		}	
		if(counter == primeFactors.size())
		{
			return guess;
		}
	}

}

void human::printcipher(int len){
	int lim = len*KSIZE;
	for(int i=0;i<lim;i++){
		printf("|%03u|",cipher[i]);
	}
}

int main(int argc, char *argv[]){

	// Seed the random number generator
	srand(time(NULL));	

	// Create Alice and Bob
	human Alice;
	human Bob;

	// Have Alice and Bob choose paint
	Alice.chosenPaint = Alice.choosePaint();
	Bob.chosenPaint = Bob.choosePaint();
	cout << "Alice Secret Value:	" << Alice.chosenPaint << endl;
	cout << "Bob Secret Value:   	" << Bob.chosenPaint << endl;

	// Have the chat program generate a prime "p" and a primitive root mod p "g"
	unsigned long int p = findPrime();
	unsigned int g = findRoot(p);
	cout << "DH Prime: 		" << p << endl;
	cout << "DH Primitive Root:	" << g << endl;

	// Have Alice compute g ^ Alice.chosenPaint mod p
	unsigned int dh_ping = power(g, Alice.chosenPaint, p);
	Alice.g_val_sent = dh_ping;
	cout << "g^A mod p: 		" << dh_ping << endl;

	// Have Alice generate a random string Ka
	Alice.k = (rand()%1000000);
	cout << "Ka: 			" << Alice.k << endl;

	// Have Alice encrypt Ka using Bob's public key
	char msg1[256];
	sprintf(msg1, "%d", Alice.k);
	msg1[strlen(msg1)] = '\0';
	char *encrypt1 = (char*)malloc(RSA_size(Bob.keypair));
	int encrypt1_len;
	encrypt1_len = RSA_public_encrypt(strlen(msg1)+1, (unsigned char*)msg1, 			(unsigned char*)encrypt1, Bob.keypair, RSA_PKCS1_OAEP_PADDING);
	cout << "Ka Encryption: 	" << endl;
    	for(int i=0;i<encrypt1_len;i++)
	{
    	    printf("|%03u|",encrypt1[i]);
    	}
	cout << endl;

	// Have Alice create ping1
	Alice.ping.encr_K = encrypt1;
	Alice.ping.name = "Alice";
	Alice.ping.g_val = dh_ping;
	
	// Send Alice's ping1 to be received by Bob
	Bob.ping.encr_K = Alice.ping.encr_K;
	Bob.ping.name = Alice.ping.name;
	Bob.ping.g_val = Alice.ping.g_val;
	cout << "			Ping #1 is received by Bob" << endl;

	// Have Bob compute the shared secret (variable called "sharedPaint")
	Bob.sharedPaint = power(Bob.ping.g_val, Bob.chosenPaint, p);
	cout << "Bob's calculated SS: 	" << Bob.sharedPaint << endl;

	// Have Bob decrypt Ka and store value
	char *decrypt1 = (char*)malloc(RSA_size(Bob.keypair));
	RSA_private_decrypt(encrypt1_len, (unsigned char*)encrypt1, (unsigned char*)decrypt1, 			Bob.keypair, RSA_PKCS1_OAEP_PADDING);
	Bob.k_partner = atol(decrypt1);
	cout << "Ka Decryption: 		" << decrypt1 << endl;

	// Have Bob compute g ^ Bob.chosenPaint mod p
	dh_ping = power(g, Bob.chosenPaint, p);
	Bob.g_val_sent = dh_ping;
	cout << "g^B mod p: 		" << dh_ping << endl;

	// Have Bob generate a random string Kb
	Bob.k = (rand()%1000000);
	cout << "Kb: 			" << Bob.k << endl;

	// Have Bob encrypt Kb using Alice's public key
	char msg2[256];
	sprintf(msg2, "%d", Bob.k);
	msg2[strlen(msg2)] = '\0';
	char *encrypt2 = (char*)malloc(RSA_size(Alice.keypair));
	int encrypt2_len;
	encrypt2_len = RSA_public_encrypt(strlen(msg2)+1, (unsigned char*)msg2, 			(unsigned char*)encrypt2, Alice.keypair, RSA_PKCS1_OAEP_PADDING);
	cout << "Kb Encryption: 	" << endl;
    	for(int i=0;i<encrypt2_len;i++)
	{
    	    printf("|%03u|",encrypt2[i]);
    	}
	cout << endl;

	// Have Bob calculate hash(Ka||Kb)
	string B_k_sum = doSHA1(Bob.k + Bob.k_partner);
	cout << "Bob SHA1 Digest:	" << B_k_sum << endl;

	// Have Bob construct the string to be hashed by HMAC
	//Bob.g_val_sent;	// Has Bob's g_val
	//Bob.ping.g_val;	// Has Alice's g_val
	char Bob_g_sent[256];
	char Bob_g_got[256];
	sprintf(Bob_g_sent, "%d", Bob.g_val_sent);
	sprintf(Bob_g_got, "%d", Bob.ping.g_val);
	string B_g_s(Bob_g_sent);
	string B_g_g(Bob_g_got);
	string B_to_HMAC = B_g_s+","+B_g_g+","+"Alice,Bob";	
	cout << "Bob HMAC input string:	" << B_to_HMAC << endl;

	// Have Bob hash the string with HMAC
	string B_HMAC = doHMAC(B_k_sum, B_to_HMAC);
	cout << "Bob HMAC Digest:	" << B_HMAC << endl;
	
	// Have Bob create ping2
	Bob.ping.encr_K = encrypt2;
	Bob.ping.name = "Bob";
	Bob.ping.g_val = dh_ping;
	Bob.ping.HMAC = B_HMAC;
	
	// Send Bob's ping2 to be received by Alice
	Alice.ping.encr_K = Bob.ping.encr_K;
	Alice.ping.name = Bob.ping.name;
	Alice.ping.g_val = Bob.ping.g_val;
	Alice.ping.p_HMAC = Bob.ping.HMAC;
	cout << "			Ping #2 is received by Alice" << endl;

	// Have Alice compute the shared secret (variable called "sharedPaint")
	Alice.sharedPaint = power(Alice.ping.g_val, Alice.chosenPaint, p);
	cout << "Alice's calculated SS:	" << Alice.sharedPaint << endl;

	// Have Alice decrypt Kb and store value
	char *decrypt2 = (char*)malloc(RSA_size(Alice.keypair));
	RSA_private_decrypt(encrypt2_len, (unsigned char*)encrypt2, (unsigned char*)decrypt2, 			Alice.keypair, RSA_PKCS1_OAEP_PADDING);
	Alice.k_partner = atol(decrypt2);
	cout << "Kb Decryption: 		" << decrypt2 << endl;

	// Have Alice calculate hash(Ka||Kb)
	string A_k_sum = doSHA1(Alice.k + Alice.k_partner);
	cout << "Alice SHA1 Digest: 	" << A_k_sum << endl;

	// Have Bob construct the string to be hashed by HMAC
	char Alice_g_sent[256];
	char Alice_g_got[256];
	sprintf(Alice_g_sent, "%d", Alice.g_val_sent);
	sprintf(Alice_g_got, "%d", Alice.ping.g_val);
	string A_g_s(Alice_g_sent);
	string A_g_g(Alice_g_got);
	string A_to_HMAC = A_g_g+","+A_g_s+","+"Alice,Bob";	
	cout << "Alice HMAC input string:" << A_to_HMAC << endl;

	// Have Alice hash the string with HMAC
	string A_HMAC = doHMAC(A_k_sum, A_to_HMAC);
	cout << "Alice HMAC Digest:	" << A_HMAC << endl;
	
	// Have Alice create ping3
	Alice.ping.HMAC = A_HMAC;

	// Send Alice's ping3 to be received by Bob
	Bob.ping.p_HMAC = Alice.ping.HMAC;
	cout << "			Ping #3 is received by Bob" << endl;

	// Have Alice use hash function to create secret key
	string hashkey = doSHA1(Alice.sharedPaint);

	//Alice will store this hashed key
	Alice.chat.create_key((unsigned char *)hashkey.c_str());

	//Have Bob use hash function to create secret key
	string hashkey2 = doSHA1(Bob.sharedPaint);

	//Bob will store this hashed key
	Bob.chat.create_key((unsigned char *)hashkey2.c_str());
	cout << "Alice's AES Key generated from SS:" << endl;
	Alice.chat.printkey();
	cout << "Bob's AES Key generated from SS:" << endl;
	Bob.chat.printkey();

	//Have Alice send Bob first of 2 messages
	strcpy((char *)Alice.plain,"Hello Bob, how are you?");
	int m1len = Alice.chat.encrypt(Alice.plain,
	Bob.cipher);
	//Bob.printcipher(ILIM);
	//Have Bob decrypt the first of 2 messages
	int end1 = Bob.chat.decrypt(Bob.plain,Bob.cipher,m1len);
	//cout << "Bob's encrypted messages received from Alice:" << endl;
	//Bob.printcipher(m1len*KSIZE); //print the cipher if you want to
	cout <<"Alice's first sent message to Bob:" << endl;
	cout << Alice.plain << endl;
	cout <<"Bob's first received message from Alice (Decrypted):" << endl;
	cout << Bob.plain << endl;

	cout << endl;
	//Have Bob send Alice second of 2 messages
	memset(Bob.plain,0,ILIM); //zero Bob's plaintext
	memset(Bob.cipher,0,OLIM); //zero Bob's ciphertext
	memset(Alice.cipher,0,OLIM); //zero Alice's ciphertext
	strcpy((char *)Bob.plain,"I'm doing well Alice, have a good night.");
	int m2len = Bob.chat.encrypt(Bob.plain,
	Alice.cipher);

	//Have Alice decrypt the second of 2 messages
	memset(Alice.plain,0,ILIM); //zero Bob's plaintext
	int end2 = Alice.chat.decrypt(Alice.plain,Alice.cipher,m2len);
	//cout << "Bob's encrypted messages received from Alice:" << endl;
	//Bob.printcipher(m1len*KSIZE); //print the cipher if you want to
	cout <<"Bob's second sent message to Alice:" << endl;
	cout << Bob.plain << endl;
	cout <<"Alice's second received message from Bob (Decrypted):" << endl;
	cout << Alice.plain << endl;
}
