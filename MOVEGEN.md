# Move Generation Explanations
This contains an explanation of how the move generation works. Move generation is based on the bitboard
representation of the current position as well as other data structures. The main concept underpinning move generation
is the use of magic bitboards. Purely for the sake of not being just an ordinary magic bitboard implementation, I used a
slightly tweaked approach that involves precomputing indices into precomputed move arrays. This allows us to get the
moves of a piece in the position fairly quickly while also avoiding the problem of calculating and storing redundant
move sets. Current investigation indicates that this approach gives several orders of magnitude advantage when it comes
to memory consumption for the move generation portion only. Legal move generation is also a happy convenience.

## Explanation
To explain the approach used I will first explain a few basic concepts. The first shall be understanding the magic
bitboard approach. I learnt the concept here https://www.chessprogramming.org/Magic_Bitboards, but I shall explain.

### Bitboards

I use unsigned 64 bit integers to represent various board states. Specifically, I have such integers for the white
pieces, black pieces, kings, queens, rooks, bishops, knights and pawns. First, note that the kings, queens, rooks,
bishops, knights and pawns bitboards do not distinguish between sides. That information is captured in the white and
black pieces bitboards.

I have followed the convention that the least significant bit corresponds to the square a1 followed by b1, then c1 and
so on until h1. After this point, it will begin a2, b2,... and follow the same pattern until the final square h8.

Using this convention, if there is a white king on e1 and no other pieces on the board, then the kings and white
bitboards will both look as follows in binary:

<code>0xb0000000000000000000000000000000000000000000000000000000000010000</code>

This can be represented in the more intuitive format of:

```
00000000
00000000
00000000
00000000
00000000
00000000
00000000
00001000
```

And so in the starting position of a normal game of chess, the white bitboard and kings bitboards would look as follows
respectively:

```
00000000        00001000
00000000        00000000
00000000        00000000
00000000        00000000
00000000        00000000
00000000        00000000
11111111        00000000
11111111        00001000
```

Bitwise AND-ing these two bitboards would give us all the white kings. A similar approach is used for any piece
type/colour combination.

### Magic Bitboards

Magic bitboards are so named because they use "magic numbers" to retrieve moves for ranged pieces. These magic numbers
have to be found via guess and check but are only needed to be found once and can then be hardcoded. The ones I found
are in <code>bitboard.hpp</code>. The general approach is as follows:

<ol>
    <li>Create the occupancy bitboard: <code>whiteBB | blackBB = occ</code></li>
    <li>Mask out unneeded bits: <code>occ & mask = mOcc</code></li>
    <li>Multiply by the magic bitboard: <code>mOcc * mn = out</code></li>
    <li>Shift the result to get the unique index: <code>out >> shift = index</code></li>
</ol>

To more clearly understand how they are used (and thus a hint on how we can find them) let us see an example. Suppose we
have a random position from some game and we create an occupancy bitboard by taking our white and black bitboards and
performing a bitwise OR. The resulting bitboard will contain 0s and 1s where a 1 would indicate that a piece, either
white or black, is on that square. Say that the bitboard we got was the following:

```
10001101
01000000
00001010
00000000
01001000
00000000
01100100
10001001
```

Say we have a rook on e4 and we want its available moves. We start by bitwise AND-ing this bitboard with a precomputed
mask for rooks on e4 to get only the occupants of the rank and file that the rook is on.

```
occupancy & mask = masked occupancy

10001101     00000000     00000000
01000000     00001000     00000000
00001010     00001000     00001000
00000000 AND 00001000  =  00000000
01001000  &  01110110     01000000
00000000     00001000     00000000
01100100     00001000     00000000
10001001     00000000     00000000
```

Note that the mask does not include the edge squares. This is because we can reach the edge squares whether or not there
is a piece on them (we are currently ignoring illegal captures of own pieces). This ultimately helps to reduce the
amount of memory needed since the end result will be 4 bits shorter.

Using this masked occupancy bitboard, we can multiply it by a precomputed "magic number". This is an unsigned 64 bit
number we found through trial-and-error that gives a unique string of bits in the n most significant positions of the
result for every possible occupancy. The number n will be the number of set bits in the mask. Taking only these n most
significant bits therefore gives us a unique number for every possible masked occupancy for when a rook is on e4. This
will be our unique index into a precomputed set of moves/bit masks or whatever we want. Hopefully the following
illustrates this more clearly.

```
In the above example, the mask contains 10 set bits.

masked occupancy * magic number = output
We then shift this "output" to get the 10 most significant bits as our index.

00000000   ........   CDEFGHIJ      CDEFGHIJ
00000000   ........   ......AB      ......AB
00001000   .some...   ........      ........
00000000 * .magic.. = ........ then ........ >> (64 - 10) = ABCDEFGHIJ
01000000   .number.   ........      ........
00000000   ........   ........      ........
00000000   ........   ........      ........
00000000   ........   ........      ........
```

The bits <code>ABCDEFGHIJ</code> now form the unique index for this particular occupancy. Now, this same magic number
above would have to produce a unique index for every possible occupancy when a rook is on e4. This is the key part of
this perfect hashing algorithm. An analgous approach is done for bishops, and a queen is basically just a rook and
bishop combined.

I believe the traditional approach from here is to use the resulting unique index to get the moves for a piece either
as a bitboard of possible moves or actual precomputed moves. It must be noted that this would result in pseudo-legal
moves at it allows capturing a player's own pieces. Therefore the move legality must be checked beforehand. This is the
basic idea of the magic bitboard approach. It is a perfect hashing algorithm.

### Drawbacks

This approach above does have some drawbacks. As mentioned, one of them is that it is a pseudo-legal move generator.

