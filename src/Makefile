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

target := sicario
objs := game.o main.o mcts.o movegen.o search.o sicario.o uci.o utils.o

CXX := g++
CXXFLAGS := -std=c++20 -g -pedantic -Wall -pthread $(OPTIONS)

all: $(target)

deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

sicario: $(objs)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< $(DEPFLAGS)

clean:
	rm $(target) $(objs) $(deps)