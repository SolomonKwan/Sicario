
main() {
	case $1 in
		perft)
			perft "$2" "$3"
			;;
		# perft_suite)
		# 	perft_suite
		# 	;;
		# record_result)
		# 	record_result "$2"
		# 	;;
		# record_file)
		# 	record_file "$2"
		# 	;;
		# check)
		# 	check
		# 	;;
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
	make -C ../ -s
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

# sicario_value() {
# 	make -C ~/Sicario/src/ -s
# 	~/Sicario/src/sicario > ~/Sicario/tests/sc_result <<-EOF
# 		nouci
# 		set fen $1
# 		perft $2
# 		exit
# 	EOF
# 	echo $(cut -d ":" -f2- <<< $(tail -n 2 ~/Sicario/tests/sc_result))
# }

# perft_suite() {
# 	echo "Running perft suite"
# 	while IFS= read -r line; do
# 		local FEN="${line%%;*}"
# 		val=$(sicario_value "$FEN" 6)
# 		actualVal=$(echo "$line", grep -o "D6 .*", cut -c 4-)
#         GREEN='\033[0;32m'
#         RED='\033[0;31m'
#         NC='\033[0m'
# 		if [[ $val -eq $actualVal ]]
# 		then
# 			echo -e "${GREEN}[ PASSED ]${NC} " "$FEN"
# 		else
# 			echo -e "${RED}[ FAILED ]${NC} " "$FEN"
# 		fi
# 	done < ~/Sicario/tests/perftsuite.epd
# 	rm sc_result
# }

# clean() {
# 	echo "Removing files:"
# 	echo "	sc_result_vd"
# 	echo "	sc_result"
# 	echo "	sf_result_vd"
# 	echo "	sf_result"
# 	rm ~/Sicario/tests/sc_result_vd ~/Sicario/tests/sf_result_vd ~/Sicario/tests/sf_result ~/Sicario/tests/sc_result
# }

# stockfish_value() {
# 	~/Stockfish/src/stockfish > ~/Sicario/tests/sf_result <<-EOF
# 		position fen "$1"
# 		go perft $2
# 	EOF
# 	echo $(cut -d ":" -f2- <<< $(tail -n 2 ~/Sicario/tests/sf_result))
# }

# # Quick check of perft 5 on the original and kiwipete position.
# check() {
# 	echo "Performing quick check..."
# 	local original="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
# 	val=$(sicario_value "$original" 5)
# 	actualVal=$(stockfish_value "$original" 5)
# 	if [[ $val -eq $actualVal ]]
# 	then
# 		echo "[ PASSED ] Original position"
# 	else
# 		echo "[ FAILED ] Original position"
# 	fi

# 	local kiwipete="r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
# 	val=$(sicario_value "$kiwipete" 5)
# 	actualVal=$(stockfish_value "$kiwipete" 5)
# 	if [[ $val -eq $actualVal ]]
# 	then
# 		echo "[ PASSED ] Kiwipete position"
# 	else
# 		echo "[ FAILED ] Kiwipete position"
# 	fi
# }

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
