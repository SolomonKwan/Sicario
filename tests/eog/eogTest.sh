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
	runTests "$@"
	clean "$@"
}

# Checks for help flag.
helpCommand() {
	if [[ "$*" == *"-h"* ]]; then
		echo "Usage: ./eogTest.sh test [-h] [-c] [-k]"
		echo "    test: The test to run. One of the directory names."
		echo "    -h: Display usage message."
		echo "    -c: Clean the keep files."
		echo "    -k: Keep files from tests."
		exit 0
	fi
}

# Clean the tmp and keep files manually.
manualClean() {
	if [[ "$*" == *"-c"* ]]; then
		rm -f **/*.tmp
		rm -f **/*.tmp.keep
		exit 0
	fi
}

runTests() {
	number=1
	while IFS= read -r line || [[ -n "$line" ]]; do
		# Generate the input
		for word in $line; do
			echo move $word >> $1/input$number.tmp
		done
		echo state >> $1/input$number.tmp
		echo quit >> $1/input$number.tmp

		# Run the program
		timeout 5s ../../src/sicario > $1/output$number.tmp < $1/input$number.tmp

		if [ $? -eq 124 ]; then
			printf "$ERASE"
			printf "$ERROR $number - Timeout.\n"
		elif cmp -s $1/output$number.tmp $1/output.txt; then
			printf "$ERASE"
			printf "$OK $number\n"
		else
			printf "$ERASE"
			printf "$FAIL $number - Different output.\n"
			mv $1/output$number.tmp $1/output$number.tmp.keep
		fi

		((number++))
	done < $1/inputs.txt

	return 0
}

clean() {
	if [[ "$*" != *"-k"* ]]; then
		rm -f **/*.tmp
	fi
}

main "$@"