if [ "$#" -ne  2 ] && [ "$#" -ne 1 ]; then
	echo "Need at least a perft value..."
	echo "Run: ./run.sh perft_value or ./run.sh fen_string perft_value"
elif [ "$#" -eq 1  ]; then
	cd ../../Stockfish/src
	./stockfish > sfresult1.txt <<-EOF
	go perft $1
	EOF
	mv sfresult1.txt ../../Chess/tests
	cd ../../Chess/src
	make && ./tardigrade > result1.txt h h -perft $1 <<-EOF
	perft
	EOF
	mv result1.txt ../tests/
	cd ../tests
	sort result1.txt -r > result2.txt
	rm result1.txt
	sort sfresult1.txt -r > sfresult2.txt
	rm sfresult1.txt
	head -n -3 result2.txt > result.txt
	rm result2.txt
	head -n -4 sfresult2.txt > sfresult.txt
	rm sfresult2.txt
	vimdiff result.txt sfresult.txt
else
	cd ../../Stockfish/src
	./stockfish > sfresult1.txt <<-EOF
	position fen $1
	go perft $2
	EOF
	mv sfresult1.txt ../../Chess/tests
	cd ../../Chess/src
	make && ./tardigrade > result1.txt h h -f "$1" -perft $2 M <<-EOF
	perft
	EOF
	mv result1.txt ../tests/
	cd ../tests
	sort result1.txt -r > result2.txt
	rm result1.txt
	sort sfresult1.txt -r > sfresult2.txt
	rm sfresult1.txt
	head -n -3 result2.txt > result.txt
	rm result2.txt
	head -n -4 sfresult2.txt > sfresult.txt
	rm sfresult2.txt
	vimdiff result.txt sfresult.txt
fi
