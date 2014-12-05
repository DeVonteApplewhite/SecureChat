all:	main

main: solve.o
	g++ -I/usr/bin/ -lcrypto solve.o -o main

solve.o: solve.cpp
	g++ -I/usr/bin/ -lcrypto -c solve.cpp

clean:
	rm -f *.o main
