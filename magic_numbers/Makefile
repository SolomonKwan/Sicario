all: generate

movegen.o: ../src/movegen.cpp ../src/movegen.hpp ../src/constants.hpp ../src/bitboard.hpp ../src/utils.hpp ../src/game.hpp
	g++ -std=c++20 -g -pedantic -Wall -O3 -c ../src/movegen.cpp -o movegen.o

generate: generate_magic_numbers.cpp movegen.o
	g++ -std=c++20 -g -pedantic -Wall -O3 generate_magic_numbers.cpp movegen.o -o generate

clean:
	rm generate movegen.o