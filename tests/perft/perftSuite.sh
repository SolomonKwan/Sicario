#!/bin/bash

GREEN="\033[0;32m"
RED="\033[0;31m"
NC="\033[0m"

main() {
	# Parse input
	helpCommand "$@"
	parseInputs "$@"
	local depth=$(getDepth "$@")

	# Perform checks
	local totalTests=0
	local passedTests=0
	while IFS= read -r line; do
		((totalTests++))
		local FEN="${line%%;*}"
		local actualVal=$(echo $line | grep -Eo "D$depth [^;]*(| ;)" | grep -Eo "\b[0-9]+\b")
		local val=$(sicario "$FEN" $depth | cut -c 17-)
		if [[ $val -eq $actualVal ]]; then
			printf "[  ${GREEN}OK${NC}  ] ${FEN}\n"
			((passedTests++))
		else
			printf "[ ${RED}FAIL${NC} ] ${FEN}\n"
		fi
	done < ./perftsuite.epd

	# Show end result
	if [[ $passedTests -eq $totalTests ]]; then
		echo "Perft suite testing OK"
	else
		printf "${GREEN}OK${NC}: $((passedTests))\n"
		printf "${RED}FAIL${NC}: $((totalTests - passedTests))\n"
	fi
	rm res.tmp
}

# Prints the usage message.
showUsageMessage() {
	echo "Usage: ./perftsuite depth [-h]"
	echo "    depth: The depth to perform the perft to. Any integer from 1 to 6."
	echo "    -h: Display usage message."
}

# Checks for help flag.
helpCommand() {
	if [[ "$*" == *"-h"* ]]; then
		showUsageMessage
		exit 0
	fi
}

# Checks the depth input and returns the FEN string.
parseInputs() {
	if  [ ! -z $1 ] && ([[ ! $1 =~ ^[0-9]+$ ]] || [ $1 -gt 6 ] || [ $1 -lt 1 ]); then
		echo "Invalid input"
		showUsageMessage
		exit -1
	fi
}

# Get the depth.
getDepth() {
	local depth="6"
	if [ ! -z $1 ]; then
		depth=$1
	fi
	echo $depth
}

# Runs the perft command on sicario and returns the result.
sicario() {
	../../src/sicario > ./res.tmp <<-EOF
		position fen $1
		perft $2
		quit
	EOF
	local res=$(tail -n 1 res.tmp)
	echo $res
}

main "$@"