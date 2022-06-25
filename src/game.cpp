
#include <iostream>
#include <bitset>
#include <random>

#include "game.hpp"
#include "utils.hpp"
#include "bitboard.hpp"

/**
 * Precomputed moves information.
 */
namespace Moves {
    const std::array<std::vector<std::vector<Move>>, 64> KING = computeKingMoves();
    const std::array<std::vector<std::vector<Move>>, 64> ROOK = computeRookMoves();
    const std::array<std::vector<std::vector<Move>>, 64> BISHOP = computeBishopMoves();
    const std::array<std::vector<std::vector<Move>>, 64> KNIGHT = computeKnightMoves();
    const std::array<std::array<std::vector<std::vector<Move>>, 64>, 2> PAWN = computePawnMoves();
}

namespace Moves::Blocks {
    const std::array<std::vector<std::vector<Move>>, 64> ROOK = computeRookBlockMoves();
    const std::array<std::vector<std::vector<Move>>, 64> BISHOP = computeBishopBlockMoves();
}

/**
 * Vector of indices for rook and bishop indexing.
 */
namespace Indices {
    const std::array<std::vector<int>, 64> ROOK = computeRookReachIndices();
    const std::array<std::vector<int>, 64> BISHOP = computeBishopReachIndices();
}

namespace Reach {
    const std::array<std::vector<Bitboard>, 64> ROOK = computeRookReaches();
    const std::array<std::vector<Bitboard>, 64> BISHOP = computeBishopReaches();
}

namespace KingReach {
    const std::array<std::vector<std::vector<Square>>, 64> SQUARES = computeKingReachSquares();
}

/**
 * Rays bitboards from start (exlusive) to end (inclusive).
 */
namespace Rays {
    const std::vector<std::vector<Bitboard>> LEVEL = computeLevelRays();
    const std::vector<std::vector<Bitboard>> DIAGONAL = computeDiagonalRays();
}

/**
 * Hashes used for zobrist.
 */
namespace Hashes {
    std::array<std::array<Hash, 64>, 12> PIECES = generatePieceHashes();
    Hash TURN = generateTurnHash();
    std::array<Hash, 16> CASTLING = generateCastlingHash();
    std::array<Hash, 8> EN_PASSANT = generateEnPassantHash();
}

void Position::setCheckers() {
    checkers = 0ULL;
    checkers = isAttacked(piece_list[turn][0], (Player) !turn);
}

