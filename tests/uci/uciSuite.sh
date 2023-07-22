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
	testName=$(getTestName "$@")
	staticTest "$testName" UCI
	variableTest "$testName" DEBUG
	staticTest "$testName" ISREADY
	staticTest "$testName" SETOPTION
	staticTest "$testName" UCINEWGAME
	staticTest "$testName" POSITION
	variableTest "$testName" GO
	variableTest "$testName" STOP
	notImplemented PONDERHIT
	variableTest "$testName" QUIT

	# Test custom UCI commands
	staticTest "$testName" PERFT
	staticTest "$testName" MOVE
	staticTest "$testName" UNDO
	staticTest "$testName" DISPLAY
	staticTest "$testName" MOVES
	staticTest "$testName" BITBOARDS
	variableTest "$testName" RANDOMGAME
	staticTest "$testName" STATE
	staticTest "$testName" OPTIONS
	staticTest "$testName" DATA
	variableTest "$testName" HASHCOUNT
	variableTest "$testName" LETTERMODE
	staticTest "$testName" INVALID_COMMAND

	# Test multiple commands
	variableTest "$testName" MULTIPLE

	# Clean up
	clean "$@"
}

# Checks for help flag.
helpCommand() {
	if [[ "$*" == *"-h"* ]]; then
		echo "Usage: ./uciSuite.sh -h -c -k -t testName"
		echo "    -h: Display usage message."
		echo "    -c: Clean the keep files."
		echo "    -k: Keep files from tests."
		echo "    -t: Run [testName] only."
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

getTestName() {
	while getopts ":t:" opt; do
	case $opt in
		t)
			echo $OPTARG
			;;
	esac
	done
}

staticTest() {
	if [ ! -z $1 ] && [ ! $1 = $2 ]; then
		return
	fi

	printf "[  --  ] Testing $2..."

	# Check if necessary files exist.
	if ! filesExist $2; then
		return 1
	fi

	# Parse the delay
	if [ $# -lt 3 ]; then
		DELAY=5
	else
		DELAY=$3
	fi

	# Run and compare result
	timeout $DELAY ../../src/sicario > output/$2.tmp < input/$2.txt
	if [ $? -eq 124 ]; then
		printf "$ERASE"
		printf "$ERROR $2 - Timeout.\n"
	elif cmp -s output/$2.tmp output/$2.txt; then
		printf "$ERASE"
		printf "$OK $2\n"
	else
		printf "$ERASE"
		printf "$FAIL $2 - Different output.\n"
		mv output/$2.tmp output/$2.tmp.keep
	fi
}

variableTest() {
	if [ ! -z $1 ] && [ ! $1 = $2 ]; then
		return
	fi

	printf "[  --  ] Testing $2..."

	# Check if necessary files exist.
	if ! filesExist $2; then
		return 1
	fi

	# Set up named pipe
	mkfifo fifo
	../../src/sicario > output/$2.tmp < fifo &
	sicariopid=$!

	# Run and input commands
	exec 3>fifo
	while IFS= read -r line || [[ -n "$line" ]]; do
		if [[ $line == "delay "* ]]; then
			sleep $(cut -d " " -f2- <<< "$line")
		else
			echo $line >&3
		fi
	done < input/$2.txt
	exec 3>&-

	# Clean up
	rm fifo

	# Check if process has ended
	sleep 1
	if kill -9 $sicariopid &> /dev/null; then
		printf "$ERASE"
		printf "$ERROR $2 - Sicario process was not ended correctly.\n"
		return
	fi

	# Compare and show results
	python3 compare.py $2
	if [ $? -eq 0 ]; then
		printf "$ERASE"
		printf "$OK $2\n"
	else
		printf "$ERASE"
		printf "$FAIL $2 - Different output.\n"
		mv output/$2.tmp output/$2.tmp.keep
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