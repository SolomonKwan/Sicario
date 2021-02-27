
FLAGS = g++ -std=c++17 -g -pedantic -Wall -O3 -pthread

all: sicario

movegen.o: movegen.cpp movegen.hpp constants.hpp game.hpp
	$(FLAGS) -c movegen.cpp -o movegen.o

game.o: game.cpp game.hpp constants.hpp movegen.hpp search.hpp
	$(FLAGS) -c game.cpp -o game.o

search.o: search.cpp search.hpp constants.hpp movegen.hpp
	$(FLAGS) -c search.cpp -o search.o

evaluate.o: evaluate.cpp evaluate.hpp constants.hpp
	$(FLAGS) -c evaluate.cpp -o evaluate.o

uci.o: uci.cpp uci.hpp
	$(FLAGS) -c uci.cpp -o uci.o

main.o: main.cpp movegen.hpp game.hpp search.hpp evaluate.hpp uci.hpp
	$(FLAGS) -c main.cpp -o main.o

sicario: main.o game.o movegen.o search.o evaluate.o uci.o
	$(FLAGS) main.o game.o movegen.o search.o evaluate.o uci.o -o sicario

clean:
	rm *.o sicario
