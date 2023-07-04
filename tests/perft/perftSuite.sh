#!/bin/bash

GREEN="\033[0;32m"
RED="\033[0;31m"
NC="\033[0m"

main() {
	# Parse input
	helpCommand "$@"
	updateDepths "$@"
	parseInputs "$@"
	runTests "$@"
	runMaxTests "$@"
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
	depth="6"
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
	res=$(tail -n 1 res.tmp)
	res=$(echo $res | cut -c 17-)
	echo $res
}

# Update the perftsuite.epd file with the depth value if they are missing.
# Does not update it with depths that have a count with greater than 10 digits. Takes too long.
updateDepths() {
	if ! [[ "$*" == *"-u"* ]]; then
		return
	fi

	touch ./perftsuite.epd.updated
	while IFS= read -r line || [ -n "$line" ]; do
		newLine=$(getNewLine "$line" $nextDepth)
		echo $newLine >> ./perftsuite.epd.updated
	done < ./perftsuite.epd

	echo "Done. New depth file is located at perftsuite.epd.updated."

	exit 0
}

getFen() {
	echo "${1%%;*}"
}

getCount() {
	count=$(echo "$1" | grep -oP "(?<=D$2 )[0-9]+")
	echo $count
}

# Run the test cases on the specified depth.
runTests() {
	depth=$(getDepth "$@")

	# Perform checks
	totalTests=0
	passedTests=0
	while IFS= read -r line; do
		FEN=$(getFen "$line")
		actualVal=$(getCount "$line" $depth)
		val=$(sicario "$FEN" $depth)
		if [[ $val -eq $actualVal ]]; then
			printf "[  ${GREEN}OK${NC}  ] ${FEN}\n"
			((passedTests++))
		else
			printf "[ ${RED}FAIL${NC} ] ${FEN}\n"
		fi
		((totalTests++))
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

# Run the test cases on the maximum depth available.
runMaxTests() {
	echo
}

getNextDepth() {
	nextDepth=$(echo $1 | grep -oP "(?<=D)[0-9](?=([0-9]+|[ \t]|;)+$)")
	if [ -z "$nextDepth" ]; then
		nextDepth=0
	fi
	((nextDepth++))
	echo $nextDepth
}

getNewLine() {
	newLine=$line
	fen=$(getFen "$line")
	nextDepth=$(getNextDepth "$line")

	while :; do
		printf "\rComputing depth $nextDepth for $fen" 1>&2
		timeout 30s bash -c "echo -e 'position fen $fen\ngo perft $nextDepth\nquit' | ./stockfish > res.tmp"
		if [ $? -eq 124 ]; then
			printf "\n" 1>&2
			break
		fi
		count=$(grep -oP "(?<=Nodes searched: )[0-9]+" res.tmp)
		newLine+=";D${nextDepth} ${count}"
		((nextDepth++))
	done

	echo "$newLine"
}

main "$@"