void Position::setPinAndCheckRayBitboards() {
    Square king_sq = piece_list[turn][0];
    Bitboard pieces = sides[WHITE] | sides[BLACK];
    rook_pins = 0ULL;
    bishop_pins = 0ULL;
    check_rays = 0ULL;

    // Enemy queen rays
    PieceType eQueen = turn == WHITE ? B_QUEEN : W_QUEEN;
    for (int i = 0; i < piece_index[eQueen]; i++) {
        Bitboard ray = Rays::DIAGONAL[king_sq][piece_list[eQueen][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) bishop_pins |= ray;
        if (oneBitSet(ray & pieces)) check_rays |= ray;

        ray = Rays::LEVEL[king_sq][piece_list[eQueen][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) rook_pins |= ray;
        if (oneBitSet(ray & pieces)) check_rays |= ray;
    }

    // Enemy rooks
    PieceType eRook = turn == WHITE ? B_ROOK : W_ROOK;
    for (int i = 0; i < piece_index[eRook]; i++) {
        Bitboard ray = Rays::LEVEL[king_sq][piece_list[eRook][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) rook_pins |= ray;
        if (oneBitSet(ray & pieces)) check_rays |= ray;
    }

    // Enemy bishops
    PieceType eBishop = turn == WHITE ? B_BISHOP : W_BISHOP;
    for (int i = 0; i < piece_index[eBishop]; i++) {
        Bitboard ray = Rays::DIAGONAL[king_sq][piece_list[eBishop][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) bishop_pins |= ray;
        if (oneBitSet(ray & pieces)) check_rays |= ray;
    }
}

void Position::getMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    setCheckers();
    if (inDoubleCheck()) {
        getKingMoves(moves_index, pos_moves);
    } else if (inCheck()) {
        setPinAndCheckRayBitboards();
        getCheckMoves(moves_index, pos_moves);
    } else {
        setPinAndCheckRayBitboards();
        getNormalMoves(moves_index, pos_moves);
    }
}

/**
 * Return true if a square is a dark square, else false.
 * @param square: The square to check.
 */
inline bool isDark(int square) {
    return (DARK >> square) & 1;
}

void Position::parseFen(std::string fen) {
    // TODO Check that a FEN is valid (see https://chess.stackexchange.com/questions/1482/how-do-you-know-when-a-fen-position-is-legal)

    // Zero out variables.
    this->resetPosition();

    // Split the string into parts and ranks
    std::vector<std::string> parts = split(fen, " ");
    std::vector<std::string> ranks = split(parts[0], "/");

    // Set the pieces
    int rank = 7;
    for (std::string rank_string : ranks) {
        int file = 0;

        for (char c : rank_string) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                for (int i = 0; i < (c - '0'); i++) {
                    this->pieces[8 * rank + file] = NO_PIECE;
                    file++;
                }
            } else {
                if (c == 'K') {
                    this->kings |= (8 * rank + file);
                    this->piece_list[WHITE][0] = (Square) (8 * rank + file);
                    this->piece_index[W_KING]++;
                    this->pieces[8 * rank + file] = W_KING;
                } else if (c == 'Q') {
                    this->queens |= (8 * rank + file);
                    this->piece_list[W_QUEEN][this->piece_index[W_QUEEN]] = (Square)(8 * rank + file);
                    this->piece_index[W_QUEEN]++;
                    this->pieces[8 * rank + file] = W_QUEEN;
                } else if (c == 'R') {
                    this->rooks |= (8 * rank + file);
                    this->piece_list[W_ROOK][this->piece_index[W_ROOK]] = (Square)(8 * rank + file);
                    this->piece_index[W_ROOK]++;
                    this->pieces[8 * rank + file] = W_ROOK;
                } else if (c == 'B') {
                    this->bishops |= (8 * rank + file);
                    this->piece_list[W_BISHOP][this->piece_index[W_BISHOP]] = (Square)(8 * rank + file);
                    this->piece_index[W_BISHOP]++;
                    this->pieces[8 * rank + file] = W_BISHOP;
                    if (isDark(8 * rank + file)) {
                        this->wdsb_cnt++;
                    } else {
                        this->wlsb_cnt++;
                    }
                } else if (c == 'N') {
                    this->knights |= (8 * rank + file);
                    this->piece_list[W_KNIGHT][this->piece_index[W_KNIGHT]] = (Square)(8 * rank + file);
                    this->piece_index[W_KNIGHT]++;
                    this->pieces[8 * rank + file] = W_KNIGHT;
                    this->knight_cnt++;
                } else if (c == 'P') {
                    this->pawns |= (8 * rank + file);
                    this->piece_list[W_PAWN][this->piece_index[W_PAWN]] = (Square)(8 * rank + file);
                    this->piece_index[W_PAWN]++;
                    this->pieces[8 * rank + file] = W_PAWN;
                } else if (c == 'k') {
                    this->kings |= (8 * rank + file);
                    this->piece_list[BLACK][0] = (Square) (8 * rank + file);
                    this->piece_index[B_KING]++;
                    this->pieces[8 * rank + file] = B_KING;
                } else if (c == 'q') {
                    this->queens |= (8 * rank + file);
                    this->piece_list[B_QUEEN][this->piece_index[B_QUEEN]] = (Square)(8 * rank + file);
                    this->piece_index[B_QUEEN]++;
                    this->pieces[8 * rank + file] = B_QUEEN;
                } else if (c == 'r') {
                    this->rooks |= (8 * rank + file);
                    this->piece_list[B_ROOK][this->piece_index[B_ROOK]] = (Square)(8 * rank + file);
                    this->piece_index[B_ROOK]++;
                    this->pieces[8 * rank + file] = B_ROOK;
                } else if (c == 'b') {
                    this->bishops |= (8 * rank + file);
                    this->piece_list[B_BISHOP][this->piece_index[B_BISHOP]] = (Square)(8 * rank + file);
                    this->piece_index[B_BISHOP]++;
                    this->pieces[8 * rank + file] = B_BISHOP;
                    if (isDark(8 * rank + file)) {
                        this->bdsb_cnt++;
                    } else {
                        this->blsb_cnt++;
                    }
                } else if (c == 'n') {
                    this->knights |= (8 * rank + file);
                    this->piece_list[B_KNIGHT][this->piece_index[B_KNIGHT]] = (Square)(8 * rank + file);
                    this->piece_index[B_KNIGHT]++;
                    this->pieces[8 * rank + file] = B_KNIGHT;
                    this->knight_cnt++;
                } else {
                    this->pawns |= (8 * rank + file);
                    this->piece_list[B_PAWN][this->piece_index[B_PAWN]] = (Square)(8 * rank + file);
                    this->piece_index[B_PAWN]++;
                    this->pieces[8 * rank + file] = B_PAWN;
                }

                // Set sides and piece count
                if (std::isupper(static_cast<unsigned char>(c))) {
                    this->sides[WHITE] |= 1ULL << (8 * rank + file);
                } else {
                    this->sides[BLACK] |= 1ULL << (8 * rank + file);
                }
                this->piece_cnt++;

                file++;
            }
        }

        rank--;
    }

    // Set turn
    if (parts[1] == "w") {
        this->turn = WHITE;
    } else {
        this->turn = BLACK;
    }

    // Set castling
    this->castling = 0;
    if (parts[2] != "-") {
        for (char c : parts[2]) {
            if (c == 'K') {
                this->castling |= 1 << WKSC;
            } else if (c == 'Q') {
                this->castling |= 1 << WQSC;
            } else if (c == 'k') {
                this->castling |= 1 << BKSC;
            } else {
                this->castling |= 1 << BQSC;
            }
        }
    }

    // Set en-passant
    if (parts[3] != "-") {
        int value = -1;
        for (char c : parts[3]) {
            if (value == -1) {
                value = (c - 'a');
            } else {
                value += 8 * (c - '1');
            }
        }
        this->en_passant = (Square) value;
    } else {
        this->en_passant = NONE;
    }

    // Set fullmoves and halfmoves.
    this->halfmove = std::stoi(parts[4]);
    this->fullmove = std::stoi(parts[5]);

    // Set ply and position hash to zero.
    this->ply = 0;
    this->hash = 0ULL;
}

void Position::resetPosition() {
    // Non-positional variables
    turn = WHITE;
    castling = 0;
    en_passant = NONE;
    halfmove = 0;
    fullmove = 0;

    // Bitboards
    sides[WHITE] = 0ULL;
    sides[BLACK] = 0ULL;
    kings = 0ULL;
    queens = 0ULL;
    rooks = 0ULL;
    bishops = 0ULL;
    knights = 0ULL;
    pawns = 0ULL;
    rook_pins = 0ULL;
    bishop_pins = 0ULL;
    check_rays = 0ULL;
    checkers = 0ULL;

    // Piece positions
    std::fill(std::begin(piece_index), std::end(piece_index), 0);
    std::fill(piece_list[0] + 0, piece_list[12] + 10, NONE);
    std::fill(std::begin(pieces), std::end(pieces), NO_PIECE);

    // Piece counts
    piece_cnt = 0;
    knight_cnt = 0;
    wdsb_cnt = 0;
    wlsb_cnt = 0;
    bdsb_cnt = 0;
    blsb_cnt = 0;

    // History
    last_move = 0;
    piece_moved = NO_PIECE;
    piece_captured = NO_PIECE;
    last_move_type = NORMAL;
    history.clear();
    hashes.clear(); // TODO reset this to whatever the default hash size is.
    hash = 0ULL;
    ply = 0;
}

/**
 * Show the usage of the program.
 *
 * @param argv: Array of command line arguments.
 */
void showUsage(char *argv[]) {
    std::cerr << "\nUsage: " << argv[0] << " white black [options]\n";
    std::cerr << "    white - Type of player for white (c or h)\n";
    std::cerr << "    black - Type of player for white (c or h)\n\n";
    std::cerr << "Options:\n";
    std::cerr << "    -f FEN            FEN input string in quotes (' or \").";
    std::cerr << "\n    -h --help         Print usage information.\n";
    std::cerr << "    -i                Invert piece colours.\n";
    std::cerr << "    -p                Print pgn.\n";
    std::cerr << "    -q                Supress output.\n";
    std::cerr << "    -u                Show unicode chess chars.\n";
    std::cerr << "    --version         Show version.\n\n";
    std::cerr << "Compiled by running 'make'.\n";
    std::cerr << "Made in development using GNU Make 4.1.\n\n";
}

/**
 * Computes the index (in this case the key) into the ROOK_BLOCK moves map.
 * @param position: The uint64_t with possible destination squares from the param square set.
 * @param square: The square from which the piece moves.
 * @return: Index into the ROOK_BLOCK array.
 */
const int Position::rookBlockIndex(uint64_t position, Square square) {
    return position & this->getRookFamily(square)->reach;
}

/**
 * Check if draw by insufficient material.
 *
 * @param game: Pointer to game struct.
 * @return: True if draw, else false.
 */
bool Position::insufficientMaterial() {
    // King vs king
    if (this->piece_cnt == 2) return true;

    // King and bishop(s) vs king (same colour bishop(s))
    if ((this->wlsb_cnt && (this->piece_cnt - this->wlsb_cnt) == 2) ||
            (this->wdsb_cnt && (this->piece_cnt - this->wdsb_cnt) == 2) ||
            (this->blsb_cnt && (this->piece_cnt - this->blsb_cnt) == 2) ||
            (this->bdsb_cnt && (this->piece_cnt - this->bdsb_cnt) == 2)) {
        return true;
    }

    // King and bishop(s) vs king and bishop(s) (same colour bishop(s))
    if ((this->wlsb_cnt && this->blsb_cnt && (this->piece_cnt -
            this->wlsb_cnt - this->blsb_cnt) == 2) || (this->wdsb_cnt &&
            this->bdsb_cnt && (this->piece_cnt - this->wdsb_cnt -
            this->bdsb_cnt) == 2)) {
        return true;
    }

    // King and knight vs king
    if (this->knight_cnt == 1 && this->piece_cnt == 3) return true;

    return false;
}

bool Position::inCheck() {
    return checkers;
}

/**
 * Get the bitboard of possible moves for a king on sq.
 */
Bitboard Position::getKingAttackBitBoard() const {
    Bitboard result = 0ULL;
    // for (int sq : Moves::KING[this->piece_list[this->turn][0]].block_bits) {
    //     if (!this->getKingAttackers((Square) sq, !this->turn)) {
    //         result |= sq;
    //     }
    // }
    return result;
}

void Position::getKingMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    Square kingSquare = piece_list[turn][0];
    Bitboard reachBB = kingMasks[kingSquare] & ~sides[turn];

    if (reachBB == 0ULL) return;

    for (Square square : KingReach::SQUARES[kingSquare][getKingMovesIndex(reachBB, kingSquare)]) {
        if (isAttacked(square, (Player) !turn, true)) {
            reachBB &= ~(1ULL << square);
        }
    }

    pos_moves[moves_index++] = &Moves::KING[kingSquare][getKingMovesIndex(reachBB, kingSquare)];
}

