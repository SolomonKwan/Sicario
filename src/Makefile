
FLAGS = g++ -std=c++17 -g -pedantic -Wall -O3 -pthread

all: sicario test

movegen.o: movegen.cpp movegen.hpp constants.hpp game.hpp
	$(FLAGS) -c movegen.cpp -o movegen.o

game.o: game.cpp game.hpp constants.hpp movegen.hpp utils.hpp
	$(FLAGS) -c game.cpp -o game.o

mcts.o: mcts.cpp mcts.hpp constants.hpp movegen.hpp
	$(FLAGS) -c mcts.cpp -o mcts.o

uci.o: uci.cpp uci.hpp utils.hpp
	$(FLAGS) -c uci.cpp -o uci.o

utils.o: utils.cpp utils.hpp
	$(FLAGS) -c utils.cpp -o utils.o

sicario.o: constants.hpp
	$(FLAGS) -c sicario.cpp -o sicario.o

main.o: main.cpp movegen.hpp game.hpp uci.hpp sicario.hpp
	$(FLAGS) -c main.cpp -o main.o

sicario: main.o game.o movegen.o uci.o sicario.o utils.o mcts.o
	$(FLAGS) main.o game.o movegen.o uci.o sicario.o utils.o mcts.o -o sicario



test.o: test.cpp movegen.hpp constants.hpp
	$(FLAGS) -c test.cpp -o test.o

test: test.o movegen.o
	$(FLAGS) test.o movegen.o -o test



clean:
	rm *.o sicario test
