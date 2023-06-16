#!/bin/bash

GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RED="\033[0;31m"
NC="\033[0m"

OK="[  ${GREEN}OK${NC}  ]"
FAIL="[ ${RED}FAIL${NC} ]"
ERROR="[ ${YELLOW}ERRO${NC} ]"

main() {
	helpCommand "$@"
	cleanKeep "$@"

	# Test UCI commands
	testUci
	testDebug
	testIsready
	testSetoption
	testUcinewgame
	testPosition
	testGo
	testStop
	testPonderhit
	testQuit

	# Test custom UCI commands
	testPerft
	testMove
	testUndo
	testDisplay
	testMoves
	testBitboards
	testRandomgame
	testState
	testOptions
	testData
	testInvalid_command

	# Test multiple commands
	testMultiple

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

# Clean the keep files
cleanKeep() {
	if [[ "$*" == *"-k"* ]]
	then
		rm output/*.tmp.keep
		exit 0
	fi
}

testUci() {
	printf "Testing UCI...\t\t\t"
	../src/sicario > output/UCI.tmp < input/UCI.txt

	# Show result
	if cmp -s output/UCI.tmp output/UCI.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/UCI.tmp output/UCI.tmp.keep
	fi
}

testDebug() {
	printf "Testing DEBUG...\t\t"
	../src/sicario > output/DEBUG.tmp < input/DEBUG.txt

	# Show result
	if cmp -s output/DEBUG.tmp output/DEBUG.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/DEBUG.tmp output/DEBUG.tmp.keep
	fi
}

testIsready() {
	printf "Testing ISREADY...\t\t"
	../src/sicario > output/ISREADY.tmp < input/ISREADY.txt

	# Show result
	if cmp -s output/ISREADY.tmp output/ISREADY.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/ISREADY.tmp output/ISREADY.tmp.keep
	fi
}

testSetoption() {
	printf "Testing SETOPTION...\t\t"
	../src/sicario > output/SETOPTION.tmp < input/SETOPTION.txt

	# Show result
	if cmp -s output/SETOPTION.tmp output/SETOPTION.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/SETOPTION.tmp output/SETOPTION.tmp.keep
	fi
}

testUcinewgame() {
	printf "Testing UCINEWGAME...\t\t$ERROR\n"
}

testPosition() {
	printf "Testing POSITION...\t\t"
	../src/sicario > output/POSITION.tmp < input/POSITION.txt

	# Show result
	if cmp -s output/POSITION.tmp output/POSITION.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/POSITION.tmp output/POSITION.tmp.keep
	fi
}

testGo() {
	printf "Testing GO...\t\t\t$ERROR\n"
}

testStop() {
	printf "Testing STOP...\t\t\t$ERROR\n"
}

testPonderhit() {
	printf "Testing PONDERHIT...\t\t$ERROR\n"
}

testQuit() {
	printf "Testing QUIT...\t\t\t$ERROR\n"
}

testPerft() {
	printf "Testing PERFT...\t\t"
	../src/sicario > output/PERFT.tmp < input/PERFT.txt

	# Show result
	if cmp -s output/PERFT.tmp output/PERFT.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/PERFT.tmp output/PERFT.tmp.keep
	fi
}

testMove() {
	printf "Testing MOVE...\t\t\t"
	../src/sicario > output/MOVE.tmp < input/MOVE.txt

	# Show result
	if cmp -s output/MOVE.tmp output/MOVE.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/MOVE.tmp output/MOVE.tmp.keep
	fi
}

testUndo() {
	printf "Testing UNDO...\t\t\t"
	../src/sicario > output/UNDO.tmp < input/UNDO.txt

	# Show result
	if cmp -s output/UNDO.tmp output/UNDO.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/UNDO.tmp output/UNDO.tmp.keep
	fi
}

testDisplay() {
	printf "Testing DISPLAY...\t\t"
	../src/sicario > output/DISPLAY.tmp < input/DISPLAY.txt

	# Show result
	if cmp -s output/DISPLAY.tmp output/DISPLAY.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/DISPLAY.tmp output/DISPLAY.tmp.keep
	fi
}

testMoves() {
	printf "Testing MOVES...\t\t"
	../src/sicario > output/MOVES.tmp < input/MOVES.txt

	# Show result
	if cmp -s output/MOVES.tmp output/MOVES.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/MOVES.tmp output/MOVES.tmp.keep
	fi
}

testBitboards() {
	printf "Testing BITBOARDS...\t\t"
	../src/sicario > output/BITBOARDS.tmp < input/BITBOARDS.txt

	# Show result
	if cmp -s output/BITBOARDS.tmp output/BITBOARDS.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/BITBOARDS.tmp output/BITBOARDS.tmp.keep
	fi
}

testRandomgame() {
	printf "Testing RANDOMGAME...\t\t"
	../src/sicario > output/RANDOMGAME.tmp < input/RANDOMGAME.txt

	# Show result
	if cmp -s output/RANDOMGAME.tmp output/RANDOMGAME.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/RANDOMGAME.tmp output/RANDOMGAME.tmp.keep
	fi
}

testState() {
	printf "Testing STATE...\t\t"
	../src/sicario > output/STATE.tmp < input/STATE.txt

	# Show result
	if cmp -s output/STATE.tmp output/STATE.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/STATE.tmp output/STATE.tmp.keep
	fi
}

testOptions() {
	printf "Testing OPTIONS...\t\t"
	../src/sicario > output/OPTIONS.tmp < input/OPTIONS.txt

	# Show result
	if cmp -s output/OPTIONS.tmp output/OPTIONS.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/OPTIONS.tmp output/OPTIONS.tmp.keep
	fi
}

testData() {
	printf "Testing DATA...\t\t\t"
	../src/sicario > output/DATA.tmp < input/DATA.txt

	# Show result
	if cmp -s output/DATA.tmp output/DATA.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/DATA.tmp output/DATA.tmp.keep
	fi
}

testInvalid_command() {
	printf "Testing INVALID_COMMAND...\t"
	../src/sicario > output/INVALID_COMMAND.tmp < input/INVALID_COMMAND.txt

	# Show result
	if cmp -s output/INVALID_COMMAND.tmp output/INVALID_COMMAND.txt;
	then
		printf "$OK\n"
	else
		printf "$FAIL\n"
		mv output/INVALID_COMMAND.tmp output/INVALID_COMMAND.tmp.keep
	fi
}

testMultiple() {
	printf "Testing multiple...\t\t$ERROR\n"
}

clean() {
	rm output/*.tmp
}

main "$@"