void Position::getCheckMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    getKingMoves(moves_index, pos_moves);
    getQueenCheckedMoves(moves_index, pos_moves);
    getRookCheckedMoves(moves_index, pos_moves);
    getBishopCheckedMoves(moves_index, pos_moves);
    getKnightCheckedMoves(moves_index, pos_moves);
    getPawnCheckedMoves(moves_index, pos_moves);
}

void Position::getQueenCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // For each queen
    //      if not pinned
    //            find rook equivalent blocks and capturing moves
    //            find bishop equivalent block and capturing moves
}

void Position::getRookCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // For rook
    //      if not pinned
    //          find block and capturing moves
}

void Position::getBishopCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // For bishop
    //      if not pinned
    //          find block and capturing moves
}

void Position::getKnightCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // For knight
    //      if not pinned
    //          find block and capturing moves
}

void Position::getPawnCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // For pawn
    //      if not pinned
    //          find block and capturing moves
}

/**
 * Gets a bitboard of the rays between the bishop (queen) checkers and the king
 * being checked.
 *
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
Bitboard Position::getBishopCheckRays(Square square, Bitboard& checkers_only) {
    Bitboard result = 0;
    bool turn = this->turn;
    Square king_sq = this->piece_list[turn][0];
    Bitboard king_rays = this->getBishopFamily(king_sq)->reach & ~this->sides[turn];
    checkers_only |= (this->queens, this->bishops) & this->sides[!turn] & king_rays;
    PieceType queen = turn ? B_QUEEN : W_QUEEN;
    PieceType bishop = turn ? B_BISHOP : W_BISHOP;

    for (int i = 0; i < this->piece_index[queen]; i++) {
        Square piece = this->piece_list[queen][i];
        if (std::abs(piece % 8 - king_sq % 8) == std::abs(piece / 8 - king_sq / 8)) {
            result |= (this->getBishopFamily((Square) piece)->reach & ~this->sides[!turn] & king_rays);
        }
    }

    for (int i = 0; i < this->piece_index[bishop]; i++) {
        Square piece = this->piece_list[bishop][i];
        if (std::abs(piece % 8 - king_sq % 8) == std::abs(piece / 8 - king_sq / 8)) {
            result |= (this->getBishopFamily((Square) piece)->reach & ~this->sides[!turn] & king_rays);
        }
    }

    return result;
}

Player Position::getTurn() {
    return this->turn;
}

/**
 * Gets a bitboard of the rays between the rook (queen) checkers and the king
 * being checked.
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers: Bitboard of checkers.
 * @return: Check rays of rooks and queens.
 */
Bitboard Position::getRookCheckRays(Square square, Bitboard& checkers) {
    Bitboard result = 0;
    Square king_sq = this->piece_list[this->turn][0];
    uint64_t king_rays = this->getRookFamily(king_sq)->reach & ~this->sides[this->turn];
    checkers |= (this->queens, this->rooks) & this->sides[1 - this->turn] & king_rays;
    PieceType queen = this->turn ? B_QUEEN : W_QUEEN;
    PieceType rook = this->turn ? B_ROOK : W_ROOK;

    for (int i = 0; i < this->piece_index[queen]; i++) {
        Square queen_sq = this->piece_list[queen][i];
        if (queen_sq % 8 == king_sq % 8 || queen_sq / 8 == king_sq / 8) {
            result |= (this->getRookFamily((Square) queen_sq)->reach & ~this->sides[1 - this->turn] & king_rays);
        }
    }

    for (int i = 0; i < this->piece_index[rook]; i++) {
        Square rook_sq = this->piece_list[rook][i];
        if (rook_sq % 8 == king_sq % 8 || rook_sq / 8 == king_sq / 8) {
            result |= (this->getRookFamily((Square) rook_sq)->reach & ~this->sides[1 - this->turn] & king_rays);
        }
    }

    return result;
}

/**
 * Returns a bitboard of all pawn checkers.
 * @param game: The game struct pointer of the current position.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
Bitboard Position::getPawnCheckers(Square square, Bitboard& checkers_only) {
    Bitboard enemy_pawns = this->sides[1 - this->turn] & this->pawns;
    Bitboard result = 0;
    int rank_offset = this->turn ? 8 : -8;
    if (square % 8 != 0) {
        int pawn_sq = square + rank_offset - 1;
        if ((pawn_sq) & enemy_pawns) checkers_only |= pawn_sq;
    }

    if (square % 8 != 7) {
        int pawn_sq = square + rank_offset + 1;
        if ((pawn_sq) & enemy_pawns) checkers_only |= pawn_sq;
    }

    return result;
}

/**
 * Computes the initial hash of the position. Basically, just computes the current hash.
 */
void Position::initialiseHash() {
    // this->hash = 0ULL;

    // // Hash the pieces
    // for (int sq = A1; sq <= H8; sq++) {
    //     if (this->pieces[sq] != NO_PIECE) {
    //         this->hash ^= Hashes::PIECES[this->pieces[sq]][sq];
    //     }
    // }

    // // Hash turn
    // if (this->turn == BLACK) this->hash ^= Hashes::TURN;

    // // Hash the castling
    // this->hash ^= Hashes::CASTLING[this->castling];

    // // Hash en-passant
    // if (this->en_passant != NONE) {
    //     this->hash ^= Hashes::EN_PASSANT[this->en_passant % 8];
    // }
}

void Position::getNormalMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    this->getKingMoves(moves_index, pos_moves);
    // this->getQueenMoves(moves_index, pos_moves);
    // this->getRookMoves(moves_index, pos_moves);
    // this->getBishopMoves(moves_index, pos_moves);
    // this->getKnightMoves(moves_index, pos_moves);
    // this->getPawnMoves(moves_index, pos_moves);
    // this->getCastlingMoves(moves_index, pos_moves);
}

void Position::getQueenMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // check for rook pin, bishop pin
    // if rook pinned, get rook pinned moves
    // if bishop pinned, get bishop pinned moves
    // Else get normal moves
}

void Position::getRookMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // check for rook pin, bishop pin
    // if rook pinned, get rook pinned moves
    // if bishop pinned, get bishop pinned moves
    // Else get normal moves
}

void Position::getBishopMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // check for rook pin, bishop pin
    // if rook pinned, get rook pinned moves
    // if bishop pinned, get bishop pinned moves
    // Else get normal moves
}

void Position::getKnightMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // check for rook pin, bishop pin
    // if rook pinned or bishop pinned, ignore
    // Else proceed
}

void Position::getPawnMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    // check for rook pin, bishop pin
    // if rook pinned, get rook pinned moves
    // if bishop pinned, get bishop pinned moves
    // Else proceed as normal
}

