
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
	rm /home/solomon/Sicario/tests/sc_result_vd /home/solomon/Sicario/tests/sf_result_vd
}

record_result() {
    echo "Recording result " "$1"
}

record_file() {
    while IFS= read -r line; do
        echo "$line"
    done < $1
}

perft() {
	# Default value to starting fen string
	local FEN=${2:-"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"}
	echo "Testing perft $1 on " $FEN

	# Run stockfish perft
	/home/solomon/Stockfish/src/stockfish > /home/solomon/Sicario/tests/sf_result <<-EOF
		position fen $FEN
		go perft $1
	EOF
	tail -n +2 /home/solomon/Sicario/tests/sf_result | sort > /home/solomon/Sicario/tests/sf_result_vd
	rm /home/solomon/Sicario/tests/sf_result

	# Run sicario perft
	make -C /home/solomon/Sicario/src/ -s
	/home/solomon/Sicario/src/sicario > /home/solomon/Sicario/tests/sc_result <<-EOF
		set fen $FEN
		perft $1
		exit
	EOF
	tail -n +21 /home/solomon/Sicario/tests/sc_result | sort > /home/solomon/Sicario/tests/sc_result_vd
	rm /home/solomon/Sicario/tests/sc_result

	# Vimdiff the results
	vimdiff /home/solomon/Sicario/tests/sc_result_vd /home/solomon/Sicario/tests/sf_result_vd
}

main "$@"
