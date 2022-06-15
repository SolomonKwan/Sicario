
FLAGS = g++ -std=c++17 -g -pedantic -Wall -O3 -pthread

all: test

test.o: test.cpp ../src/movegen.hpp ../src/constants.hpp ../src/utils.hpp
	$(FLAGS) -c test.cpp -o test.o

test: test.o ../src/utils.o
	$(FLAGS) test.o ../src/utils.o ../src/movegen.o -o test

clean:
	rm *.o test
