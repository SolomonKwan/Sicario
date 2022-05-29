
FLAGS = g++ -std=c++17 -g -pedantic -Wall -O3 -pthread

all: sicario

movegen.o: movegen.cpp movegen.hpp constants.hpp game.hpp
	$(FLAGS) -c movegen.cpp -o movegen.o

game.o: game.cpp game.hpp constants.hpp movegen.hpp search.hpp utils.hpp
	$(FLAGS) -c game.cpp -o game.o

mcts.o: mcts.cpp mcts.hpp constants.hpp movegen.hpp
	$(FLAGS) -c mcts.cpp -o mcts.o

search.o: search.cpp search.hpp constants.hpp movegen.hpp mcts.hpp game.hpp
	$(FLAGS) -c search.cpp -o search.o

evaluate.o: evaluate.cpp evaluate.hpp constants.hpp
	$(FLAGS) -c evaluate.cpp -o evaluate.o

uci.o: uci.cpp uci.hpp utils.hpp
	$(FLAGS) -c uci.cpp -o uci.o

utils.o: utils.cpp utils.hpp
	$(FLAGS) -c utils.cpp -o utils.o

sicario.o: constants.hpp
	$(FLAGS) -c sicario.cpp -o sicario.o

main.o: main.cpp movegen.hpp game.hpp search.hpp evaluate.hpp uci.hpp sicario.hpp
	$(FLAGS) -c main.cpp -o main.o

sicario: main.o game.o movegen.o search.o evaluate.o uci.o mcts.o sicario.o utils.o
	$(FLAGS) main.o game.o movegen.o search.o evaluate.o uci.o mcts.o sicario.o utils.o -o sicario

clean:
	rm *.o sicario
