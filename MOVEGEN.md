# Overview
Just a file for jotting down my thoughts about the move generation. When generating moves, the 2 main rules of the game we must keep in mind is that we must not:
<ul>
    <li>Move our king into check (including capturing defended pieces); and</li>
    <li>Move a piece in such a way that exposes our king to check</li>
</ul>
As a result, we must be mindful of checking <strong>pins</strong>.
</br>
Keep in mind that when a piece is pinned, it can still move along the line in which it is pinned.
</br>
</br>
The move generation can be split into 3 main cases depending on the state of the board.
<ul>
    <li>Moves when in double in check</li>
    <li>Moves when in check</li>
    <li>Moves when not in check</li>
</ul>
</br>

# Double Check Moves
King must move. Things to keep in mind when doing this:
<ul>
    <li>Capturing own pieces</li>
    <li>Move king into check</li>
    <li>Capturing defended piece</li>
</ul>
We shall use magic bitboards for king move generation. Let us assume that we have a precomputed array of the reach of a king on every square. Assuming we have a bitboard of potential destination squares for the king, we multiply this by a magic number and then shift it to get a unique index into a precomputed array of moves.
</br>
</br>
We can easily exclude capturing own pieces using bit manipulation to mask out our own pieces from the reach.
</br>
</br>
To check if we are moving the king into check, we will need check if the destination square is attacked. Will need to retrieve the rook reach mask and bishop reach mask to check respective attacks (including queen for both). Check if square is attacked by knight. Check if attacked by pawn.
</br>
</br>
<strong>king cannot move along the line of check</strong>.
</br>
</br>
The same checks as the one immediately above cover this case too.
</br>
</br>

## <strong>King move generation will be pseudolegal. Need to check if moving to defended square.</strong>


</br>
</br>

# Check Moves
Either we move the king, block the check or capture the checking piece.
</br>
</br>
If we are moving the king out of check, we can follow the same as the above pseudolegal move generation. However, we have to remember that the <strong>king cannot move along the line of check</strong>.
</br>
</br>
To block. Can create a check ray, including the checker. Find direction of checker, and mask out the other rays. Iterate over own pieces and check if the reach intersects with the check ray and checker bitboard. If it intersects then we can index into a precomputed array of moves. Will need magic bitboard for pawns and knights. Will need special magic nums for rook and bishops for blocks specifically.
</br>
</br>

# Non-Check Moves
Same checks for king as the above.
</br>
</br>
Pinned pieces can move and capture along the line of being pinned.
</br>
</br>

# King Moves
Precompute king reach bitboard for each square
</br>
Precompute magic numbers for king on each square.
</br>
Precompute index for king on each square.
</br>
Precompute set of possible permutation of king on each square from the bitboard and store moves for it.
</br>
<ol>
    <li>To get moves for King, get reach bitboard by square.</li>
    <li>For each square in moore neigh, if not own piece check if attacked</li>
    <li>If not attacked add it to a bitboard</li>
    <li>Multiply bitboard by king magic number and shift to get unique index</li>
    <li>Use index to get precomputed moves</li>
</ol>
The moves will be valid except possibly moving the king into check. Need to check every move that destination square is not attacked.
</br>
</br>

# Rook Moves
<ol>
    <li>Mask rook file and rank (remove the rook itself)</li>
    <li>Multiply by magic num and bit shift to get unique index</li>
    <li>Use unique index to find the reach</li>
    <li>Then we can use the reach and mask out our own pieces</li>
    <li>Then multiply by a different magic number and shift to get index into precomputed moves</li>
</ol>
</br>

# Bishop Moves
Similar to rook moves
</br>
</br>

For pinned moves, we can also do a similar thing. Once we know a piece is pinned, we can build a custom bitboard that maskes out the rays in which we don't care.
Using this new bitboard, we mask out our own pieces too and just search for the moves as per usual.