inline Bitboard Position::getRookReachBB(Bitboard occupancy, Square square) {
    return Reach::ROOK[square][getRookReachIndex(occupancy, square)];
}

inline Bitboard Position::getBishopReachBB(Bitboard occupancy, Square square) {
    return Reach::BISHOP[square][getBishopReachIndex(occupancy, square)];
}

/**
 * Get the arguments for the pawn index function for pawns.
 * @param game: Pointer to game struct.
 * @param square: Square of pawn.
 */
uint64_t Position::pawnMoveArgs(Square square) {
    bool turn = this->turn;
    return (this->sides[!turn], (this->sides[turn] & files[square % 8]));
}

/**
 * Check if sq is occupied by a piece.
 * @param sq: Square to check.
 * @return: True if occupied, else false.
 */
Bitboard Position::isOccupied(const Square sq) {
    return ((this->sides[WHITE], this->sides[BLACK]) & (sq));
}

void Position::getCastlingMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {

}

/**
 * Gets a bitboard of knight checkers.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the king in check.
 * @param checkers_only: A pointer to a bit board of checkers only.
 * @return: Bitboard of knight checkers.
 */
Bitboard Position::getKnightCheckers(Square square, Bitboard& checkers_only) {
    // Bitboard result = 0;
    // checkers_only |= Moves::KNIGHT[square].reach & this->sides[1 - this->turn] & this->knights;
    // return result;
    return 0ULL;
}

bool Position::inDoubleCheck() {
    return (checkers & (checkers - 1)) != 0;
}

/**
 * Finds and returns a pointer to a rook move family.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* Position::getRookFamily(Square square) {
    // return &Moves::ROOK[Indices::ROOK[square][rookIndex(this->sides[BLACK], this->sides[WHITE], square)]];
    return nullptr;
}

/**
 * Finds and returns a pointer to a bishop move family.
 *
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the bishop is on.
 */
MovesStruct* Position::getBishopFamily(Square square) {
    // return &Moves::BISHOP[Indices::BISHOP[square][bishopIndex(this->sides[BLACK], this->sides[WHITE], square)]];
    return nullptr;
}

/**
 * Checks if a three fold repetition occured. If so, return true, else false.
 * Also performs the hash updates for the history.
 * @param game: Pointer to game struct.
 */
bool Position::isThreeFoldRep() {
    auto val = this->hashes.find(this->hash);
    if (val != this->hashes.end() && val->second >= 3) {
        return true;
    }
    return false;
}

/**
 * Checks if end of game has been reached.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves.
 * @param moves_index: Pointer to number of vectors in pos_moves.
 * @return: The appropriate ExitCode.
 */
ExitCode Position::isEOG(MoveList& move_list) {
    if (this->isThreeFoldRep()) return THREE_FOLD_REPETITION;

    if (this->halfmove == 100) return FIFTY_MOVES_RULE;

    if (this->insufficientMaterial()) return INSUFFICIENT_MATERIAL;

    if (move_list.moves_index == 0 && !(this->inCheck())) return STALEMATE;

    if (move_list.moves_index == 0 && this->inCheck()) {
        if (this->turn) return BLACK_WINS;
        return WHITE_WINS;
    }

    return NORMAL_PLY;
}

Bitboard Position::isAttacked(const Square square, const Player player, const bool ignoreKing) {
    // Get bitboard of all pieces and mask out !"player" king if indicated.
    Bitboard pieces = (sides[WHITE] | sides[BLACK]) ^ (ignoreKing ? 1ULL << piece_list[!player][0] : 0ULL);

    // Check if square is attacked by "player" king.
    Bitboard king = kingMasks[square] & this->sides[turn] & this->kings;

    // Check if square is attacked by "player" queen or rook horizontally or vertically.
    Bitboard rooks = getRookReachBB(rookMasks[square] & pieces, square) & sides[player] & (queens | rooks);

    // Check if square is attacked by "player" queen or bishop diagonally.
    Bitboard bishops = getBishopReachBB(bishopMasks[square] & pieces, square) & sides[player] & (queens | bishops);

    // Check if square is attacked by "player" knight.
    Bitboard knights = knightMasks[square] & sides[player] & knights;

    // Check if square is attacked by "player" pawn.
    Bitboard pawns = pawnMasks[!player][square] & ~files[square % 8] & sides[player] & pawns;

    return king | rooks | bishops | knights | pawns;
}

/**
 * Is sq attacked by pieces of player turn but with the !turn king masked out.
 * @param sq: Square to check.
 * @param turn: Current turn.
 */
Bitboard Position::getKingAttackers(const Square sq, const bool turn) const {
    // Bitboard pieces = (this->sides[WHITE], this->sides[BLACK]) ^ (this->piece_list[!turn][0]);
    // // Bitboard king = Moves::KING[sq].reach & this->sides[turn] & this->kings;
    // Bitboard rooks = Moves::ROOK[Indices::ROOK[sq][rookIndex(pieces, sq)]].reach & this->sides[turn] &
    //         (this->queens, this->rooks);
    // Bitboard bishops = Moves::BISHOP[Indices::BISHOP[sq][bishopIndex(pieces, sq)]].reach & this->sides[turn] &
    //         (this->queens, this->bishops);
    // Bitboard knights = Moves::KNIGHT[sq].reach & this->knights & this->sides[turn];
    // Bitboard pawns = Moves::PAWN[!turn][sq].reach & this->pawns & this->sides[turn] & ~files[sq % 8];
    // return (rooks, bishops, knights, pawns);
    return 0ULL;
}

Hash Position::getHash() {
    return this->hash;
}

/**
 * Find the piece in their associated piece list and removes it.
 * @param game: Pointer to game struct.
 * @param piece: The piece type to find and remove.
 * @param square: The square of the piece type to find and remove.
 */
void Position::findAndRemovePiece(PieceType piece, Square square) {
    int taken_index = -1;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        if (this->piece_list[piece][i] == square) {
            taken_index = i;
            break;
        }
    }

    this->piece_index[piece]--;
    this->piece_list[piece][taken_index] = this->piece_list[piece][this->piece_index[piece]];
    if (piece == W_BISHOP) {
        if (isDark(square)) {
            this->wdsb_cnt--;
        } else {
            this->wlsb_cnt--;
        }
    } else if (piece == B_BISHOP) {
        if (isDark(square)) {
            this->bdsb_cnt--;
        } else {
            this->blsb_cnt--;
        }
    } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
        this->knight_cnt--;
    }
    this->piece_cnt--;
}

/**
 * Adds a piece to the end of the appropriate piece list.
 * @param game: Pointer to game struct.
 * @param piece: The piece type to add.
 * @param square: The square to add for piece.
 */
void Position::addPiece(PieceType piece, Square square) {
    this->piece_list[piece][this->piece_index[piece]] = square;
    this->piece_index[piece]++;
    if (piece == W_BISHOP) {
        if (isDark(square)) {
            this->wdsb_cnt++;
        } else {
            this->wlsb_cnt++;
        }
    } else if (piece == B_BISHOP) {
        if (isDark(square)) {
            this->bdsb_cnt++;
        } else {
            this->blsb_cnt++;
        }
    } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
        this->knight_cnt++;
    }
    this->piece_cnt++;
}

/**
 * Process the removal of a piece just captured, if any.
 * @param game: Pointer to game struct.
 */
