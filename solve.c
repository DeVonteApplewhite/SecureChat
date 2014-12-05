#include <stdio.h>
#include <stdlib.h>

unsigned long int power(unsigned long a,unsigned long n,unsigned long m){
	unsigned long e = n;
	unsigned long y = 1;
	unsigned long z = a;
	//printf("$e = %lu$,",e);
	//printf("$y = %lu$,",y);
	//printf("$z = %lu$\\\\\n",z);

	while(1){
		if(e%2 == 1){ //e is odd
			//printf("$y = (%lu \\cdot %lu)\\%%%lu = ",y,z,m);
			y = (y*z)%m;
			//printf("%lu$,",y);
		}
		if(e<=1){
			return y; //answer
		}
		//printf("$z = (%lu \\cdot %lu)\\%%%lu = ",z,z,m);
		z = (z*z) % m;
		//printf("%lu$,",z);
		//printf("$e = %lu \\gg 1 = ",e);
		e = e >> 1;
		//
		//printf("%lu$\\\\\n",e);
		//printf("\\noindent\\makebox[\\linewidth]{\\rule{\\paperwidth}{0.4pt}}\n");
	}
	return 0;
}

int main(int argc, char *argv[]){
	if(argc != 4){
		printf("Usage %s <a> <n> <m>\n",argv[0]);
		return 1;
	}
	printf("Answer is $%lu$\n",power(atol(argv[1]),atol(argv[2]),atol(argv[3])));

	return 0;
}
