# <strong>Sicario</strong>

Sicario is a (soon-to-be) UCI compatible chess engine. It was created as a project to learn C++ and other programming
tools in a way I enjoy. I am currently more focused on the speed performance than the playing strength of the engine.
There is a lot more work to be done.

## Compiling

The program can be compiled with the included <code>Makefile</code>. I compile it using the GNU C++ compiler. I am yet
to try on other compilers but that shall come in the future. It should be executable on any 64 bit CPU (I am unsure
about 32 bit CPUs).

To compile it using the GNU C++ compiler, run the following:

```
make [OPTIMISE=1|2|3] [NDEBUG=TRUE|FALSE] [PEXT=TRUE|FALSE]
```

If any of the available options (<code>OPTIMISE</code>, <code>NDEBUG</code>, <code>PEXT</code>) are not provided, it is
assumed to take on the value <code>FALSE</code>. The meaning of the available options are as follows:

<ul>
	<li>
		<code>OPTIMISE</code> - If provided, must be one of <code>1</code>, <code>2</code> or <code>3</code>,
		corresponding to <code>-O1</code>, <code>-O2</code> and <code>-O3</code> respectively. If none is provided, the
		optimise flag is not used.</br>
	</li>
	<li>
		<code>NDEBUG</code> - If <code>TRUE</code> then all asserts and debugging related code is not compiled. If
		<code>FALSE</code> they are compiled. </br>
	</li>
	<li>
		<code>PEXT</code> - If <code>TRUE</code> then the program will use the intrinsic <code>PEXT</code> (parallel
		bit extract) intrinsic function when compiling. If <code>FALSE</code> it will use an alternative (more
		conventional) implementation. Note that this should only be <code>TRUE</code> if the CPU that the program is to
		run on supports the <code>PEXT</code> instruction.</br>
	</li>
</ul>

The output executable should be called <code>sicario</code>.

## Running

You can run the program like any executable (<code>./src/sicario</code>). You can interact with the engine as per the
UCI protocol (keep in mind that not everything is implemented yet). I have included some custom commands that are useful
either for development/debugging purposes or just for curiosity. They are as follows:

<ul>
	<li>
		<code>perft depth</code> - Performs perft to the given <code>depth</code>.
	</li>
	<li>
		<code>move alg_move</code> - Makes <code>alg_move</code> in the current position. The argument
		<code>alg_move</code> is given in algebraic notation.
	</li>
	<li>
		<code>undo</code> - Undoes the last move played.
	</li>
	<li>
		<code>display</code> - Displays an UNICODE representation of the current game state.
	</li>
	<li>
		<code>moves</code> - Displays the current moves available in the position in algebraic notation.
	</li>
	<li>
		<code>bitboards</code> - Displays the bitboards of the current position (useful for debugging).
	</li>
	<li>
		<code>random</code> - Plays out completely random games and shows the end results.
	</li>
	<li>
		<code>state</code> - Prints the state of the current position. (i.e. Normal ply (game is still ongoing), type of
		checkmate, or type of draw).
	</li>
	<li>
		<code>options</code> - Displays the engine options and their current values.
	</li>
</ul>

## Testing

In the <code>tests</code> directory are the resources that are needed to run the tests. There are a few directories and
files here.

### Directory: eog
The <code>eog</code> directory contains a script (<code>eogTest.sh</code>) and a directory for each of the possible end
of game conditions. To run:

```
Usage: ./eogTest.sh test [-h] [-c] [-k]
    test: The test to run. One of the directory names.
    -h: Display usage message.
    -c: Clean the keep files.
    -k: Keep files from tests.
```

Running this will run the tests located in the <code>inputs.txt</code> files of each directory and compares it with the
expected output. Additional tests can be added by appending a new game to the corresponding <code>games.pgn</code> file
and running the <code>convertPgnToSmith.py</code> script.

### Directory: perft
There are 2 shell scripts in this directory. They are <code>perftSuite.sh</code> and <code>perftDiff.sh</code>. The
former is for running perft on Sicario for a bunch of different test cases and checking that it counts the correct
number of leaf nodes. The latter, is for when there is a bug in the engine and you want to find out where it differs
from another engine.

The script <code>perftSuite.sh</code> is for testing move generation. It should be run whenever changes are made. If all
tests are passing, then you can be confident that there are no obvious mistakes (needless to say, not a guarantee). It
is used as follows:

```
Usage: ./perftsuite depth
    depth: The depth to perform the perft to. Any integer from 1 to 6.
```

The maximum depth is 6 because it takes an inordinate amount of time to compute at greater depths. If you wish to add a
new position to the suite, add it to the <code>perftsuite.epd</code> file in the same format as the other entries.

The script <code>perftDiff.sh</code> is useful when there is an error in move generation. Running this will show a
breakdown of where Sicario differs to Stockfish in the perft result. It is used as follows:

```
Usage: ./perftDiff.sh depth fen
    depth: The depth to perform the perft to.
    fen: FEN string of the position to run perft on. Enclose with quotes (", ')
```

Currently, it assumes that there is an existing Stockfish binary called <code>stockfish</code> in the same
directory. You will have to include this binary youself first.

### Directory: uci
This directory is for testing the uci and custom commands. Also used for testing more complex combinations of commands.
To run:

```
Usage: ./uciSuite.sh [-h]
    -h: Display usage message.
    -c: Clean the keep files.
    -k: Keep files from tests.
```

This will run through all the tests available. To add a new test, follow the same pattern and add an input and output
file. The python script <code>compare.py</code> is used to compare the test cases where the output is variable. Variable
output tests are handled using regex expressions.

### convertPgnToSmith.py
The file <code>convertPgnToSmith.py</code> converts the game(s) in a given PGN file to smith notation in a specified
output file. To run:

```
python convertPgnToSmith.py path/to/PGN path/to/output
```

Each line in the output file will contain the moves of a game in smith notation.

The file <code>pos.txt</code> contains some different forced mate positions or positions where there is/are only a
number of moves that are not losing. These can be used in the future for AI testing.

The <code>test.cpp</code> file was used only for initial stages of development (hence all the hardcoding). It is not
needed anymore but I keep it around on the off chance it becomes useful. Should you wish to compile it, simply run the
<code>make test</code> (assuming you are using GNU). This should create an executable called <code>test</code> in the
<code>tests</code> directory. Running this executable should result in 570 passing tests. If any are failing then you
have probably made an unintended change to a file somewhere.

## More Information

There are 2 other markdown files in this repo.

<ul>
	<li><code>AI.md</code> - Contains information on how the AI works.</li>
	<li><code>MOVEGEN.md</code> - Contains information on how move generation works.</li>
</ul>

## Acknowledgements

<strong>www.chessprogramming.org</strong> - Primary source of information and knowledge.

<strong>Stockfish</strong> - Main source of inspiration.

<strong>Bluefeversoft</strong> - YouTube tutorials and explanations of some concepts.

Whoever/whatever else I missed/can't recall.