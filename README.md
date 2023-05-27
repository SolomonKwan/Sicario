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
make [OPTIMISE=TRUE|FALSE] [NDEBUG=TRUE|FALSE] [PEXT=TRUE|FALSE]
```

If any of the available options (<code>OPTIMISE</code>, <code>NDEBUG</code>, <code>PEXT</code>) are not provided, it is
assumed to take on the value <code>FALSE</code>. The meaning of the available options are as follows:

<ul>
	<li>
		<code>OPTIMISE</code> - If <code>TRUE</code> then the program compiles with the <code>-O3</code> flag, else it
		does not compile with any optimisation flag.</br>
	</li>
	<li>
		<code>NDEBUG</code> - If <code>TRUE</code> then all asserts and debugging related code is not compiled, else
		they are compiled. </br>
	</li>
	<li>
		<code>PEXT</code> - If <code>TRUE</code> then the program will use the intrinsic <code>PEXT</code> (parallel
		bit extract) intrinsic function when compiling, else it will use an alternative (more conventional)
		implementation. Note that this should only be <code>TRUE</code> if the CPU that the program is to run on
		supports the <code>PEXT</code> instruction.</br>
	</li>
</ul>

The output executable should be called <code>sicario</code>.

## Running

You can run the program like any executable (<code>./src/sicario</code>). You can interact with the engine as per the
UCI protocol (keep in mind that not everything is implemented yet). I have included some custom commands that are useful
either for development/debugging purposes or just for curiosity. They are as follows:

<ul>
	<li><code>perft depth</code> - Performs perft to the given <code>depth</code>.</li>
	<li>
		<code>move alg_move</code> - Makes <code>alg_move</code> in the current position. The argument
		<code>alg_move</code> is given in algebraic notation.
	</li>
	<li><code>undo</code> - Undoes the last move played.</li>
	<li><code>display</code> - Displays an UNICODE representation of the current game state.</li>
	<li><code>moves</code> - Displays the current moves available in the position in algebraic notation.</li>
	<li><code>bitboards</code> - Displays the bitboards of the current position (useful for debugging).</li>
	<li>
		<code>random</code> - Plays out 1000000000 games to the end with completely random moves. Only prints to console
		the end result of each game (who wins or the kind of draw).
	</li>
	<li>
		<code>state</code> - Prints the state of the current position. (i.e. Normal ply (game is still ongoing), type of
		checkmate, or type of draw).
	</li>
	<li><code>options</code> - Displays the engine options and their current values.</li>
</ul>

## Testing

In the <code>tests</code> directory are the resources and files that are needed to run the tests. The
<code>test.cpp</code> file was used only for initial stages of development (hence all the hardcoding). It is not needed
anymore but I keep it around on the off chance it becomes useful. Should you wish to compile it, simply run the
<code>make test</code> (assuming you are using GNU). This should create an executable called <code>test</code> in the
<code>tests</code> directory. Running this executable should result in 570 passing tests. If any are failing then you
have probably made an unintended change to a file somewhere. In that, sense its a good sanity check test.

The shell script <code>test.sh</code> is also a sanity check testing script but more comprehensive. Some of the
functionality of this script will require Stockfish. The following is how to run:

```
./test.sh [perft depth position]|perft_suite|check
```

The first option (<code>perft depth position</code>) runs the perft command on both Sicario and Stockfish and compares
the output of both to show any discrepancies. The argument <code>depth</code> is the depth to run perft to and
<code>position</code> is the FEN representation of the position to run perft on.

The second option (<code>perft_suite</code>) runs perft on all the positions in the file <code>perftsuite.epd</code> to
a depth of 6 and compares the number of leaf nodes counted to the known value recorded in the file.

The last option (<code>check</code>) runs perft on the start and "kiwipete" position for both Sicario and Stockfish and
compares the number of leaf nodes counted.

The file <code>pos.txt</code> contains some different forced mate positions or positions where there is/are only a
number of moves that are not losing. These can be used in the future for AI testing.

## More Information

There are 2 other markdown files in this repo.

<ul>
	<li><code>AI.md</code> - Contains information on how the AI works.</li>
	<li><code>MOVEGEN.md</code> - Contains information on how move generation works.</li>
</ul>

## Known Issues

If you find an issue, a bug, or even something you think can be improved feel free to raise an issue. As this is still
an ongoing project there are likely to be issues here and there.

<ul>
	<li>
		In some terminal fonts, the name and logo does not render correctly. Might be worth trying to determine whether
		or not something will render correctly (not sure if fixable).
	</li>
	<li>
		The PV table or history position counter may have collisions due to different positions having the same hash.
		Very unlikely, but possible. May need some kind of check
	</li>
</ul>

## Acknowledgements

<strong>www.chessprogramming.org</strong> - Primary source of information and knowledge.

<strong>Stockfish</strong> - Main source of inspiration.

<strong>Bluefeversoft</strong> - YouTube tutorials and explanations of some concepts.

Whoever/whatever else I missed/can't recall.