void Position::removePiece() {
    PieceType captured = this->piece_captured;
    if (captured == NO_PIECE) return;
    int end = (this->last_move >> 6) & 0b111111;
    if (captured == W_QUEEN || captured == B_QUEEN) {
        this->queens &= ~(end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_ROOK || captured == B_ROOK) {
        this->rooks &= ~(end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_BISHOP || captured == B_BISHOP) {
        this->bishops &= ~(end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_KNIGHT || captured == B_KNIGHT) {
        this->knights &= ~(end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_PAWN || captured == B_PAWN) {
        this->pawns &= ~(end);
        this->findAndRemovePiece(captured, (Square) end);
    }
}

/**
 * Process castling.
 * @param game: Pointer to game struct.
 */
void Position::handleCastle() {
    // int start, end;
    // int last_end = (this->last_move >> 6) & 0b111111;
    // PieceType rook;

    // if (last_end  == G1) {
    //     start = H1;
    //     end = F1;
    //     rook = W_ROOK;
    // } else if (last_end == C1) {
    //     start = A1;
    //     end = D1;
    //     rook = W_ROOK;
    // } else if (last_end == G8) {
    //     start = H8;
    //     end = F8;
    //     rook = B_ROOK;
    // } else {
    //     start = A8;
    //     end = D8;
    //     rook = B_ROOK;
    // }

    // this->sides[this->turn] &= ~(start);
    // this->sides[this->turn] |= end;
    // this->pieces[start] = NO_PIECE;
    // this->hash ^= Hashes::PIECES[rook][start];
    // this->pieces[end] = rook;
    // this->hash ^= Hashes::PIECES[rook][end];

    // this->findAndRemovePiece(rook, (Square) start);
    // this->addPiece(rook, (Square) end);
    // this->rooks &= ~(start);
    // this->rooks |= end;
}

/**
 * Make the king move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Position::makeKingMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    this->piece_list[this->turn][0] = (Square) end;

    // Remove castling rights
    if (this->turn) {
        this->castling &= ~(1 << WKSC, 1 << WQSC);
    } else {
        this->castling &= ~(1 << BKSC, 1 << BQSC);
    }

    if (move_type == CASTLING) {
        this->handleCastle();
    }

    // Update bitboards.
    this->kings &= ~(start);
    this->kings |= end;
}

/**
 * Make the queen move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Position::makeQueenMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
    this->findAndRemovePiece(queen, (Square) start);
    this->addPiece(queen, (Square) end);
    this->queens &= ~(start);
    this->queens |= end;
}

/**
 * Make the rook move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Position::makeRookMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType rook = this->turn ? W_ROOK : B_ROOK;
    this->findAndRemovePiece(rook, (Square) start);
    this->addPiece(rook, (Square) end);
    this->rooks &= ~(start);
    this->rooks |= end;
}

/**
 * Make the bishop move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Position::makeBishopMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
    this->findAndRemovePiece(bishop, (Square) start);
    this->addPiece(bishop, (Square) end);
    this->bishops &= ~(start);
    this->bishops |= end;
}

/**
 * Make the knight move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Position::makeKnightMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
    this->findAndRemovePiece(knight, (Square) start);
    this->addPiece(knight, (Square) end);
    this->knights &= ~(start);
    this->knights |= end;
}

/**
 * Prints a readable version of a move.
 * @param move: The move to be printed.
 */
void printMove(Move move, bool extraInfo) {
    if (extraInfo) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111] << " " << moveName[(move >> 12) &
                0b11] << " " << promoName[(move >> 14) & 0b11];
    } else if ((move & (0b11 << 12)) == PROMOTION) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &0b111111] << promoName[(move >> 14) & 0b11];
    } else {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111];
    }
}

/**
 * Make the pawn move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Position::makePawnMoves(Move move) {
//     this->removePiece();
//     int start = move & 0b111111;
//     int end = (move >> 6) & 0b111111;
//     int move_type = move & (0b11 << 12);
//     int promo = move & (0b11 << 14);
//     bool pawn = false;
//     int ep = end + (this->turn ? -8 : 8);
//     PieceType piece = this->turn ? W_PAWN : B_PAWN;
//     this->findAndRemovePiece(piece, (Square) start);

//     if (move_type == PROMOTION) {
//         if (this->turn) {
//             piece = promo == pQUEEN ? W_QUEEN : promo == pROOK ? W_ROOK : promo == pBISHOP ? W_BISHOP : W_KNIGHT;
//         } else {
//             piece = promo == pQUEEN ? B_QUEEN : promo == pROOK ? B_ROOK : promo == pBISHOP ? B_BISHOP : B_KNIGHT;
//         }

//         if (promo == pQUEEN) {
//             this->queens |= end;
//             PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
//             this->addPiece(queen, (Square) end);
//         } else if (promo == pROOK) {
//             this->rooks |= end;
//             PieceType ROOK = this->turn ? W_ROOK : B_ROOK;
//             this->addPiece(ROOK, (Square) end);
//         } else if (promo == pBISHOP) {
//             this->bishops |= end;
//             PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
//             this->addPiece(bishop, (Square) end);
//         } else {
//             this->knights |= end;
//             PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
//             this->addPiece(knight, (Square) end);
//         }
//         this->pieces[end] = piece;
//         this->hash ^= Hashes::PIECES[piece][end];
//     } else if (move_type == EN_PASSANT) {
//         pawn = true;
//         this->addPiece(piece, (Square) end);
//         piece = this->turn ? B_PAWN : W_PAWN;
//         this->findAndRemovePiece(piece, (Square) ep);
//         this->pawns &= ~(ep);
//         this->sides[1 - this->turn] &= ~(ep);
//         this->pieces[ep] = NO_PIECE;
//         this->hash ^= Hashes::PIECES[piece][ep];
//     } else {
//         pawn = true;
//         this->addPiece(piece, (Square) end);
//         int rank_diff = end / 8 - start / 8;
//         if (std::abs(rank_diff) != 1) {
//             this->en_passant = (Square) (start + 8 * (rank_diff - (rank_diff > 0 ? 1 : -1)));
//             this->hash ^= Hashes::EN_PASSANT[this->en_passant % 8];
//         }
//     }

//     this->pawns &= ~(start);
//     this->pawns |= (uint64_t)pawn << end;
// }

// /**
//  * Save game state to history for undoing moves.
//  * @param game: Pointer to game struct.
//  * @param move: The move to made.
//  */
// void Position::saveHistory(Move move) {
//     this->history[this->ply].castling = this->castling;
//     this->history[this->ply].en_passant = this->en_passant;
//     this->history[this->ply].halfmove = this->halfmove;
//     this->history[this->ply].move = move;
//     this->history[this->ply].captured = this->pieces[(move >> 6) & 0b111111];
//     this->history[this->ply].hash = this->hash;
//     this->ply++;

//     if (this->ply >= MAX_MOVES) {
//         this->history.resize(2 * MAX_MOVES);
//     }
}

/**
 * Undo normal moves.
 * @param game: Pointer to game struct.
 */
