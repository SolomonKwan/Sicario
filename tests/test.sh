#!/bin/bash

GREEN="\033[0;32m"
RED='\033[0;31m'
NC='\033[0m'

main() {
	case $1 in
		perft)
			perft "$2" "$3"
			;;
		perft_suite)
			perft_suite
			;;
		# record_result)
		# 	record_result "$2"
		# 	;;
		# record_file)
		# 	record_file "$2"
		# 	;;
		check)
			check
			;;
		# clean)
		# 	clean
		# 	;;
		*)
			echo "invalid command"
			;;
	esac
}

perft() {
	# Default value to starting fen string
	local FEN=${2:-"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"}
	echo "Testing perft $1 on " $FEN

	# Run stockfish perft
	../../stockfish_15_linux_x64_bmi2/stockfish_15_x64_bmi2 > ./sf_result.txt <<-EOF
		position fen $FEN
		go perft $1
		quit
	EOF
	sort sf_result.txt | tail -n +3 | head -n -1 > sf_result_sorted.txt
	cat sf_result_sorted.txt > sf_result.txt
	rm sf_result_sorted.txt

	# Run sicario perft
	../src/sicario > ./sc_result.txt <<-EOF
		position fen $FEN
		perft $1
		quit
	EOF
	tail sc_result.txt -n +13 | sort > sc_result_sorted.txt
	cat sc_result_sorted.txt > sc_result.txt
	rm sc_result_sorted.txt

	# Vimdiff the results
	vimdiff sf_result.txt sc_result.txt
}

sicario_value() {
	../src/sicario > ./sc_result.txt <<-EOF
		position fen $1
		perft $2
		quit
	EOF
	tail -n 1 sc_result.txt > sc_result.tmp
	cat sc_result.tmp > sc_result.txt
	cat sc_result.txt
	rm sc_result.tmp
}

perft_suite() {
	printf "Running perft suite\n"
	while IFS= read -r line; do
		local FEN="${line%%;*}"
		local actualVal=$(echo $line | grep -o "D6 .*" | cut -c 4-)
		local val=$(sicario_value "$FEN" 6 | cut -c 17-)
		if [[ $val -eq $actualVal ]]
		then
			printf "[ ${GREEN}PASSED${NC} ] ${FEN}\n"
		else
			printf "[ ${RED}FAILED${NC} ] ${FEN}\n"
		fi
	done < ./perftsuite.epd
	rm sc_result.txt sf_result.txt
}

# clean() {
# 	echo "Removing files:"
# 	echo "	sc_result_vd"
# 	echo "	sc_result"
# 	echo "	sf_result_vd"
# 	echo "	sf_result"
# 	rm ~/Sicario/tests/sc_result_vd ~/Sicario/tests/sf_result_vd ~/Sicario/tests/sf_result ~/Sicario/tests/sc_result
# }

stockfish_value() {
	../../stockfish_15_linux_x64_bmi2/stockfish_15_x64_bmi2 > ./sf_result.txt <<-EOF
		position fen "$1"
		go perft $2
		quit
	EOF
	tail -n 2 sf_result.txt | head -n 1 > sf_result.tmp
	cat sf_result.tmp > sf_result.txt
	cat sf_result.txt
	rm sf_result.tmp
}

check() {
	printf "Performing quick check...\n"

	local original="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
	local sc_value=$(sicario_value "$original" 6)
	local sf_value=$(stockfish_value "$original" 6)
	if [[ $sc_value == $sf_value ]]
	then
		printf "[ ${GREEN}PASSED${NC} ] Original position\n"
	else
		printf "[ ${RED}FAILED${NC} ] Original position\n"
	fi

	local kiwipete="r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
	local sc_value=$(sicario_value "$kiwipete" 5)
	local sf_value=$(stockfish_value "$kiwipete" 5)
	if [[ $sc_value == $sf_value ]]
	then
		printf "[ ${GREEN}PASSED${NC} ] Kiwipete position\n"
	else
		printf "[ ${RED}FAILED${NC} ] Kiwipete position\n"
	fi
}

# record_result() {
# 	echo "Recording result" "$1"
# 	local entry=$1
# 	for i in {1..6}; do
# 		local val=$(stockfish_value "$1" "$i")
# 		entry=$entry' ;D'$i' '$val
# 	done
# 	echo "$entry" >> perftsuite.epd
# }

# record_file() {
# 	while IFS= read -r line; do
# 		record_result "$line"
# 	done < $1
# 	rm ~/Sicario/tests/sf_result
# }

main "$@"
