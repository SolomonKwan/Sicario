
cd ../src
make
cd ../tests

while IFS= read -r line
do
    IFS=';'
    read -ra ADDR <<< "$line"
    cd ../src
    ./tardigrade > perftPosResult.txt h h -perft 6 <<-EOF
    perft
	EOF
	mv perftPosResult.txt ../tests/
	break
	# cat perftPosResult.txt
	# cd ../tests







    # echo ${ADDR[0]}${ADDR[6]:3}
done < perftsuite.epd
# cd ../src
# rm perftPosResult.txt