all: proj2.o
	g++ -g -pthread -o proj2 proj2.o 

proj2.o: proj2.cpp utilities.cpp 
	g++ -Wall -g -O2 -c -pthread proj2.cpp

clean:
	rm -rf proj2 *.o

