all:	main

main: solve.o
	g++ solve.o -o main

solve.o: solve.cpp
	g++ -c solve.cpp

clean:
	rm -f *.o main
