# Optimise flag
ifeq ($(OPTIMISE), 1)
    OPTIMISE_FLAG = -O1
else ifeq ($(OPTIMISE), 2)
    OPTIMISE_FLAG = -O2
else ifeq ($(OPTIMISE), 3)
    OPTIMISE_FLAG = -O3
else ifneq ($(OPTIMISE),)
    $(error Unknown OPTIMISE flag: "$(OPTIMISE)")
endif
OPTIONS = $(OPTIMISE_FLAG)

# Debugging flag
NDEBUG := FALSE
ifeq ($(NDEBUG), TRUE)
    OPTIONS := $(OPTIONS) -DNDEBUG
else ifneq ($(NDEBUG), FALSE)
    $(error Unknown NDEBUG flag: "$(NDEBUG)")
endif

# PEXT flag
PEXT := FALSE
ifeq ($(PEXT), TRUE)
    OPTIONS := $(OPTIONS) -DUSE_PEXT -march=native
else ifneq ($(PEXT), FALSE)
    $(error Unknown PEXT flag: "$(PEXT)")
endif

# Profiling flag
GPROF := FALSE
ifeq ($(GPROF), TRUE)
    OPTIONS := $(OPTIONS) -pg
else ifneq ($(GPROF), FALSE)
    $(error Unknown GPROF flag: "$(GPROF)")
endif

# Relative directories
SRC_DIR = ./src
TEST_DIR = ./tests
GENERATE_DIR = ./magic_numbers

# Compilation flags
CC = g++
CFLAGS = -std=c++20 -g -pedantic -Wall $(OPTIONS)
THREAD = -pthread

all: test sicario generate

movegen.o: $(SRC_DIR)/movegen.cpp $(SRC_DIR)/movegen.hpp $(SRC_DIR)/game.hpp $(SRC_DIR)/utils.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/movegen.cpp -o $(SRC_DIR)/movegen.o

game.o: $(SRC_DIR)/game.cpp $(SRC_DIR)/game.hpp $(SRC_DIR)/constants.hpp $(SRC_DIR)/utils.hpp $(SRC_DIR)/bitboard.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/game.cpp -o $(SRC_DIR)/game.o

search.o: $(SRC_DIR)/search.cpp $(SRC_DIR)/search.hpp $(SRC_DIR)/constants.hpp $(SRC_DIR)/movegen.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/search.cpp -o $(SRC_DIR)/search.o

uci.o: $(SRC_DIR)/uci.cpp $(SRC_DIR)/uci.hpp $(SRC_DIR)/utils.hpp
	$(CC) $(CFLAGS) $(THREAD) -c $(SRC_DIR)/uci.cpp -o $(SRC_DIR)/uci.o

utils.o: $(SRC_DIR)/utils.cpp $(SRC_DIR)/utils.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/utils.cpp -o $(SRC_DIR)/utils.o

mcts.o: $(SRC_DIR)/mcts.cpp $(SRC_DIR)/mcts.hpp $(SRC_DIR)/search.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/mcts.cpp -o $(SRC_DIR)/mcts.o

sicario.o: $(SRC_DIR)/sicario.cpp $(SRC_DIR)/constants.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/sicario.cpp -o $(SRC_DIR)/sicario.o

test.o: $(TEST_DIR)/test.cpp
	$(CC) $(CFLAGS) -c $(TEST_DIR)/test.cpp -o $(TEST_DIR)/test.o

main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/movegen.hpp $(SRC_DIR)/game.hpp $(SRC_DIR)/uci.hpp $(SRC_DIR)/sicario.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(SRC_DIR)/main.o

test: test.o utils.o movegen.o $(SRC_DIR)/movegen.hpp
	$(CC) $(CFLAGS) $(TEST_DIR)/test.o $(SRC_DIR)/utils.o $(SRC_DIR)/movegen.o -o $(TEST_DIR)/test

generate: $(GENERATE_DIR)/generate_magic_numbers.cpp
	$(CC) $(CFLAGS) $(GENERATE_DIR)/generate_magic_numbers.cpp $(SRC_DIR)/movegen.o -o $(GENERATE_DIR)/generate

sicario: main.o game.o movegen.o uci.o sicario.o utils.o search.o mcts.o
	$(CC) $(CFLAGS) $(SRC_DIR)/*.o -o $(SRC_DIR)/sicario

clean-test:
	rm $(TEST_DIR)/*.o $(TEST_DIR)/test

clean-generate:
	rm $(GENERATE_DIR)/generate

clean:
	rm $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(SRC_DIR)/sicario $(GENERATE_DIR)/generate $(TEST_DIR)/test