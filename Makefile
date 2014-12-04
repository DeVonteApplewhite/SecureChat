cipherAES: messaging.cpp
	g++ -I/usr/bin/ -o messaging -Wall -lcrypto messaging.cpp
clean:
	rm messaging
