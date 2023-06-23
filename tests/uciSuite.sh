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
	variableTest DEBUG
	staticTest ISREADY
	staticTest SETOPTION
	staticTest UCINEWGAME
	staticTest POSITION
	variableTest GO
	variableTest STOP
	notImplemented PONDERHIT
	variableTest QUIT

	# Test custom UCI commands
	staticTest PERFT
	staticTest MOVE
	staticTest UNDO
	staticTest DISPLAY
	staticTest MOVES
	staticTest BITBOARDS
	staticTest RANDOMGAME
	# TODO Add a bunch of different drawing conditions including manual moves made for 3 fold repetitions
	staticTest STATE
	staticTest OPTIONS
	staticTest DATA
	staticTest INVALID_COMMAND

	# Test multiple commands
	variableTest MULTIPLE

	# Clean up
	clean "$@"
}

# Checks for help flag.
helpCommand() {
	if [[ "$*" == *"-h"* ]]; then
		echo "Usage: ./uciSuite.sh [-h]"
		echo "    -h: Display usage message."
		echo "    -c: Clean the keep files."
		echo "    -k: Keep files from tests."
		exit 0
	fi
}

# Clean the tmp and keep files manually.
manualClean() {
	if [[ "$*" == *"-c"* ]]; then
		rm -f output/*.tmp
		rm -f output/*.tmp.keep
		exit 0
	fi
}

staticTest() {
	printf "[  --  ] Testing $1..."

	# Check if necessary files exist.
	if ! filesExist $1; then
		return 1
	fi

	# Parse the delay
	if [ $# -lt 2 ]; then
		DELAY=5
	else
		DELAY=$2
	fi

	# Run and compare result
	timeout $DELAY ../src/sicario > output/$1.tmp < input/$1.txt
	if [ $? -eq 124 ]; then
		printf "$ERASE"
		printf "$ERROR $1 - Timeout.\n"
	elif cmp -s output/$1.tmp output/$1.txt; then
		printf "$ERASE"
		printf "$OK $1\n"
	else
		printf "$ERASE"
		printf "$FAIL $1 - Different output.\n"
		mv output/$1.tmp output/$1.tmp.keep
	fi
}

variableTest() {
	printf "[  --  ] Testing $1..."

	# Check if necessary files exist.
	if ! filesExist $1; then
		return 1
	fi

	# Set up named pipe
	mkfifo fifo
	../src/sicario > output/$1.tmp < fifo &
	sicariopid=$!

	# Run and input commands
	exec 3>fifo
	while IFS= read -r line || [[ -n "$line" ]]; do
		if [[ $line == "delay "* ]]; then
			sleep $(cut -d " " -f2- <<< "$line")
		else
			echo $line >&3
		fi
	done < input/$1.txt
	exec 3>&-

	# Clean up
	rm fifo

	# Check if process has ended
	sleep 1
	if kill -9 $sicariopid &> /dev/null; then
		printf "$ERASE"
		printf "$ERROR $1 - Sicario process was not ended correctly.\n"
		return
	fi

	# Compare and show results
	python3 compare.py $1
	if [ $? -eq 0 ]; then
		printf "$ERASE"
		printf "$OK $1\n"
	else
		printf "$ERASE"
		printf "$FAIL $1 - Different output.\n"
		mv output/$1.tmp output/$1.tmp.keep
	fi

	return 0
}

notImplemented() {
	printf "$ERROR $1 - Not implemented.\n"
}

filesExist() {
	if ! [ -f output/$1.txt ] || ! [ -f input/$1.txt ]; then
		printf "$ERASE"
		printf "$ERROR Input and/or output files (\"$1.txt\") do not exist.\n"
		return 1
	fi
	return 0
}

clean() {
	if [[ "$*" != *"-k"* ]]; then
		rm -f output/*.tmp
	fi
}

main "$@"