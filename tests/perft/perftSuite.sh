#!/bin/bash

GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RED="\033[0;31m"
NC="\033[0m"
ERASE="\033[1K\r"

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
	if  [ ! -z $1 ] && ([[ ! $1 =~ ^[0-9]+$ ]] || [ $1 -lt 1 ]); then
		echo "Invalid input"
		showUsageMessage
		exit -1
	fi
}

# Runs the perft command on sicario and returns the result.
sicario() {
	../../src/sicario > ./res.tmp <<-EOF
		position fen $1
		perft $2
		quit
	EOF
	count=$(grep -oP "(?<=Nodes searched: )[0-9]+" res.tmp)
	echo $count
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

# Extract the FEN string from the given file line.
getFen() {
	echo "${1%%;*}"
}

# Get count for the specified depth.
getCount() {
	count=$(echo "$1" | grep -oP "(?<=D$2 )[0-9]+")
	echo $count
}

# Run the test cases on the specified depth.
runTests() {
	if [ -z $1 ]; then
		return
	fi
	depth=$1

	# Perform checks
	totalTests=0
	passedTests=0
	while IFS= read -r line; do
		FEN=$(getFen "$line")
		actualVal=$(getCount "$line" $depth)

		# Check if the depth is recorded.
		if [ -z $actualVal ]; then
			printf "[ ${YELLOW}ERRO${NC} ] Depth $depth not recorded for $FEN\n"
			continue
		fi

		# Test on Sicario and compare the output
		printf "${ERASE}Testing depth $depth on \"$FEN\"..."
		val=$(sicario "$FEN" $depth)
		if [[ $val -eq $actualVal ]]; then
			((passedTests++))
		else
			printf "${ERASE}[ ${RED}FAIL${NC} ] ${FEN}\n"
		fi

		((totalTests++))
	done < ./perftsuite.epd

	# Show end result
	if [[ $passedTests -eq $totalTests ]]; then
		printf "${ERASE}Perft suite testing OK\n"
	else
		printf "${ERASE}${GREEN}OK${NC}:\t$((passedTests))\n"
		printf "${RED}FAIL${NC}:\t$((totalTests - passedTests))\n"
	fi
	rm res.tmp

	exit 0
}

# Run the test cases on the maximum depth available.
runMaxTests() {

	# Perform checks
	totalTests=0
	passedTests=0
	while IFS= read -r line; do
		FEN=$(getFen "$line")
		depth=$(($(getNextDepth "$line") - 1))
		actualVal=$(getCount "$line" $depth)

		# Test on Sicario and compare the output
		printf "${ERASE}Testing depth $depth on \"$FEN\"..."
		val=$(sicario "$FEN" $depth)
		if [[ $val -eq $actualVal ]]; then
			((passedTests++))
		else
			printf "${ERASE}[ ${RED}FAIL${NC} ] ${FEN}\n"
		fi

		((totalTests++))
	done < ./perftsuite.epd

	# Show end result
	if [[ $passedTests -eq $totalTests ]]; then
		printf "${ERASE}Perft max suite testing OK\n"
	else
		printf "${ERASE}${GREEN}OK${NC}:\t$((passedTests))\n"
		printf "${RED}FAIL${NC}:\t$((totalTests - passedTests))\n"
	fi
	rm res.tmp

	exit 0
}

# Get the next depth of the test case.
getNextDepth() {
	nextDepth=$(echo $1 | grep -oP "(?<=D)[0-9](?=([0-9]+|[ \t]|;)+$)")
	if [ -z "$nextDepth" ]; then
		nextDepth=0
	fi
	((nextDepth++))
	echo $nextDepth
}

# Get the new line with the updated depth values for the fen.
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