void Position::undoNormal() {
    // // Change game history
    // this->ply--;
    // History previous_pos = this->history[this->ply];

    // // Retrieve last move information
    // Move move = previous_pos.move;
    // int start = move & 0b111111;
    // int end = (move >> 6) & 0b111111;
    // PieceType moved = this->pieces[end];
    // PieceType captured = previous_pos.captured;

    // // Change turn
    // turn = (Player)!turn;
    // bool turn = this->turn;

    // // Change sides bitboards
    // this->sides[turn] &= ~(end);
    // this->sides[turn] |= start;
    // if (captured != NO_PIECE) this->sides[!turn] |= end;

    // // Change pieces bitboards, piece list and indices and piece counts
    // if (moved == B_PAWN || moved == W_PAWN) {
    //     this->pawns &= ~(end);
    //     this->findAndRemovePiece(moved, (Square) end);
    //     this->pawns |= start;
    //     this->addPiece(moved, (Square) start);
    // } else if (moved == B_KNIGHT || moved == W_KNIGHT) {
    //     this->knights &= ~(end);
    //     this->findAndRemovePiece(moved, (Square) end);
    //     this->knights |= start;
    //     this->addPiece(moved, (Square) start);
    // } else if (moved == B_BISHOP || moved == W_BISHOP) {
    //     this->bishops &= ~(end);
    //     this->findAndRemovePiece(moved, (Square) end);
    //     this->bishops |= start;
    //     this->addPiece(moved, (Square) start);
    // } else if (moved == B_ROOK || moved == W_ROOK) {
    //     this->rooks &= ~(end);
    //     this->findAndRemovePiece(moved, (Square) end);
    //     this->rooks |= start;
    //     this->addPiece(moved, (Square) start);
    // } else if (moved == B_QUEEN || moved == W_QUEEN) {
    //     this->queens &= ~(end);
    //     this->findAndRemovePiece(moved, (Square) end);
    //     this->queens |= start;
    //     this->addPiece(moved, (Square) start);
    // } else {
    //     this->kings &= ~(end);
    //     this->findAndRemovePiece(moved, (Square) end);
    //     this->kings |= start;
    //     this->addPiece(moved, (Square) start);
    // }

    // if (captured == NO_PIECE) {
    //     /* Do nothing */
    // } else if (captured == B_PAWN || captured == W_PAWN) {
    //     this->pawns |= end;
    //     this->addPiece(captured, (Square) end);
    // } else if (captured == B_KNIGHT || captured == W_KNIGHT) {
    //     this->knights |= end;
    //     this->addPiece(captured, (Square) end);
    // } else if (captured == B_BISHOP || captured == W_BISHOP) {
    //     this->bishops |= end;
    //     addPiece(captured, (Square) end);
    // } else if (captured == B_ROOK || captured == W_ROOK) {
    //     this->rooks |= end;
    //     this->addPiece(captured, (Square) end);
    // } else {
    //     this->queens |= end;
    //     this->addPiece(captured, (Square) end);
    // }

    // // Undo fullmove and History struct information
    // if (turn) this->fullmove--;
    // this->castling = previous_pos.castling;
    // this->en_passant = previous_pos.en_passant;
    // this->halfmove = previous_pos.halfmove;
    // this->hash = previous_pos.hash;

    // // Update pieces array
    // this->pieces[start] = moved;
    // this->pieces[end] = captured;
}

/**
 * Undo promotion moves.
 * @param game: Pointer to game struct.
 */
void Position::undoPromotion() {
    // // Change game history
    // this->ply--;
    // History previous_pos = this->history[this->ply];

    // // Change turn
    // this->turn = 1 - this->turn;
    // bool turn = this->turn;

    // // Retrieve last move information
    // Move move = previous_pos.move;
    // int start = move & 0b111111;
    // int end = (move >> 6) & 0b111111;
    // PieceType promoted = this->pieces[end];
    // PieceType captured = previous_pos.captured;
    // PieceType pawn = turn ? W_PAWN : B_PAWN;

    // // Remove promoted piece
    // this->findAndRemovePiece(promoted, (Square) end);
    // this->sides[turn] &= ~(end);
    // this->pieces[end] = NO_PIECE;
    // if (promoted == W_QUEEN || promoted == B_QUEEN) {
    //     this->queens &= ~(end);
    // } else if (promoted == W_ROOK || promoted == B_ROOK) {
    //     this->rooks &= ~(end);
    // } else if (promoted == W_BISHOP || promoted == B_BISHOP) {
    //     this->bishops &= ~(end);
    // } else { // Knight
    //     this->knights &= ~(end);
    // }

    // // Replace captured piece (if any)
    // if (captured != NO_PIECE) {
    //     this->addPiece(captured, (Square) end);
    //     this->sides[!turn] |= end;
    //     this->pieces[end] = captured;
    //     if (captured == W_QUEEN || captured == B_QUEEN) {
    //         this->queens |= end;
    //     } else if (captured == W_ROOK || captured == B_ROOK) {
    //         this->rooks |= end;
    //     } else if (captured == W_BISHOP || captured == B_BISHOP) {
    //         this->bishops |= end;
    //     } else { // Knight
    //         this->knights |= end;
    //     }
    // }

    // // Replace pawn
    // this->addPiece(pawn, (Square) start);
    // this->sides[turn] |= start;
    // this->pawns |= start;
    // this->pieces[start] = pawn;

    // // Undo fullmove and History struct information
    // if (turn) this->fullmove--;
    // this->castling = previous_pos.castling;
    // this->en_passant = previous_pos.en_passant;
    // this->halfmove = previous_pos.halfmove;
    // this->hash = previous_pos.hash;
}

/**
 * Undo en-passant moves.
 * @param game: Pointer to game struct.
 */
void Position::undoEnPassant() {
    // // Change game history
    // this->ply--;
    // History previous_pos = this->history[this->ply];

    // // Change turn
    // this->turn = 1 - this->turn;
    // bool turn = this->turn;

    // // Retrieve last move information
    // Move move = previous_pos.move;
    // int start = move & 0b111111;
    // int end = (move >> 6) & 0b111111;
    // PieceType moved = this->pieces[end];
    // PieceType captured = turn ? B_PAWN : W_PAWN;
    // int captured_sq = end + (turn ? -8 : 8);

    // // Change sides bitboards
    // this->sides[turn] &= ~(end);
    // this->sides[turn] |= start;
    // this->sides[!turn] |= captured_sq;

    // // Change pawn bitboards
    // this->pawns &= ~(end);
    // this->pawns |= start;
    // this->pawns |= captured_sq;

    // // Undo fullmove and History struct information
    // if (turn) this->fullmove--;
    // this->castling = previous_pos.castling;
    // this->en_passant = previous_pos.en_passant;
    // this->halfmove = previous_pos.halfmove;
    // this->hash = previous_pos.hash;

    // // Update piece list, indices and counts
    // this->findAndRemovePiece(moved, (Square) end);
    // this->addPiece(moved, (Square) start);
    // this->addPiece(captured, (Square) captured_sq);

    // // Update pieces array
    // this->pieces[start] = moved;
    // this->pieces[end] = NO_PIECE;
    // this->pieces[captured_sq] = captured;
}

/**
 * Undo castling moves.
 * @param game: Pointer to game struct.
 */
