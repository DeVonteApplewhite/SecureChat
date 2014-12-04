#include <stdio.h>
#include <stdlib.h>
#include "solve.h"
#include <iomanip>
#include <unistd.h>
#include <time.h>

using namespace std;

human::human(){}			// default constructor
unsigned int human::choosePaint()	// assign random 9 digit int
{
	//unsigned int microseconds = 100000;
	//usleep(microseconds);
	//srand(time(NULL));		// seeds the rand()
	return (rand()%1000000000);
}

unsigned int findPrime()
{
	while(1)
	{
		unsigned int start = (rand()%1000000000);

		for(start; start<=999999999; start++)
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
		
			if(count==0 && start != 1)
			{
				return start;
			}
		}
	}
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

int main(int argc, char *argv[]){

	// Seed the random number generator
	srand(time(NULL));	

	// Create Alice and Bob
	human Alice;
	human Bob;

	// Have Alice and Bob choose paint
	Alice.chosenPaint = Alice.choosePaint();
	Bob.chosenPaint = Bob.choosePaint();
	cout << Alice.chosenPaint << endl;
	cout << Bob.chosenPaint << endl;

	// Have Alice and Bob get eachother's paint
	Alice.partnersPaint = Bob.chosenPaint;
	Bob.partnersPaint = Alice.chosenPaint;
	cout << Alice.partnersPaint << endl;
	cout << Bob.partnersPaint << endl;

	// Have the chat program generate a prime "p" and a primitive root mod p "g"
	unsigned int p = findPrime();
	cout << p << endl;






	if(argc != 4){
		printf("Usage %s <a> <n> <m>\n",argv[0]);
		return 1;
	}
	printf("Answer is $%lu$\n",power(atol(argv[1]),atol(argv[2]),atol(argv[3])));

	return 0;
}
