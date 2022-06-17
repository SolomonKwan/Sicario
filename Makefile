SRC = ./src
TEST = ./tests
GENERATE = ./magic_numbers
CC = g++
FLAGS = g++ -std=c++20 -g -pedantic -Wall -O3 -pthread

all: sicario generate test

movegen.o: $(SRC)/movegen.cpp $(SRC)/movegen.hpp $(SRC)/constants.hpp $(SRC)/game.hpp
	$(FLAGS) -c $(SRC)/movegen.cpp -o $(SRC)/movegen.o

game.o: $(SRC)/game.cpp $(SRC)/game.hpp $(SRC)/constants.hpp $(SRC)/movegen.hpp $(SRC)/utils.hpp
	$(FLAGS) -c $(SRC)/game.cpp -o $(SRC)/game.o

mcts.o: $(SRC)/mcts.cpp $(SRC)/mcts.hpp $(SRC)/constants.hpp $(SRC)/movegen.hpp
	$(FLAGS) -c $(SRC)/mcts.cpp -o $(SRC)/mcts.o

uci.o: $(SRC)/uci.cpp $(SRC)/uci.hpp $(SRC)/utils.hpp
	$(FLAGS) -c $(SRC)/uci.cpp -o $(SRC)/uci.o

utils.o: $(SRC)/utils.cpp $(SRC)/utils.hpp
	$(FLAGS) -c $(SRC)/utils.cpp -o $(SRC)/utils.o

sicario.o: $(SRC)/sicario.cpp $(SRC)/constants.hpp
	$(FLAGS) -c $(SRC)/sicario.cpp -o $(SRC)/sicario.o

test.o: $(TEST)/test.cpp
	$(FLAGS) -c $(TEST)/test.cpp -o $(TEST)/test.o

main.o: $(SRC)/main.cpp $(SRC)/movegen.hpp $(SRC)/game.hpp $(SRC)/uci.hpp $(SRC)/sicario.hpp
	$(FLAGS) -c $(SRC)/main.cpp -o $(SRC)/main.o

generate: $(GENERATE)/generate_magic_numbers.cpp
	$(FLAGS) $(GENERATE)/generate_magic_numbers.cpp -o $(GENERATE)/generate

test: $(TEST)/test.o $(SRC)/utils.o
	$(FLAGS) $(TEST)/test.o $(SRC)/utils.o $(SRC)/movegen.o -o $(TEST)/test

sicario: $(SRC)/main.o $(SRC)/game.o $(SRC)/movegen.o $(SRC)/uci.o $(SRC)/sicario.o $(SRC)/utils.o $(SRC)/mcts.o
	$(FLAGS) $(SRC)/main.o $(SRC)/game.o $(SRC)/movegen.o $(SRC)/uci.o $(SRC)/sicario.o $(SRC)/utils.o $(SRC)/mcts.o -o $(SRC)/sicario

clean-test:
	rm $(TEST)/*.o $(TEST)/test

clean-generate:
	rm $(GENERATE)/generate

clean:
	rm $(SRC)/*.o $(TEST)/*.o $(SRC)/sicario $(GENERATE)/generate $(TEST)/test