The other problem is that it has a lot of redundancy for pieces that have ranged-based movement (queens, rooks and
bishops). This is because the first blockers (occupied bits) on each "ray" of the masked occupancies are what determines
the available moves. The occupancies after the blockers of each "ray" are redundant. For example:

```
00000000     00001000                00000000
00000000     00000000                00000000
00001000     00001000                00001000
00000000  ,  00000000  both lead to  00001000
01000000     11000000                01110111
00001000     00001000                00001000
00000000     00001000                00000000
00000000     00000000                00000000
```

Since each possible occupancy leads to a unique index which is then used to map to a distinct piece of data (move set or
move bitboard), this results in duplication of data. This is especially the case where the first blockers on each ray
are the first squares available. All remaining squares on each ray have no influence. E.g., a rook on e4 immediately
surrounded by pieces on all sides has only 1 possible set of pseudo-legal moves, namely the following:

```
00000000
00000000
00000000
00001000
00010100
00001000
00000000
00000000
```

But the number of possible occupancy masks is 2^10 = 1024. And so we have 1024 unique indices pointing to separate
pieces of memory that store a (possibly large) object which all have the same value. My tweaked approach fixes this
problem (as well as the pseudo-legal move problem) by using an subsequent mapping based on a bitboard of legal moves.

### My Tweaked Approach

My tweaked approach follows on from the steps listed above (near Magic Bitboards) and goes as follows:

<ol>
    <li>The last step completed gave us a unique index which we will call <code>index1</code>.</li>
    <li>Index into a precomputed set of indices: <code>indices[index1] = index2</code>.</li>
    <li>Retrieve a reach bitboard: <code>precomputedBB[index2] = reachBB</code>.</li>
    <li>Clear self captures with the negated bitboard of the moving side: <code>reachBB = reachBB & ~sideBB</code>.</li>
    <li>Multiply by the magic number (a different one this time): <code>reachBB * mn = out</code>.</li>
    <li>Shift to get a unique index: <code>out >> shift = index3</code>.</li>
</ol>

This final index is then used to index into a precomputed move array. The key parts of this approach are step 2 and 4.
In the vanilla approach <code>index1</code> would be unique for every possible occupancy and used to index into an array
or bitboard of pseudo-legal moves. As mentioned before, this leads to repeated values. Instead, step 2 uses
<code>index1</code> to index into a precomputed indices array that maps to a new index, <code>index2</code>, that gives
constructive collisions for occupancies that map to the same move set.

```
Old way: index1 -> move object
a1 -> obj1
a2 -> obj2
   ...
aM -> objM
aN -> objN
Here, obj1 to objM are all different memory addresses that contain the same value.

Tweaked way: index1 -> index2 -> move object
a1 -> b1 -> obj1
a2 -> b1 -> obj1
      ...
aM -> b1 -> obj1
aN -> b2 -> obj2
```

In this contrived example above, all of the first stage indices, except the last one, lead to the same theoretical move
set. The intermediary second stage of indices creates convenient collisions to the same object in memory.

Using this method, instead of duplicating move arrays/bitboards that are at least 64 bits long to potentially hundreds
long, we are instead duplicating only 16 bit indices in the second stage indices. For a single rook on a single square
this may not appear to be much, but for both rooks and bishops on all 64 squares it is considerable.

As you may notice in step 4, although not necessary, it is a convenient way of restricting the moves retrieved to legal
moves. A happy convenience.

This concludes an explanation of the basic ideas underpinning move generation using magic bitboards. Traditionally, the
magic bitboard approach is used only for ranged piece movements but I have used it for all piece move generation except
en-passant and castling. En-passant is rare enough that we can treat them on demand. Castling is also easy and quick
enough to check on demand.

## Implementation Overview

Move generation in Sicario is split into 3 cases:
<ul>
    <li>Double check moves</li>
    <li>Single check moves</li>
    <li>Non-check moves</li>
</ul>

When the king is in <strong>double check</strong>, the king <strong>must</strong> move. Things to keep in mind when
doing this:
<ul>
    <li>The king cannot capture pieces of the same colour.</li>
    <li>The king cannot capture a defended piece.</li>
    <li>The king cannot move to an attacked square.</li>
</ul>

When the king is in <strong>single check</strong> one of the following must occur:

<ul>
    <li>The check is blocked.</li>
    <li>The checking piece is captured.</li>
    <li>The king moves out of check to a square that is not attacked.</li>
</ul>

In both the single and double check situations, one must be mindful when generating moves that the king cannot move
along any of the lines of check as it would still remain in check after the move is completed.

<strong>Non-check</strong> moves are retrieved by iterating over every piece and retrieving the legal moves. The only
things to keep in mind are the following:

<ul>
    <li>The king cannot move to an attacked square (i.e. into check).</li>
    <li>The king cannot capture a defended piece (equivalent to the previous point).</li>
    <li>Pinned pieces cannot move out of the line of pin, but they can move along it.</li>
</ul>

### General Idea of the Implementation

The general idea used in Sicario for legal move generation for each piece is as follows:

<ol>
    <li>Construct a bitboard where a set bit indicates a legal move for that piece to that square.</li>
    <li>Multiply this bitboard by the appropriate magic number to get a result.</li>
    <li>Shift this result to get an index.</li>
    <li>Use this to index into a precomputed moves data structure.</li>
</ol>

The only part that differs majorly for some of the pieces is the first step where we construct a bitboard of legal
moves. As mentioned before, traditinoally the magic bitboard approach is used for range-based pieces only. I have used
it for all pieces instead. Turns out to be more straightforward (unsurprisingly) for non-range based pieces.