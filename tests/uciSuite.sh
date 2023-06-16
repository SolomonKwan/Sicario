#!/bin/bash

GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RED="\033[0;31m"
NC="\033[0m"

OK="[  ${GREEN}OK${NC}  ]"
FAIL="[ ${RED}FAIL${NC} ]"
ERROR="[ ${YELLOW}ERRO${NC} ]"

ERASE="\033[1K\r"

main() {
	helpCommand "$@"
	manualClean "$@"

	# Test UCI commands
	echo "Testing UCI commands"
	staticTest UCI
	staticTest DEBUG
	staticTest ISREADY
	staticTest SETOPTION
	staticTest UCINEWGAME
	staticTest POSITION
	variableTest GO
	variableTest STOP
	variableTest PONDERHIT
	variableTest QUIT

	# Test custom UCI commands
	staticTest PERFT
	staticTest MOVE
	staticTest UNDO
	staticTest DISPLAY
	staticTest MOVES
	staticTest BITBOARDS
	staticTest RANDOMGAME
	staticTest STATE
	staticTest OPTIONS
	staticTest DATA
	staticTest INVALID_COMMAND

	# Test multiple commands
	staticTest MULTIPLE

	# Clean up
	clean
}

# Checks for help flag.
helpCommand() {
	if [[ "$*" == *"-h"* ]]
	then
		echo "Usage: ./uciSuite.sh [-h]"
		echo "    -h: Display usage message."
		echo "    -k: Clean the keep files."
		exit 0
	fi
}

# Clean the tmp and keep files manually.
manualClean() {
	if [[ "$*" == *"-k"* ]]
	then
		rm -f output/*.tmp
		rm -f output/*.tmp.keep
		exit 0
	fi
}

staticTest() {
	printf "[  --  ] Testing $1..."

	# Check if necessary files exist.
	if ! filesExist $1
	then
		printf "$ERASE"
		printf "$ERROR Input and/or output files (\"$1.txt\") do not exist.\n"
		return 1
	fi

	# Parse the delay
	if [ $# -lt 2 ]
	then
		DELAY=5
	else
		DELAY=$2
	fi

	# Run and compare result
	timeout $DELAY ../src/sicario > output/$1.tmp < input/$1.txt
	if [ $? -eq 124 ]
	then
		printf "$ERASE"
		printf "$FAIL $1 - Timeout.\n"
	elif cmp -s output/$1.tmp output/$1.txt;
	then
		printf "$ERASE"
		printf "$OK $1\n"
	else
		printf "$ERASE"
		printf "$FAIL $1 - Different output.\n"
		mv output/$1.tmp output/$1.tmp.keep
	fi
}

variableTest() {
	printf "$ERROR $1 - Not implemented\n"
}

filesExist() {
	[ -f output/$1.txt ] || [ -f input/$1.txt ]
}

clean() {
	rm -f output/*.tmp
}

main "$@"