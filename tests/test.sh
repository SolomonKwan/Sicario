
main() {
	case $1 in
		perft)
			perft "$2" "$3"
			;;
		perft_suite)
			echo "perft_suite"
			;;
		ep_suite)
			echo "ep_suite"
			;;
		record_result)
			record_result "$2"
			;;
        record_file)
            record_file "$2"
            ;;
		clean)
			clean
			;;
		*)
			echo "invalid command"
			;;
	esac
}

clean() {
	echo "Removing files:"
	echo "    sc_result_vd"
	echo "    sf_result_vd"
	rm ~/Sicario/tests/sc_result_vd ~/Sicario/tests/sf_result_vd
}

record_result() {
    echo "Recording result" "$1"
	local entry=$line
	for i in {1..6}; do
		local val=$(stockfish_value "$1" $i)
		entry=$entry' ;D'$i' '$val
	done
	echo "$entry" >> perftsuite.epd
}

stockfish_value() {
    ~/Stockfish/src/stockfish > ~/Sicario/tests/sf_result <<-EOF
		position fen "$1"
		go perft $2
	EOF
	echo $(cut -d ":" -f2- <<< $(tail -n 2 ~/Sicario/tests/sf_result))
}

record_file() {
    while IFS= read -r line; do
		record_result "$line"
    done < $1
	rm ~/Sicario/tests/sf_result
}

perft() {
	# Default value to starting fen string
	local FEN=${2:-"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"}
	echo "Testing perft $1 on " $FEN

	# Run stockfish perft
	~/Stockfish/src/stockfish > ~/Sicario/tests/sf_result <<-EOF
		position fen $FEN
		go perft $1
	EOF
	tail -n +2 ~/Sicario/tests/sf_result | sort > ~/Sicario/tests/sf_result_vd
	rm ~/Sicario/tests/sf_result

	# Run sicario perft
	make -C ~/Sicario/src/ -s
	~/Sicario/src/sicario > ~/Sicario/tests/sc_result <<-EOF
		set fen $FEN
		perft $1
		exit
	EOF
	tail -n +21 ~/Sicario/tests/sc_result | sort > ~/Sicario/tests/sc_result_vd
	rm ~/Sicario/tests/sc_result

	# Vimdiff the results
	vimdiff ~/Sicario/tests/sc_result_vd ~/Sicario/tests/sf_result_vd
}

main "$@"
