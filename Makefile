ifeq ($(OPTIMISE), TRUE)
	OPTIONS = -O3
endif

ifeq ($(NDEBUG), TRUE)
	OPTIONS := $(OPTIONS) -DNDEBUG
endif

ifeq ($(PEXT), TRUE)
	OPTIONS := $(OPTIONS) -DUSE_PEXT -march=native
endif

SRC = ./src
TEST = ./tests
GENERATE = ./magic_numbers

CC = g++
CFLAGS = -std=c++20 -g -pedantic -Wall $(OPTIONS)
THREAD = -pthread

all: test sicario generate

movegen.o: $(SRC)/movegen.cpp $(SRC)/movegen.hpp $(SRC)/constants.hpp $(SRC)/game.hpp
	$(CC) $(CFLAGS) -c $(SRC)/movegen.cpp -o $(SRC)/movegen.o

game.o: $(SRC)/game.cpp $(SRC)/game.hpp $(SRC)/constants.hpp $(SRC)/movegen.hpp $(SRC)/utils.hpp $(SRC)/bitboard.hpp
	$(CC) $(CFLAGS) -c $(SRC)/game.cpp -o $(SRC)/game.o

search.o: $(SRC)/search.cpp $(SRC)/search.hpp $(SRC)/constants.hpp $(SRC)/movegen.hpp
	$(CC) $(CFLAGS) -c $(SRC)/search.cpp -o $(SRC)/search.o

uci.o: $(SRC)/uci.cpp $(SRC)/uci.hpp $(SRC)/utils.hpp
	$(CC) $(CFLAGS) $(THREAD) -c $(SRC)/uci.cpp -o $(SRC)/uci.o

utils.o: $(SRC)/utils.cpp $(SRC)/utils.hpp
	$(CC) $(CFLAGS) -c $(SRC)/utils.cpp -o $(SRC)/utils.o

sicario.o: $(SRC)/sicario.cpp $(SRC)/constants.hpp
	$(CC) $(CFLAGS) -c $(SRC)/sicario.cpp -o $(SRC)/sicario.o

test.o: $(TEST)/test.cpp
	$(CC) $(CFLAGS) -c $(TEST)/test.cpp -o $(TEST)/test.o

main.o: $(SRC)/main.cpp $(SRC)/movegen.hpp $(SRC)/game.hpp $(SRC)/uci.hpp $(SRC)/sicario.hpp
	$(CC) $(CFLAGS) -c $(SRC)/main.cpp -o $(SRC)/main.o

test: test.o utils.o movegen.o $(SRC)/movegen.hpp
	$(CC) $(CFLAGS) $(TEST)/test.o $(SRC)/utils.o $(SRC)/movegen.o -o $(TEST)/test

generate: $(GENERATE)/generate_magic_numbers.cpp
	$(CC) $(CFLAGS) $(GENERATE)/generate_magic_numbers.cpp $(SRC)/movegen.o -o $(GENERATE)/generate

sicario: main.o game.o movegen.o uci.o sicario.o utils.o search.o
	$(CC) $(CFLAGS) $(SRC)/main.o $(SRC)/game.o $(SRC)/movegen.o $(SRC)/uci.o $(SRC)/sicario.o $(SRC)/utils.o $(SRC)/search.o -o $(SRC)/sicario

clean-test:
	rm $(TEST)/*.o $(TEST)/test

clean-generate:
	rm $(GENERATE)/generate

clean:
	rm $(SRC)/*.o $(TEST)/*.o $(SRC)/sicario $(GENERATE)/generate $(TEST)/test