void Position::undoCastling() {
    // // Change game history
    // this->ply--;
    // History previous_pos = this->history[this->ply];

    // // Change turn
    // this->turn = 1 - this->turn;
    // bool turn = this->turn;

    // // Retrieve last move information
    // Move move = previous_pos.move;
    // int start = move & 0b111111;
    // int end = (move >> 6) & 0b111111;
    // PieceType moved = this->pieces[end];

    // // Rook changes
    // int rook_start, rook_end;
    // PieceType rook = turn ? W_ROOK : B_ROOK;
    // if (end == G1) {
    //     rook_start = H1;
    //     rook_end = F1;
    // } else if (end == C1) {
    //     rook_start = A1;
    //     rook_end = D1;
    // } else if (end == G8) {
    //     rook_start = H8;
    //     rook_end = F8;
    // } else {
    //     rook_start = A8;
    //     rook_end = D8;
    // }

    // // Change sides bitboards
    // this->sides[turn] &= ~(end);
    // this->sides[turn] |= start;
    // this->sides[turn] &= ~(rook_end);
    // this->sides[turn] |= rook_start;

    // // Change king bitboards
    // this->kings &= ~(end);
    // this->kings |= start;

    // // Change rook bitboards
    // this->rooks &= ~(rook_end);
    // this->rooks |= rook_start;

    // // Undo fullmove and History struct information (some is not necessary i think? Done out of principle)
    // if (turn) this->fullmove--;
    // this->castling = previous_pos.castling;
    // this->en_passant = previous_pos.en_passant;
    // this->halfmove = previous_pos.halfmove;
    // this->hash = previous_pos.hash;

    // // Update piece list, indices and counts
    // this->findAndRemovePiece(moved, (Square) end);
    // this->addPiece(moved, (Square) start);
    // this->findAndRemovePiece(rook, (Square) rook_end);
    // this->addPiece(rook, (Square) rook_start);

    // // Update pieces array
    // this->pieces[start] = moved;
    // this->pieces[end] = NO_PIECE;
    // this->pieces[rook_start] = rook;
    // this->pieces[rook_end] = NO_PIECE;
}

/**
 * Decrements the position hash counter for undo moves.
 * @param hash: The hash to decrement.
 */
void Position::decrementHash(const Bitboard hash) {
    auto it = this->hashes.find(hash);
    if (it != this->hashes.end()) {
        this->hashes[hash]--;
    }
}

/**
 * Undo moves.
 * @param game: Pointer to game struct.
 */
void Position::undoMove() {
    if (this->ply == 0) {
        std::cout << "Nothing to undo...\n";
        return;
    }

    this->decrementHash(this->hash);
    int type = this->history[this->ply - 1].move & (0b11 << 12);
    if (type == NORMAL) {
        this->undoNormal();
    } else if (type == PROMOTION) {
        this->undoPromotion();
    } else if (type == EN_PASSANT) {
        this->undoEnPassant();
    } else {
        this->undoCastling();
    }
}

/**
 * Make the moves.
 * @param move: The moves to make.
 * @param game: Pointer to game struct.
 */
void Position::makeMove(Move move) {
    // if (move == 0) {
    //     this->undoMove();
    //     return;
    // }

    // // Save current position and move to make to history.
    // this->saveHistory(move);

    // int start = move & 0b111111;
    // int end = (move >> 6) & 0b111111;
    // int move_type = move & (0b11 << 12);

    // this->last_move = move;
    // this->last_move_type = (MoveType) move_type;
    // this->piece_captured = this->pieces[end];
    // this->piece_moved = this->pieces[start];

    // // Remove en-passant and its hash
    // if (this->en_passant != NONE) {
    //     this->hash ^= Hashes::EN_PASSANT[this->en_passant % 8];
    //     this->en_passant = NONE;
    // }

    // this->sides[this->turn] &= ~(start);
    // this->sides[this->turn] |= end;
    // this->sides[1 - this->turn] &= ~(end);
    // this->pieces[start] = NO_PIECE;
    // this->hash ^= Hashes::PIECES[this->piece_moved][start];
    // this->pieces[end] = this->piece_moved;
    // if (this->piece_captured != NO_PIECE) {
    //     this->hash ^= Hashes::PIECES[this->piece_captured][end];
    // }
    // this->hash ^= Hashes::PIECES[this->piece_moved][end];

    // // Change castling privileges.
    // if (start == E1 && 0b11 & this->castling) {
    //     this->hash ^= Hashes::CASTLING[this->castling];
    //     this->castling &= 0b1100;
    //     this->hash ^= Hashes::CASTLING[this->castling];
    // }

    // if (start == E8 && (0b11 << 2) & this->castling) {
    //     this->hash ^= Hashes::CASTLING[this->castling];
    //     this->castling &= 0b0011;
    //     this->hash ^= Hashes::CASTLING[this->castling];
    // }

    // if ((start == H1 || end == H1) && this->castling & (1 << WKSC)) {
    //     this->hash ^= Hashes::CASTLING[this->castling];
    //     this->castling &= ~(1 << WKSC);
    //     this->hash ^= Hashes::CASTLING[this->castling];
    // }

    // if ((start == A1 || end == A1) && this->castling & (1 << WQSC)) {
    //     this->hash ^= Hashes::CASTLING[this->castling];
    //     this->castling &= ~(1 << WQSC);
    //     this->hash ^= Hashes::CASTLING[this->castling];
    // }

    // if ((start == H8 || end == H8) && this->castling & (1 << BKSC)) {
    //     this->hash ^= Hashes::CASTLING[this->castling];
    //     this->castling &= ~(1 << BKSC);
    //     this->hash ^= Hashes::CASTLING[this->castling];
    // }

    // if ((start == A8 || end == A8) && this->castling & (1 << BQSC)) {
    //     this->hash ^= Hashes::CASTLING[this->castling];
    //     this->castling &= ~(1 << BQSC);
    //     this->hash ^= Hashes::CASTLING[this->castling];
    // }

    // PieceType moved = this->piece_moved;
    // if (moved == W_KING || moved == B_KING) {
    //     this->makeKingMoves(move);
    // } else if (moved == W_QUEEN || moved == B_QUEEN) {
    //     this->makeQueenMoves(move);
    // } else if (moved == W_ROOK || moved == B_ROOK) {
    //     this->makeRookMoves(move);
    // } else if (moved == W_BISHOP || moved == B_BISHOP) {
    //     this->makeBishopMoves(move);
    // } else if (moved == W_KNIGHT || moved == B_KNIGHT) {
    //     this->makeKnightMoves(move);
    // } else {
    //     this->makePawnMoves(move);
    // }

    // this->turn = 1 - this->turn;
    // if (this->turn == BLACK) {
    //     this->fullmove++;
    //     this->hash ^= Hashes::TURN;
    // }
    // if (moved == W_PAWN || moved == B_PAWN || this->piece_captured != NO_PIECE) {
    //     this->halfmove = 0;
    // } else {
    //     this->halfmove++;
    // }

    // // Increment position hash counter
    // this->incrementHash(move);
}

void Position::incrementHash(Move move) {
    auto record = this->hashes.find(this->hash);
    if (record != this->hashes.end()) {
        this->hashes[this->hash]++;
    } else {
        this->hashes.insert(std::pair<Bitboard, int>(this->hash, 1));
    }
}

/**
 * Checks if a given move is valid.
 * @param move: The move to check.
 * @param game: Pointer to game move struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Int pointer to number of vectors in pos_moves.
 * @return: True if move is valid, else false.
 */
bool Position::validMove(Move move, MoveList& move_list) {
    for (int i = 0; i < move_list.moves_index; i++) {
        for (Move move_candidate : *move_list.moveSets[i]) {
            if (move == move_candidate) return true;
        }
    }
    return false;
}

