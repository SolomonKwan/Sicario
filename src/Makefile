
FLAGS = g++ -std=c++17 -g -pedantic -Wall
OBJ = ../obj

all: main

$(OBJ)/movegen.o: movegen.cpp movegen.hpp constants.hpp game.hpp
	$(FLAGS) -c movegen.cpp -o $(OBJ)/movegen.o

$(OBJ)/game.o: game.cpp game.hpp constants.hpp movegen.hpp
	$(FLAGS) -c game.cpp -o $(OBJ)/game.o

$(OBJ)/search.o: search.cpp search.hpp constants.hpp evaluate.hpp
	$(FLAGS) -c search.cpp -o $(OBJ)/search.o

$(OBJ)/evaluate.o: evaluate.cpp evaluate.hpp constants.hpp
	$(FLAGS) -c evaluate.cpp -o $(OBJ)/evaluate.o

$(OBJ)/uci.o: uci.cpp uci.hpp
	$(FLAGS) -c uci.cpp -o $(OBJ)/uci.o

$(OBJ)/main.o: main.cpp movegen.hpp game.hpp search.hpp evaluate.hpp uci.hpp
	$(FLAGS) -c main.cpp -o $(OBJ)/main.o

main: $(OBJ)/main.o $(OBJ)/game.o $(OBJ)/movegen.o $(OBJ)/search.o $(OBJ)/evaluate.o $(OBJ)/uci.o
	$(FLAGS) $(OBJ)/main.o $(OBJ)/game.o $(OBJ)/movegen.o $(OBJ)/search.o $(OBJ)/evaluate.o \
			$(OBJ)/uci.o -o sicario

clean:
	rm $(OBJ)/*.o sicario
