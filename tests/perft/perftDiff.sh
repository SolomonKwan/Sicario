#!/bin/bash

GREEN="\033[0;32m"
RED='\033[0;31m'
NC='\033[0m'

main() {
	# Parse inputs
	helpCommand "$@"
	parseInputs "$@"
	local FEN=$(getFen "$@")

	# Default value to starting fen string
	echo "Testing perft $1 on "$FEN""
	sicario $1 "$FEN"
	stockfish $1 "$FEN"

	# Vimdiff the results
	vimdiff sicario.tmp stockfish.tmp
}

# Prints the usage message.
showUsageMessage() {
	echo "Usage: ./perftDiff.sh depth fen [-h]"
	echo "    depth: The depth to perform the perft to."
	echo "    fen: FEN string of the position to run perft on. Enclose with quotes (\", ')."
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
	if [ "$#" -lt 1 ] || [ "$#" -gt 2 ] || [[ ! $1 =~ ^[0-9]+$ ]] || [ $1 -lt 1 ]; then
		echo "Invalid arguments"
		showUsageMessage
		exit -1
	fi
}

# Get the position FEN string.
getFen() {
	# Return the FEN string to test.
	res="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	if [ "$#" -eq 2 ]; then
		res="$2"
	fi
	echo $res
}

# Runs the perft command on sicario and records and processes the result for later vimdiff.
sicario() {
	../../src/sicario > ./sicario.tmp <<-EOF
		position fen $2
		perft $1
		quit
	EOF

	# Remove unnecessary lines and sort result
	sed -i '1,12d' sicario.tmp
	sort -o sicario.tmp sicario.tmp
	sed -i '/^$/d' sicario.tmp
}

# Runs the perft command on stockfish and records and processes the result for later vimdiff.
stockfish() {
	./stockfish > ./stockfish.tmp <<-EOF
		position fen $2
		go perft $1
		quit
	EOF

	# Remove unnecessary lines and sort result
	sed -i '1,1d' stockfish.tmp
	sort -o stockfish.tmp stockfish.tmp
	sed -i '/^$/d' stockfish.tmp
}

main "$@"