/**
 * Get and return the FEN string of the current position.
 * @param game: Pointer to game struct.
 */
std::string Position::getFEN() {
    std::string fen = "";
    int no_piece_count = 0;
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file <= 7; file++) {
            int sq = rank * 8 + file;

            if (sq % 8 == 0 && sq != A8) {
                if (no_piece_count != 0) {
                    fen += no_piece_count + '0';
                    no_piece_count = 0;
                }
                fen += "/";
            }

            PieceType piece = this->pieces[sq];
            if (piece == NO_PIECE) {
                no_piece_count++;
            } else {
                if (no_piece_count != 0) {
                    fen += no_piece_count + '0';
                    no_piece_count = 0;
                }
                if (piece == B_KING) fen += 'k';
                else if (piece == B_QUEEN) fen += 'q';
                else if (piece == B_ROOK) fen += 'r';
                else if (piece == B_BISHOP) fen += 'b';
                else if (piece == B_KNIGHT) fen += 'n';
                else if (piece == B_PAWN) fen += 'p';
                else if (piece == W_KING) fen += 'K';
                else if (piece == W_QUEEN) fen += 'Q';
                else if (piece == W_ROOK) fen += 'R';
                else if (piece == W_BISHOP) fen += 'B';
                else if (piece == W_KNIGHT) fen += 'N';
                else fen += 'P';
            }
        }
    }
    if (no_piece_count != 0) {
        fen += no_piece_count + '0';
        no_piece_count = 0;
    }

    if (this->turn) {
        fen += " w ";
    } else {
        fen += " b ";
    }

    if (this->castling == 0) {
        fen += "-";
    } else {
        if (this->castling & (1 << WKSC)) fen += "K";
        if (this->castling & (1 << WQSC)) fen += "Q";
        if (this->castling & (1 << BKSC)) fen += "k";
        if (this->castling & (1 << BQSC)) fen += "q";
    }
    fen += " ";

    if (this->en_passant == NONE) {
        fen += "- ";
    } else {
        fen += squareName[this->en_passant] + " ";
    }

    fen += this->halfmove + '0';
    fen += " ";
    fen += this->fullmove + '0';

    return fen;
}

/**
 * Gets the start and end squares of a move. Also sets the promotion moves.
 * @param move_string: The move string.
 * @param move: Unsigned 16-bit int representing the move.
 * @param start: Pointer to unsigned int for start square.
 * @param end: Pointer to unsigned int for end square.
 */
void Position::getSquares(std::string move_string, Move& move, uint& start, uint& end) {
    int start_file, start_rank, end_file, end_rank;
    start_file = move_string[0] - 'a';
    start_rank = move_string[1] - '1';
    end_file = move_string[2] - 'a';
    end_rank = move_string[3] - '1';
    if (move_string.length() == 5) {
        move |= PROMOTION;
        if (move_string[4] == 'q') {
            move |= pQUEEN;
        } else if (move_string[4] == 'r') {
            move |= pROOK;
        } else if (move_string[4] == 'b') {
            move |= pBISHOP;
        }
    }

    start = 8 * start_rank + start_file;
    end = 8 * end_rank + end_file;
}

/**
 * Checks and sets castling and en passant moves, if any.
 * @param game: Pointer to the game struct.
 * @param start: The start square.
 * @param end: The end square.
 * @param move: Pointer to the integer representing the move.
 */
void Position::checkCastlingEnPassantMoves(uint start, uint end, Move& move) {
    // Check for castling move.
    if (this->turn && this->piece_list[WHITE][0] == E1) {
        if (start == E1 && end == G1 && (this->castling & (1 << WKSC))) {
            move |= CASTLING;
        } else if (start == E1 && end == C1 && (this->castling & (1 << WQSC))) {
            move |= CASTLING;
        }
    } else if (!this->turn && this->piece_list[BLACK][0] == E8) {
        if (start == E8 && end == G8 && (this->castling & (1 << BKSC))) {
            move |= CASTLING;
        } else if (start == E8 && end == C8 && (this->castling & (1 << BQSC))) {
            move |= CASTLING;
        }
    }

    // Check for en-passant move.
    if ((this->turn && start / 8 == 4) || (!this->turn && start / 8 == 3)) {
        PieceType piece = this->pieces[start];
        if ((piece == W_PAWN || piece == B_PAWN) && end == this->en_passant) {
            move |= EN_PASSANT;
        }
    }
}

Position::Position(std::string fen) : history(MAX_MOVES) {
    this->parseFen(fen);
    this->initialiseHash();
}

/**
 * Concatenates vector of strings with space.
 */
std::string concatFEN(std::vector<std::string> strings) {
    std::string result = "";
    for (int i = 2; i < (int) strings.size(); i++) {
        result += strings[i] + " ";
    }
    return result;
}

MoveList::MoveList(Position& position) {
    position.getMoves(moves_index, moveSets);
}

uint64_t MoveList::size() {
    uint64_t count = 0;
    for (int i = 0; i < moves_index; i++) {
        count += moveSets[i]->size();
    }
    return count;
}

MoveList::Iterator::Iterator(int vecCnt, int i, int j, MoveSet* moves, const Move* endMove) : endAddr{endMove} {
    // End iterator
    if (vecCnt <= 0) {
        this->ptr = this->endAddr;
        return;
    }

    this->ptr = &((*moves[i])[j]);
    this->vecCnt = vecCnt;
    this->i = i;
    this->j = j;
    this->moves = moves;
}

const Move& MoveList::Iterator::operator*() const {
    return *ptr;
}

const Move* MoveList::Iterator::operator->() const {
    return ptr;
}

MoveList::Iterator& MoveList::Iterator::operator++() { // Prefix increment
    this->j++;
    if (this->j >= (int) this->moves[i]->size()) {
        this->i++;
        this->j = 0;
    }

    if (this->i != this->vecCnt) {
        this->ptr = &(*this->moves[this->i])[this->j];
    } else { // Point to end of iterator
        this->ptr = this->endAddr;
    }
    return *this;
}

MoveList::Iterator MoveList::Iterator::operator++(int) { // Postfix increment
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

MoveList::Iterator MoveList::begin() {
    return MoveList::Iterator(moves_index, 0, 0, moveSets, endMove);
}

MoveList::Iterator MoveList::end() {
    return MoveList::Iterator(-1, -1, -1, moveSets, endMove);
}

std::array<std::array<Hash, 64>, 12> generatePieceHashes() {
    std::default_random_engine generator(42); // Fixed seed
    std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);
    std::array<std::array<Hash, 64>, 12> hashes;
    for (int piece = 0; piece < 12; piece++) {
        for (int square = A1; square <= H8; square++) {
            hashes[piece][square] = distribution(generator);
        }
    }
    return hashes;
}

Hash generateTurnHash() {
    std::default_random_engine generator(314); // Fixed seed
    std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);
    return distribution(generator);
}

std::array<Hash, 16> generateCastlingHash() {
    std::default_random_engine generator(272); // Fixed seed
    std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);
    std::array<Hash, 16> hashes;
    for (int i = 0; i < 16; i++) {
        hashes[i] = distribution(generator);
    }
    return hashes;
}

std::array<Hash, 8> generateEnPassantHash() {
    std::default_random_engine generator(162); // Fixed seed
    std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);
    std::array<Hash, 8> hashes;
    for (int i = 0; i < 8; i++) {
        hashes[i] = distribution(generator);
    }
    return hashes;
}