head -n -3 en_passant_tests > ep_tests.txt
rm failed_ep.txt
touch failed_ep.txt
make
while read p; do
    echo $p
	cd ../../Stockfish/src
	./stockfish > ep_sfresult1.txt <<-EOF1
	position fen "$p"
	go perft $1
	EOF1
	mv ep_sfresult1.txt ../../Chess/tests
	cd ../../Chess/src
	./tardigrade > ep_result1.txt h h -f "$p" -perft $1 <<-EOF
	perft
	EOF
	mv ep_result1.txt ../tests/
	cd ../tests
	sort ep_result1.txt -r > ep_result2.txt
	rm ep_result1.txt
	sort ep_sfresult1.txt -r > ep_sfresult2.txt
	rm ep_sfresult1.txt
	head -n -3 ep_result2.txt > ep_result.txt
	rm ep_result2.txt
	head -n -4 ep_sfresult2.txt > ep_sfresult.txt
	rm ep_sfresult2.txt
	diff ep_result.txt ep_sfresult.txt > ep_diff.txt
	if [ -s ep_diff.txt ]; then
		echo "$p" >> failed_ep.txt
	fi
done < ep_tests.txt
rm ep_result.txt ep_sfresult.txt ep_diff.txt ep_tests.txt
