
FLAGS = g++ -std=c++17 -g -pedantic -Wall
OBJ = ../obj

all: main

$(OBJ)/movegen.o: movegen.cpp movegen.hpp constants.hpp rook_moves.hpp \
		bishop_moves.hpp knight_moves.hpp king_moves.hpp game.hpp
	$(FLAGS) -c movegen.cpp -o $(OBJ)/movegen.o

$(OBJ)/rook_moves.o: rook_moves.cpp rook_moves.hpp constants.hpp movegen.hpp
	$(FLAGS) -c rook_moves.cpp -o $(OBJ)/rook_moves.o

$(OBJ)/bishop_moves.o: bishop_moves.cpp bishop_moves.hpp constants.hpp \
		movegen.hpp
	$(FLAGS) -c bishop_moves.cpp -o $(OBJ)/bishop_moves.o

$(OBJ)/knight_moves.o: knight_moves.cpp knight_moves.hpp constants.hpp \
		movegen.hpp
	$(FLAGS) -c knight_moves.cpp -o $(OBJ)/knight_moves.o

$(OBJ)/king_moves.o: king_moves.cpp king_moves.hpp constants.hpp movegen.hpp
	$(FLAGS) -c king_moves.cpp -o $(OBJ)/king_moves.o

$(OBJ)/pawn_moves.o: pawn_moves.cpp pawn_moves.hpp constants.hpp movegen.hpp
	$(FLAGS) -c pawn_moves.cpp -o $(OBJ)/pawn_moves.o

$(OBJ)/fen.o: fen.cpp fen.hpp constants.hpp movegen.hpp
	$(FLAGS) -c fen.cpp -o $(OBJ)/fen.o

$(OBJ)/game.o: game.cpp game.hpp constants.hpp movegen.hpp fen.hpp arg.hpp
	$(FLAGS) -c game.cpp -o $(OBJ)/game.o

$(OBJ)/arg.o: arg.cpp arg.hpp game.hpp fen.hpp
	$(FLAGS) -c arg.cpp -o $(OBJ)/arg.o

$(OBJ)/test.o: test.cpp test.hpp constants.hpp game.hpp fen.hpp
	$(FLAGS) -c test.cpp -o $(OBJ)/test.o

$(OBJ)/search.o: search.cpp search.hpp constants.hpp evaluate.hpp
	$(FLAGS) -c search.cpp -o $(OBJ)/search.o

$(OBJ)/evaluate.o: evaluate.cpp evaluate.hpp constants.hpp
	$(FLAGS) -c evaluate.cpp -o $(OBJ)/evaluate.o

$(OBJ)/uci.o: uci.cpp uci.hpp
	$(FLAGS) -c uci.cpp -o $(OBJ)/uci.o

$(OBJ)/main.o: main.cpp movegen.hpp rook_moves.hpp test.hpp game.hpp \
		search.hpp evaluate.hpp uci.hpp
	$(FLAGS) -c main.cpp -o $(OBJ)/main.o

main: $(OBJ)/main.o $(OBJ)/rook_moves.o $(OBJ)/bishop_moves.o \
		$(OBJ)/knight_moves.o $(OBJ)/king_moves.o $(OBJ)/pawn_moves.o \
		$(OBJ)/fen.o $(OBJ)/game.o $(OBJ)/movegen.o $(OBJ)/test.o \
		$(OBJ)/arg.o $(OBJ)/search.o $(OBJ)/evaluate.o $(OBJ)/uci.o
	$(FLAGS) $(OBJ)/main.o $(OBJ)/rook_moves.o $(OBJ)/bishop_moves.o \
			$(OBJ)/knight_moves.o $(OBJ)/king_moves.o $(OBJ)/pawn_moves.o \
			$(OBJ)/fen.o $(OBJ)/game.o $(OBJ)/movegen.o $(OBJ)/test.o \
			$(OBJ)/arg.o $(OBJ)/search.o $(OBJ)/evaluate.o $(OBJ)/uci.o \
			-o tardigrade

clean:
	rm -rf *.o tardigrade
