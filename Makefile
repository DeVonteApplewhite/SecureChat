all:	main

main: solve.o messaging.o
	g++ -I/usr/bin/ -lcrypto solve.o messaging.o -o main

solve.o: solve.cpp messaging.cpp
	g++ -I/usr/bin/ -lcrypto -c solve.cpp messaging.cpp

messaging.o: messaging.cpp
	g++ -I/usr/bin/ -Wall -lcrypto -c messaging.cpp

clean:
	rm -f *.o main
