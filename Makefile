all: messaging

messaging: messaging.o main.o
	g++ -I/usr/bin/ -Wall -lcrypto messaging.o main.o -o messaging
messaging.o: messaging.cpp
	g++ -I/usr/bin/ -Wall -lcrypto -c messaging.cpp
main.o: main.cpp
	g++ -c main.cpp
clean:
	rm -f messaging *.o
