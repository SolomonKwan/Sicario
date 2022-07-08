
#include <iostream>
#include <bitset>
#include <random>

#include "game.hpp"
#include "utils.hpp"
#include "bitboard.hpp"

namespace Moves {
    const std::array<std::vector<std::vector<Move>>, SQUARE_COUNT> KING = computeKingMoves();
    const std::array<std::vector<std::vector<Move>>, SQUARE_COUNT> ROOK = computeRookMoves();
    const std::array<std::vector<std::vector<Move>>, SQUARE_COUNT> BISHOP = computeBishopMoves();
    const std::array<std::vector<std::vector<Move>>, SQUARE_COUNT> KNIGHT = computeKnightMoves();
    const std::array<std::array<std::vector<std::vector<Move>>, SQUARE_COUNT>, PLAYER_COUNT> PAWN = computePawnMoves();
    const std::array<std::vector<Move>, CASTLING_OPTIONS> CASTLING = computeCastlingMoves();
    const std::array<std::vector<std::vector<std::vector<Move>>>, PLAYER_COUNT> EN_PASSANT = computeEnPassantMoves();
}

namespace Moves::Blocks {
    const std::array<std::vector<std::vector<Move>>, SQUARE_COUNT> ROOK = computeRookBlockMoves();
    const std::array<std::vector<std::vector<Move>>, SQUARE_COUNT> BISHOP = computeBishopBlockMoves();
}

namespace Indices {
    const std::array<std::vector<uint>, SQUARE_COUNT> ROOK = computeRookReachIndices();
    const std::array<std::vector<uint>, SQUARE_COUNT> BISHOP = computeBishopReachIndices();
}

namespace Reach {
    const std::array<std::vector<Bitboard>, SQUARE_COUNT> ROOK = computeRookReaches();
    const std::array<std::vector<Bitboard>, SQUARE_COUNT> BISHOP = computeBishopReaches();
}

namespace KingReach {
    const std::array<std::vector<std::vector<Square>>, SQUARE_COUNT> SQUARES = computeKingReachSquares();
}

namespace Rays {
    const std::array<std::vector<Bitboard>, SQUARE_COUNT> LEVEL = computeLevelRays();
    const std::array<std::vector<Bitboard>, SQUARE_COUNT> DIAGONAL = computeDiagonalRays();
}

namespace Hashes {
    std::array<std::array<Hash, SQUARE_COUNT>, PIECE_TYPE_COUNT> PIECES = generatePieceHashes();
    Hash TURN = generateTurnHash();
    std::array<Hash, CASTLING_COMBOS> CASTLING = generateCastlingHash();
    std::array<Hash, FILE_COUNT> EN_PASSANT = generateEnPassantHash();
}

void Position::setCheckers() {
    checkers = isAttacked(getKingSquare(), !turn);
}

bool Position::isPawnPinnedByRookHorizontally(const Square square) {
    return (rank(getKingSquare()) == rank(square)) && isPinnedByRook(square);
}

bool Position::isPawnPinnedByRookVertically(const Square square) {
    return (file(getKingSquare()) == file(square)) && isPinnedByRook(square);
}

void Position::setPinAndCheckRayBitboards() {
    Square king_sq = getKingSquare();
    Bitboard pieces = sides[WHITE] | sides[BLACK];
    rook_pins = ZERO_BB;
    bishop_pins = ZERO_BB;
    rook_ep_pins = ZERO_BB;
    check_rays = checkers;

    // Enemy queen rays
    PieceType eQueen = turn == WHITE ? B_QUEEN : W_QUEEN;
    for (uint i = 0; i < piece_index[eQueen]; i++) {
        Bitboard ray = Rays::DIAGONAL[king_sq][piece_list[eQueen][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) bishop_pins |= ray;
        if (oneBitSet(ray & pieces)) check_rays |= ray;

        ray = Rays::LEVEL[king_sq][piece_list[eQueen][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) rook_pins |= ray;
        if (oneBitSet(ray & pieces)) check_rays |= ray;
        if (rank(king_sq) == rank(piece_list[eQueen][i])) rook_ep_pins |= ray;
    }

    // Enemy rooks
    PieceType eRook = turn == WHITE ? B_ROOK : W_ROOK;
    for (uint i = 0; i < piece_index[eRook]; i++) {
        Bitboard ray = Rays::LEVEL[king_sq][piece_list[eRook][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) rook_pins |= ray;
        if (oneBitSet(ray & pieces)) check_rays |= ray;
        if (rank(king_sq) == rank(piece_list[eRook][i])) rook_ep_pins |= ray;
    }

    // Enemy bishops
    PieceType eBishop = turn == WHITE ? B_BISHOP : W_BISHOP;
    for (uint i = 0; i < piece_index[eBishop]; i++) {
        Bitboard ray = Rays::DIAGONAL[king_sq][piece_list[eBishop][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) bishop_pins |= ray;
        if (oneBitSet(ray & pieces)) check_rays |= ray;
    }
}

void Position::getMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
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

void Position::parseFen(std::string fen) {
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
                    this->kings |= ONE_BB << (8 * rank + file);
                    this->piece_list[WHITE][0] = (Square) (8 * rank + file);
                    this->piece_index[W_KING]++;
                    this->pieces[8 * rank + file] = W_KING;
                } else if (c == 'Q') {
                    this->queens |= ONE_BB << (8 * rank + file);
                    this->piece_list[W_QUEEN][this->piece_index[W_QUEEN]] = (Square)(8 * rank + file);
                    this->piece_index[W_QUEEN]++;
                    this->pieces[8 * rank + file] = W_QUEEN;
                } else if (c == 'R') {
                    this->rooks |= ONE_BB << (8 * rank + file);
                    this->piece_list[W_ROOK][this->piece_index[W_ROOK]] = (Square)(8 * rank + file);
                    this->piece_index[W_ROOK]++;
                    this->pieces[8 * rank + file] = W_ROOK;
                } else if (c == 'B') {
                    this->bishops |= ONE_BB << (8 * rank + file);
                    this->piece_list[W_BISHOP][this->piece_index[W_BISHOP]] = (Square)(8 * rank + file);
                    this->piece_index[W_BISHOP]++;
                    this->pieces[8 * rank + file] = W_BISHOP;
                    if (isDark(8 * rank + file)) {
                        this->wdsb_cnt++;
                    } else {
                        this->wlsb_cnt++;
                    }
                } else if (c == 'N') {
                    this->knights |= ONE_BB << (8 * rank + file);
                    this->piece_list[W_KNIGHT][this->piece_index[W_KNIGHT]] = (Square)(8 * rank + file);
                    this->piece_index[W_KNIGHT]++;
                    this->pieces[8 * rank + file] = W_KNIGHT;
                    this->knight_cnt++;
                } else if (c == 'P') {
                    this->pawns |= ONE_BB << (8 * rank + file);
                    this->piece_list[W_PAWN][this->piece_index[W_PAWN]] = (Square)(8 * rank + file);
                    this->piece_index[W_PAWN]++;
                    this->pieces[8 * rank + file] = W_PAWN;
                } else if (c == 'k') {
                    this->kings |= ONE_BB << (8 * rank + file);
                    this->piece_list[BLACK][0] = (Square) (8 * rank + file);
                    this->piece_index[B_KING]++;
                    this->pieces[8 * rank + file] = B_KING;
                } else if (c == 'q') {
                    this->queens |= ONE_BB << (8 * rank + file);
                    this->piece_list[B_QUEEN][this->piece_index[B_QUEEN]] = (Square)(8 * rank + file);
                    this->piece_index[B_QUEEN]++;
                    this->pieces[8 * rank + file] = B_QUEEN;
                } else if (c == 'r') {
                    this->rooks |= ONE_BB << (8 * rank + file);
                    this->piece_list[B_ROOK][this->piece_index[B_ROOK]] = (Square)(8 * rank + file);
                    this->piece_index[B_ROOK]++;
                    this->pieces[8 * rank + file] = B_ROOK;
                } else if (c == 'b') {
                    this->bishops |= ONE_BB << (8 * rank + file);
                    this->piece_list[B_BISHOP][this->piece_index[B_BISHOP]] = (Square)(8 * rank + file);
                    this->piece_index[B_BISHOP]++;
                    this->pieces[8 * rank + file] = B_BISHOP;
                    if (isDark(8 * rank + file)) {
                        this->bdsb_cnt++;
                    } else {
                        this->blsb_cnt++;
                    }
                } else if (c == 'n') {
                    this->knights |= ONE_BB << (8 * rank + file);
                    this->piece_list[B_KNIGHT][this->piece_index[B_KNIGHT]] = (Square)(8 * rank + file);
                    this->piece_index[B_KNIGHT]++;
                    this->pieces[8 * rank + file] = B_KNIGHT;
                    this->knight_cnt++;
                } else {
                    this->pawns |= ONE_BB << (8 * rank + file);
                    this->piece_list[B_PAWN][this->piece_index[B_PAWN]] = (Square)(8 * rank + file);
                    this->piece_index[B_PAWN]++;
                    this->pieces[8 * rank + file] = B_PAWN;
                }

                // Set sides and piece count
                if (std::isupper(static_cast<unsigned char>(c))) {
                    this->sides[WHITE] |= ONE_BB << (8 * rank + file);
                } else {
                    this->sides[BLACK] |= ONE_BB << (8 * rank + file);
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
    this->hash = ZERO_BB;
}

void Position::resetPosition() {
    // Non-positional variables
    turn = WHITE;
    castling = 0;
    en_passant = NONE;
    halfmove = 0;
    fullmove = 0;

    // Bitboards
    sides[WHITE] = ZERO_BB;
    sides[BLACK] = ZERO_BB;
    kings = ZERO_BB;
    queens = ZERO_BB;
    rooks = ZERO_BB;
    bishops = ZERO_BB;
    knights = ZERO_BB;
    pawns = ZERO_BB;
    rook_pins = ZERO_BB;
    bishop_pins = ZERO_BB;
    check_rays = ZERO_BB;
    checkers = ZERO_BB;
    rook_ep_pins = ZERO_BB;

    // Piece positions
    std::fill(std::begin(piece_index), std::end(piece_index), 0);
    std::fill(piece_list[0] + 0, piece_list[PIECE_TYPE_COUNT] + MAX_PIECE_COUNT, NONE);
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
    hash = ZERO_BB;
    ply = 0;
}

bool Position::insufficientMaterial() {
    // King vs king
    if (piece_cnt == 2) return true;

    // King and bishop(s) vs king (same colour bishop(s))
    if ((wlsb_cnt && (piece_cnt - wlsb_cnt) == 2) || (wdsb_cnt && (piece_cnt - wdsb_cnt) == 2) ||
            (blsb_cnt && (piece_cnt - blsb_cnt) == 2) || (bdsb_cnt && (piece_cnt - bdsb_cnt) == 2)) {
        return true;
    }

    // King and bishop(s) vs king and bishop(s) (same colour bishop(s))
    if ((wlsb_cnt && blsb_cnt && (piece_cnt - wlsb_cnt - blsb_cnt) == 2) || (wdsb_cnt && bdsb_cnt &&
            (piece_cnt - wdsb_cnt - bdsb_cnt) == 2)) {
        return true;
    }

    // King and knight vs king
    if (knight_cnt == 1 && piece_cnt == 3) return true;

    return false;
}

inline bool Position::inCheck() {
    return checkers;
}

void Position::getKingMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    Square kingSquare = getKingSquare();
    Bitboard reachBB = Masks::KING[kingSquare] & ~sides[turn];

    if (reachBB == ZERO_BB) return;
    for (Square square : KingReach::SQUARES[kingSquare][getKingMovesIndex(reachBB, kingSquare)]) {
        if (isAttacked(square, (Player) !turn, true)) {
            reachBB &= ~(ONE_BB << square);
        }
    }

    if (reachBB == ZERO_BB) return;
    pos_moves[moves_index++] = &Moves::KING[kingSquare][getKingMovesIndex(reachBB, kingSquare)];
}

void Position::getCheckMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    getKingMoves(moves_index, pos_moves);
    getQueenCheckedMoves(moves_index, pos_moves);
    getRookCheckedMoves(moves_index, pos_moves);
    getBishopCheckedMoves(moves_index, pos_moves);
    getKnightCheckedMoves(moves_index, pos_moves);
    getPawnCheckedMoves(moves_index, pos_moves);
    getEnPassantMoves(moves_index, pos_moves);
}

void Position::getQueenCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType queen = turn == WHITE ? W_QUEEN : B_QUEEN;
    Bitboard pieces = sides[WHITE] | sides[BLACK];

    for (uint i = 0; i < piece_index[queen]; i++) {
        Square queenSquare = piece_list[queen][i];
        if (!isPinned(queenSquare)) {
            Bitboard reach = getRookReachBB(Masks::ROOK[queenSquare] & pieces, queenSquare) & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::Blocks::ROOK[queenSquare][getRookBlockIndex(reach, queenSquare)];
            }

            reach = getBishopReachBB(Masks::BISHOP[queenSquare] & pieces, queenSquare) & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::Blocks::BISHOP[queenSquare][getBishopBlockIndex(reach, queenSquare)];
            }
        }
    }
}

void Position::getRookCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType rook = turn == WHITE ? W_ROOK : B_ROOK;
    Bitboard pieces = sides[WHITE] | sides[BLACK];

    for (uint i = 0; i < piece_index[rook]; i++) {
        Square rookSquare = piece_list[rook][i];
        if (!isPinned(rookSquare)) {
            Bitboard reach = getRookReachBB(Masks::ROOK[rookSquare] & pieces, rookSquare) & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::Blocks::ROOK[rookSquare][getRookBlockIndex(reach, rookSquare)];
            }
        }
    }
}

void Position::getBishopCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType bishop = turn == WHITE ? W_BISHOP : B_BISHOP;
    Bitboard pieces = sides[WHITE] | sides[BLACK];

    for (uint i = 0; i < piece_index[bishop]; i++) {
        Square bishopSquare = piece_list[bishop][i];
        if (!isPinned(bishopSquare)) {
            Bitboard reach = getBishopReachBB(Masks::BISHOP[bishopSquare] & pieces, bishopSquare) & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::Blocks::BISHOP[bishopSquare][getBishopBlockIndex(reach, bishopSquare)];
            }
        }
    }
}

void Position::getKnightCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType knight = turn == WHITE ? W_KNIGHT : B_KNIGHT;
    for (uint i = 0; i < piece_index[knight]; i++) {
        Square knightSquare = piece_list[knight][i];
        if (!isPinned(knightSquare)) {
            Bitboard reach = Masks::KNIGHT[knightSquare] & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::KNIGHT[knightSquare][getKnightMovesIndex(reach, knightSquare)];
            }
        }
    }
}

void Position::getPawnCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType pawn = turn == WHITE ? W_PAWN : B_PAWN;
    int advance = turn == WHITE ? N : S;
    int startRank = turn == WHITE ? 1 : 6;
    Bitboard pieces = sides[WHITE] | sides[BLACK];

    for (uint i = 0; i < piece_index[pawn]; i++) {
        Square pawnSquare = piece_list[pawn][i];
        if (!isPinned(pawnSquare)) {
            Bitboard reach = ZERO_BB;
            if (!(ONE_BB << (pawnSquare + advance) & pieces)) {
                if ((ONE_BB << (pawnSquare + advance)) & (check_rays & ~sides[!turn])) {
                    reach |= ONE_BB << (pawnSquare + advance);
                } else if (rank(pawnSquare) == startRank && !(ONE_BB << (pawnSquare + advance + advance) & pieces) && (ONE_BB << (pawnSquare + advance + advance)) & (check_rays & ~sides[!turn])) {
                    reach |= ONE_BB << (pawnSquare + advance + advance);
                }
            }

            reach |= Masks::PAWN[turn][pawnSquare] & ~Masks::FILE[file(pawnSquare)] & (check_rays & sides[!turn]);
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::PAWN[turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, turn)];
            }
        }
    }
}

inline Player Position::getTurn() {
    return turn;
}

void Position::initialiseHash() {
    hash = ZERO_BB;

    // Hash the pieces
    for (Square sq = A1; sq <= H8; sq++) {
        if (pieces[sq] != NO_PIECE) {
            hash ^= Hashes::PIECES[pieces[sq]][sq];
        }
    }

    // Hash turn
    if (turn == BLACK) hash ^= Hashes::TURN;

    // Hash the castling
    hash ^= Hashes::CASTLING[castling];

    // Hash en-passant
    if (en_passant != NONE) {
        hash ^= Hashes::EN_PASSANT[file(en_passant)];
    }
}

void Position::getNormalMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    this->getKingMoves(moves_index, pos_moves);
    this->getQueenMoves(moves_index, pos_moves);
    this->getRookMoves(moves_index, pos_moves);
    this->getBishopMoves(moves_index, pos_moves);
    this->getKnightMoves(moves_index, pos_moves);
    this->getPawnMoves(moves_index, pos_moves);
    this->getCastlingMoves(moves_index, pos_moves);
    this->getEnPassantMoves(moves_index, pos_moves);
}

void Position::getQueenMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType queen = turn == WHITE ? W_QUEEN : B_QUEEN;
    Bitboard pieces = sides[WHITE] | sides[BLACK];

    for (uint i = 0; i < piece_index[queen]; i++) {
        Square queenSquare = piece_list[queen][i];
        if (isPinnedByBishop(queenSquare)) {
            getBishopPinMoves(moves_index, pos_moves, queenSquare);
        } else if (isPinnedByRook(queenSquare)) {
            getRookPinMoves(moves_index, pos_moves, queenSquare);
        } else {
            Bitboard reach = getBishopReachBB(Masks::BISHOP[queenSquare] & pieces, queenSquare) & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::BISHOP[queenSquare][getBishopMovesIndex(reach, queenSquare)];
            }

            reach = getRookReachBB(Masks::ROOK[queenSquare] & pieces, queenSquare) & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::ROOK[queenSquare][getRookMovesIndex(reach, queenSquare)];
            }
        }
    }
}

void Position::getRookMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType rook = turn == WHITE ? W_ROOK : B_ROOK;
    Bitboard pieces = sides[WHITE] | sides[BLACK];

    for (uint i = 0; i < piece_index[rook]; i++) {
        Square rookSquare = piece_list[rook][i];
        if (isPinnedByBishop(rookSquare)) {
            continue;
        } else if (isPinnedByRook(rookSquare)) {
            getRookPinMoves(moves_index, pos_moves, rookSquare);
        } else {
            Bitboard reach = getRookReachBB(Masks::ROOK[rookSquare] & pieces, rookSquare) & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::ROOK[rookSquare][getRookMovesIndex(reach, rookSquare)];
            }
        }
    }
}

void Position::getBishopMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType bishop = turn == WHITE ? W_BISHOP : B_BISHOP;
    Bitboard pieces = sides[WHITE] | sides[BLACK];

    for (uint i = 0; i < piece_index[bishop]; i++) {
        Square bishopSquare = piece_list[bishop][i];
        if (isPinnedByBishop(bishopSquare)) {
            getBishopPinMoves(moves_index, pos_moves, bishopSquare);
        } else if (isPinnedByRook(bishopSquare)) {
            continue;
        } else {
            Bitboard reach = getBishopReachBB(Masks::BISHOP[bishopSquare] & pieces, bishopSquare) & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::BISHOP[bishopSquare][getBishopMovesIndex(reach, bishopSquare)];
            }
        }
    }
}

void Position::getKnightMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType knight = turn == WHITE ? W_KNIGHT : B_KNIGHT;
    for (uint i = 0; i < piece_index[knight]; i++) {
        Square knightSquare = piece_list[knight][i];
        if (isPinnedByBishop(knightSquare) || isPinnedByRook(knightSquare)) {
            continue;
        } else {
            Bitboard reach = Masks::KNIGHT[knightSquare] & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::KNIGHT[knightSquare][getKnightMovesIndex(reach, knightSquare)];
            }
        }
    }
}

void Position::getPawnMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    PieceType pawn = turn == WHITE ? W_PAWN : B_PAWN;
    int advance = turn == WHITE ? N : S;
    int startRank = turn == WHITE ? RANK_2 : RANK_7;
    Bitboard pieces = sides[WHITE] | sides[BLACK];

    for (uint i = 0; i < piece_index[pawn]; i++) {
        Square pawnSquare = piece_list[pawn][i];
        if (isPinnedByBishop(pawnSquare)) {
            Bitboard reach = (Masks::PAWN[turn][pawnSquare] & ~Masks::FILE[file(pawnSquare)]);
            reach &= bishop_pins & (sides[!turn] & (queens | bishops));
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::PAWN[turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, turn)];
            }
        } else if (isPawnPinnedByRookHorizontally(pawnSquare)) { // Can't move if horizontally pinned to king.
            continue;
        } else {
            Bitboard reach = ZERO_BB;

            if (!isPawnPinnedByRookVertically(pawnSquare)) { // Can't capture if vertically pinned to king.
                reach |= Masks::PAWN[turn][pawnSquare] & ~Masks::FILE[file(pawnSquare)] & sides[!turn];
            }

            if (!(ONE_BB << (pawnSquare + advance) & pieces)) {
                reach |= ONE_BB << (pawnSquare + advance);
                if (rank(pawnSquare) == startRank && !(ONE_BB << (pawnSquare + advance + advance) & pieces)) {
                    reach |= ONE_BB << (pawnSquare + advance + advance);
                }
            }

            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::PAWN[turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, turn)];
            }
        }
    }
}

inline Bitboard Position::getRookReachBB(Bitboard occupancy, Square square) const {
    return Reach::ROOK[square][Indices::ROOK[square][getRookReachIndex(occupancy, square)]];
}

inline Bitboard Position::getBishopReachBB(Bitboard occupancy, Square square) const {
    return Reach::BISHOP[square][Indices::BISHOP[square][getBishopReachIndex(occupancy, square)]];
}

void Position::getBishopPinMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE], Square square) const {
    Bitboard pieces = (sides[WHITE] | sides[BLACK]);
    Bitboard reach = bishop_pins & ~(ONE_BB << square) & getBishopReachBB(Masks::BISHOP[square] & pieces, square);
    if (reach == ZERO_BB) return;
    pos_moves[moves_index++] = &Moves::BISHOP[square][getBishopMovesIndex(reach, square)];
}

void Position::getRookPinMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE], Square square) const {
    Bitboard pieces = (sides[WHITE] | sides[BLACK]);
    Bitboard reach = rook_pins & ~(ONE_BB << square) & getRookReachBB(Masks::ROOK[square] & pieces, square);
    if (reach == ZERO_BB) return;
    pos_moves[moves_index++] = &Moves::ROOK[square][getRookMovesIndex(reach, square)];
}

Bitboard Position::isOccupied(const Square square) {
    return ((this->sides[WHITE] | this->sides[BLACK]) & (ONE_BB << square));
}

void Position::getCastlingMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    if (turn) {
        if (castling & (1 << WKSC)) {
            if (!isOccupied(F1) && !isOccupied(G1) && !isAttacked(F1, (Player)!turn) && !isAttacked(G1, (Player)!turn)) {
                pos_moves[moves_index++] = &Moves::CASTLING[WKSC];
            }
        }

        if (castling & (1 << WQSC)) {
            if (!isOccupied(D1) && !isOccupied(C1) && !isOccupied(B1) && !isAttacked(D1, (Player)!turn) &&
                    !isAttacked(C1, (Player)!turn)) {
                pos_moves[moves_index++] = &Moves::CASTLING[WQSC];
            }
        }
    } else {
        if (castling & (1 << BKSC)) {
            if (!isOccupied(F8) && !isOccupied(G8) && !isAttacked(F8, (Player)!turn) && !isAttacked(G8, (Player)!turn)) {
                pos_moves[moves_index++] = &Moves::CASTLING[BKSC];
            }
        }

        if (castling & (1 << BQSC)) {
            if (!isOccupied(C8) && !isOccupied(B8) && !isOccupied(D8) && !isAttacked(D8, (Player)!turn) &&
                    !isAttacked(C8, (Player)!turn)) {
                pos_moves[moves_index++] = &Moves::CASTLING[BQSC];
            }
        }
    }
}

void Position::getEnPassantMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    if (en_passant) {
        std::vector<Square> pawnSquares;
        if (file(en_passant) != FILE_A) pawnSquares.push_back((Square)(en_passant + (turn == WHITE ? SW : NW)));
        if (file(en_passant) != FILE_H) pawnSquares.push_back((Square)(en_passant + (turn == WHITE ? SE : NE)));
        for (Square square : pawnSquares) {
            bool pawnExists = pieces[square] == (turn == WHITE ? W_PAWN : B_PAWN);
            if (pawnExists && !isPinnedByBishop(square) && !isPinnedByRook(square)) {
                bool pinned = oneBitSet((sides[WHITE] | sides[BLACK]) & rook_ep_pins & ~(ONE_BB << square | ONE_BB << (en_passant + (turn == WHITE ? S : N))));
                if (!pinned) pos_moves[moves_index++] = &Moves::EN_PASSANT[turn][file(en_passant)][file(square) < file(en_passant) ? 0 : 1];
            }
        }
    }
}

inline bool Position::inDoubleCheck() {
    return (checkers & (checkers - ONE_BB)) != ZERO_BB;
}

bool Position::isThreeFoldRep() {
    auto val = this->hashes.find(this->hash);
    return val != this->hashes.end() && val->second >= 3;
}

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
    Bitboard pieces = (sides[WHITE] | sides[BLACK]) ^ (ignoreKing ? ONE_BB << piece_list[!player][0] : ZERO_BB);
    Bitboard kingBB = Masks::KING[square] & this->sides[player] & this->kings;
    Bitboard rooksBB = getRookReachBB(Masks::ROOK[square] & pieces, square) & sides[player] & (queens | rooks);
    Bitboard bishopsBB = getBishopReachBB(Masks::BISHOP[square] & pieces, square) & sides[player] & (queens | bishops);
    Bitboard knightsBB = Masks::KNIGHT[square] & sides[player] & knights;
    Bitboard pawnsBB = Masks::PAWN[!player][square] & ~Masks::FILE[file(square)] & sides[player] & pawns;
    return kingBB | rooksBB | bishopsBB | knightsBB | pawnsBB;
}

inline Hash Position::getHash() {
    return this->hash;
}

void Position::findAndRemovePiece(PieceType piece, Square square) {
    int taken_index = -1;
    for (uint i = 0; i < this->piece_index[piece]; i++) {
        if (piece_list[piece][i] == square) {
            taken_index = i;
            break;
        }
    }

    piece_index[piece]--;
    piece_list[piece][taken_index] = piece_list[piece][piece_index[piece]];
    if (piece == W_BISHOP) {
        if (isDark(square)) {
            wdsb_cnt--;
        } else {
            wlsb_cnt--;
        }
    } else if (piece == B_BISHOP) {
        if (isDark(square)) {
            bdsb_cnt--;
        } else {
            blsb_cnt--;
        }
    } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
        knight_cnt--;
    }
    piece_cnt--;
}

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

void Position::removePiece() {
    PieceType captured = this->piece_captured;
    if (captured == NO_PIECE) return;
    int end = (this->last_move >> 6) & 0b111111;
    if (captured == W_QUEEN || captured == B_QUEEN) {
        this->queens &= ~(ONE_BB << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_ROOK || captured == B_ROOK) {
        this->rooks &= ~(ONE_BB << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_BISHOP || captured == B_BISHOP) {
        this->bishops &= ~(ONE_BB << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_KNIGHT || captured == B_KNIGHT) {
        this->knights &= ~(ONE_BB << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_PAWN || captured == B_PAWN) {
        this->pawns &= ~(ONE_BB << end);
        this->findAndRemovePiece(captured, (Square) end);
    }
}

void Position::handleCastle() {
    int start, end;
    int last_end = (this->last_move >> 6) & 0b111111;
    PieceType rook;

    if (last_end  == G1) {
        start = H1;
        end = F1;
        rook = W_ROOK;
    } else if (last_end == C1) {
        start = A1;
        end = D1;
        rook = W_ROOK;
    } else if (last_end == G8) {
        start = H8;
        end = F8;
        rook = B_ROOK;
    } else {
        start = A8;
        end = D8;
        rook = B_ROOK;
    }

    this->sides[this->turn] &= ~(ONE_BB << start);
    this->sides[this->turn] |= ONE_BB << end;
    this->pieces[start] = NO_PIECE;
    this->hash ^= Hashes::PIECES[rook][start];
    this->pieces[end] = rook;
    this->hash ^= Hashes::PIECES[rook][end];

    this->findAndRemovePiece(rook, (Square) start);
    this->addPiece(rook, (Square) end);
    this->rooks &= ~(ONE_BB << start);
    this->rooks |= ONE_BB << end;
}

void Position::makeKingMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    this->piece_list[this->turn][0] = (Square) end;

    // Remove castling rights
    if (this->turn) {
        this->castling &= ~(1 << WKSC | 1 << WQSC);
    } else {
        this->castling &= ~(1 << BKSC | 1 << BQSC);
    }

    if (move_type == CASTLING) {
        this->handleCastle();
    }

    // Update bitboards.
    this->kings &= ~(ONE_BB << start);
    this->kings |= ONE_BB << end;
}

void Position::makeQueenMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
    this->findAndRemovePiece(queen, (Square) start);
    this->addPiece(queen, (Square) end);
    this->queens &= ~(ONE_BB << start);
    this->queens |= ONE_BB << end;
}

void Position::makeRookMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType rook = this->turn ? W_ROOK : B_ROOK;
    this->findAndRemovePiece(rook, (Square) start);
    this->addPiece(rook, (Square) end);
    this->rooks &= ~(ONE_BB << start);
    this->rooks |= ONE_BB << end;
}

void Position::makeBishopMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
    this->findAndRemovePiece(bishop, (Square) start);
    this->addPiece(bishop, (Square) end);
    this->bishops &= ~(ONE_BB << start);
    this->bishops |= ONE_BB << end;
}

void Position::makeKnightMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
    this->findAndRemovePiece(knight, (Square) start);
    this->addPiece(knight, (Square) end);
    this->knights &= ~(ONE_BB << start);
    this->knights |= ONE_BB << end;
}

void Position::makePawnMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    int promo = move & (0b11 << 14);
    bool pawn = false;
    int ep = end + (this->turn ? -8 : 8);
    PieceType piece = this->turn ? W_PAWN : B_PAWN;
    this->findAndRemovePiece(piece, (Square) start);

    if (move_type == PROMOTION) {
        if (this->turn) {
            piece = promo == pQUEEN ? W_QUEEN : promo == pROOK ? W_ROOK : promo == pBISHOP ? W_BISHOP : W_KNIGHT;
        } else {
            piece = promo == pQUEEN ? B_QUEEN : promo == pROOK ? B_ROOK : promo == pBISHOP ? B_BISHOP : B_KNIGHT;
        }

        if (promo == pQUEEN) {
            this->queens |= ONE_BB << end;
            PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
            this->addPiece(queen, (Square) end);
        } else if (promo == pROOK) {
            this->rooks |= ONE_BB << end;
            PieceType ROOK = this->turn ? W_ROOK : B_ROOK;
            this->addPiece(ROOK, (Square) end);
        } else if (promo == pBISHOP) {
            this->bishops |= ONE_BB << end;
            PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
            this->addPiece(bishop, (Square) end);
        } else {
            this->knights |= ONE_BB << end;
            PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
            this->addPiece(knight, (Square) end);
        }
        this->pieces[end] = piece;
        this->hash ^= Hashes::PIECES[piece][end];
    } else if (move_type == EN_PASSANT) {
        pawn = true;
        this->addPiece(piece, (Square) end);
        piece = this->turn ? B_PAWN : W_PAWN;
        this->findAndRemovePiece(piece, (Square) ep);
        this->pawns &= ~(ONE_BB << ep);
        this->sides[1 - this->turn] &= ~(ONE_BB << ep);
        this->pieces[ep] = NO_PIECE;
        this->hash ^= Hashes::PIECES[piece][ep];
    } else {
        pawn = true;
        this->addPiece(piece, (Square) end);
        int rank_diff = end / 8 - start / 8;
        if (std::abs(rank_diff) != 1) {
            this->en_passant = (Square) (start + 8 * (rank_diff - (rank_diff > 0 ? 1 : -1)));
            this->hash ^= Hashes::EN_PASSANT[this->en_passant % 8];
        }
    }

    this->pawns &= ~(ONE_BB << start);
    this->pawns |= (uint64_t)pawn << end;
}

void Position::saveHistory(Move move) {
    this->history[this->ply].castling = this->castling;
    this->history[this->ply].en_passant = this->en_passant;
    this->history[this->ply].halfmove = this->halfmove;
    this->history[this->ply].move = move;
    this->history[this->ply].captured = this->pieces[(move >> 6) & 0b111111];
    this->history[this->ply].hash = this->hash;
    this->ply++;

    if (this->ply >= MAX_MOVES) {
        this->history.resize(2 * MAX_MOVES);
    }
}

void Position::undoNormal() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Retrieve last move information
    Move move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];
    PieceType captured = previous_pos.captured;

    // Change turn
    this->turn = (Player)(1 - this->turn);
    bool turn = this->turn;

    // Change sides bitboards
    this->sides[turn] &= ~(ONE_BB << end);
    this->sides[turn] |= ONE_BB << start;
    if (captured != NO_PIECE) this->sides[!turn] |= ONE_BB << end;

    // Change pieces bitboards, piece list and indices and piece counts
    if (moved == B_PAWN || moved == W_PAWN) {
        this->pawns &= ~(ONE_BB << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->pawns |= ONE_BB << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_KNIGHT || moved == W_KNIGHT) {
        this->knights &= ~(ONE_BB << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->knights |= ONE_BB << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_BISHOP || moved == W_BISHOP) {
        this->bishops &= ~(ONE_BB << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->bishops |= ONE_BB << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_ROOK || moved == W_ROOK) {
        this->rooks &= ~(ONE_BB << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->rooks |= ONE_BB << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_QUEEN || moved == W_QUEEN) {
        this->queens &= ~(ONE_BB << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->queens |= ONE_BB << start;
        this->addPiece(moved, (Square) start);
    } else {
        this->kings &= ~(ONE_BB << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->kings |= ONE_BB << start;
        this->addPiece(moved, (Square) start);
    }

    if (captured == NO_PIECE) {
        /* Do nothing */
    } else if (captured == B_PAWN || captured == W_PAWN) {
        this->pawns |= ONE_BB << end;
        this->addPiece(captured, (Square) end);
    } else if (captured == B_KNIGHT || captured == W_KNIGHT) {
        this->knights |= ONE_BB << end;
        this->addPiece(captured, (Square) end);
    } else if (captured == B_BISHOP || captured == W_BISHOP) {
        this->bishops |= ONE_BB << end;
        addPiece(captured, (Square) end);
    } else if (captured == B_ROOK || captured == W_ROOK) {
        this->rooks |= ONE_BB << end;
        this->addPiece(captured, (Square) end);
    } else {
        this->queens |= ONE_BB << end;
        this->addPiece(captured, (Square) end);
    }

    // Undo fullmove and History struct information
    if (turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = captured;
}

void Position::undoPromotion() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = (Player)(1 - this->turn);
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType promoted = this->pieces[end];
    PieceType captured = previous_pos.captured;
    PieceType pawn = turn ? W_PAWN : B_PAWN;

    // Remove promoted piece
    this->findAndRemovePiece(promoted, (Square) end);
    this->sides[turn] &= ~(ONE_BB << end);
    this->pieces[end] = NO_PIECE;
    if (promoted == W_QUEEN || promoted == B_QUEEN) {
        this->queens &= ~(ONE_BB << end);
    } else if (promoted == W_ROOK || promoted == B_ROOK) {
        this->rooks &= ~(ONE_BB << end);
    } else if (promoted == W_BISHOP || promoted == B_BISHOP) {
        this->bishops &= ~(ONE_BB << end);
    } else { // Knight
        this->knights &= ~(ONE_BB << end);
    }

    // Replace captured piece (if any)
    if (captured != NO_PIECE) {
        this->addPiece(captured, (Square) end);
        this->sides[!turn] |= ONE_BB << end;
        this->pieces[end] = captured;
        if (captured == W_QUEEN || captured == B_QUEEN) {
            this->queens |= ONE_BB << end;
        } else if (captured == W_ROOK || captured == B_ROOK) {
            this->rooks |= ONE_BB << end;
        } else if (captured == W_BISHOP || captured == B_BISHOP) {
            this->bishops |= ONE_BB << end;
        } else { // Knight
            this->knights |= ONE_BB << end;
        }
    }

    // Replace pawn
    this->addPiece(pawn, (Square) start);
    this->sides[turn] |= ONE_BB << start;
    this->pawns |= ONE_BB << start;
    this->pieces[start] = pawn;

    // Undo fullmove and History struct information
    if (turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;
}

void Position::undoEnPassant() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = (Player)(1 - this->turn);
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];
    PieceType captured = turn ? B_PAWN : W_PAWN;
    int captured_sq = end + (turn ? -8 : 8);

    // Change sides bitboards
    this->sides[turn] &= ~(ONE_BB << end);
    this->sides[turn] |= ONE_BB << start;
    this->sides[!turn] |= ONE_BB << captured_sq;

    // Change pawn bitboards
    this->pawns &= ~(ONE_BB << end);
    this->pawns |= ONE_BB << start;
    this->pawns |= ONE_BB << captured_sq;

    // Undo fullmove and History struct information
    if (turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update piece list, indices and counts
    this->findAndRemovePiece(moved, (Square) end);
    this->addPiece(moved, (Square) start);
    this->addPiece(captured, (Square) captured_sq);

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = NO_PIECE;
    this->pieces[captured_sq] = captured;
}

void Position::undoCastling() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = (Player)(1 - this->turn);
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];

    // Rook changes
    int rook_start, rook_end;
    PieceType rook = turn ? W_ROOK : B_ROOK;
    if (end == G1) {
        rook_start = H1;
        rook_end = F1;
    } else if (end == C1) {
        rook_start = A1;
        rook_end = D1;
    } else if (end == G8) {
        rook_start = H8;
        rook_end = F8;
    } else {
        rook_start = A8;
        rook_end = D8;
    }

    // Change sides bitboards
    this->sides[turn] &= ~(ONE_BB << end);
    this->sides[turn] |= ONE_BB << start;
    this->sides[turn] &= ~(ONE_BB << rook_end);
    this->sides[turn] |= ONE_BB << rook_start;

    // Change king bitboards
    this->kings &= ~(ONE_BB << end);
    this->kings |= ONE_BB << start;

    // Change rook bitboards
    this->rooks &= ~(ONE_BB << rook_end);
    this->rooks |= ONE_BB << rook_start;

    // Undo fullmove and History struct information (some is not necessary i think? Done out of principle)
    if (turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update piece list, indices and counts
    this->findAndRemovePiece(moved, (Square) end);
    this->addPiece(moved, (Square) start);
    this->findAndRemovePiece(rook, (Square) rook_end);
    this->addPiece(rook, (Square) rook_start);

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = NO_PIECE;
    this->pieces[rook_start] = rook;
    this->pieces[rook_end] = NO_PIECE;
}

void Position::decrementHash(const Bitboard hash) {
    auto it = this->hashes.find(hash);
    if (it != this->hashes.end()) {
        this->hashes[hash]--;
    }
}

void Position::undoMove() {
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

void Position::makeMove(Move move) {
    // Save current position and move to make to history.
    this->saveHistory(move);

    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);

    this->last_move = move;
    this->last_move_type = (MoveType) move_type;
    this->piece_captured = this->pieces[end];
    this->piece_moved = this->pieces[start];

    // Remove en-passant and its hash
    if (this->en_passant != NONE) {
        this->hash ^= Hashes::EN_PASSANT[this->en_passant % 8];
        this->en_passant = NONE;
    }

    this->sides[this->turn] &= ~(ONE_BB << start);
    this->sides[this->turn] |= ONE_BB << end;
    this->sides[1 - this->turn] &= ~(ONE_BB << end);
    this->pieces[start] = NO_PIECE;
    this->hash ^= Hashes::PIECES[this->piece_moved][start];
    this->pieces[end] = this->piece_moved;
    if (this->piece_captured != NO_PIECE) {
        this->hash ^= Hashes::PIECES[this->piece_captured][end];
    }
    this->hash ^= Hashes::PIECES[this->piece_moved][end];

    // Change castling privileges.
    if (start == E1 && 0b11 & this->castling) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= 0b1100;
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if (start == E8 && (0b11 << 2) & this->castling) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= 0b0011;
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if ((start == H1 || end == H1) && this->castling & (1 << WKSC)) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= ~(1 << WKSC);
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if ((start == A1 || end == A1) && this->castling & (1 << WQSC)) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= ~(1 << WQSC);
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if ((start == H8 || end == H8) && this->castling & (1 << BKSC)) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= ~(1 << BKSC);
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if ((start == A8 || end == A8) && this->castling & (1 << BQSC)) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= ~(1 << BQSC);
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    PieceType moved = this->piece_moved;
    if (moved == W_KING || moved == B_KING) {
        this->makeKingMoves(move);
    } else if (moved == W_QUEEN || moved == B_QUEEN) {
        this->makeQueenMoves(move);
    } else if (moved == W_ROOK || moved == B_ROOK) {
        this->makeRookMoves(move);
    } else if (moved == W_BISHOP || moved == B_BISHOP) {
        this->makeBishopMoves(move);
    } else if (moved == W_KNIGHT || moved == B_KNIGHT) {
        this->makeKnightMoves(move);
    } else {
        this->makePawnMoves(move);
    }

    this->turn = (Player)(1 - this->turn);
    if (this->turn == BLACK) {
        this->fullmove++;
        this->hash ^= Hashes::TURN;
    }
    if (moved == W_PAWN || moved == B_PAWN || this->piece_captured != NO_PIECE) {
        this->halfmove = 0;
    } else {
        this->halfmove++;
    }

    // Increment position hash counter
    this->incrementHash(move);
}

void Position::incrementHash(Move move) {
    auto record = this->hashes.find(this->hash);
    if (record != this->hashes.end()) {
        this->hashes[this->hash]++;
    } else {
        this->hashes.insert(std::pair<Bitboard, int>(this->hash, 1));
    }
}

bool Position::validMove(Move move, MoveList& move_list) {
    for (uint i = 0; i < move_list.moves_index; i++) {
        for (Move move_candidate : *move_list.moveSets[i]) {
            if (move == move_candidate) return true;
        }
    }
    return false;
}

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

inline std::string darkSquare(std::string str) {
    return "\033[1;30;44m" + str + " \033[0m"; // TODO bold outline?
}

inline std::string lightSquare(std::string str) {
    return "\033[1;30;47m" + str + " \033[0m"; // TODO bold outline?
}

void Position::display() {
    bool letterMode = false; // TODO implement later.

    // Print the pieces
    for (uint rank = 7; rank >= 0; rank--) {
        std::string rank_string = "";
        rank_string += (rank - 7) + '8';
        rank_string += " ";
        for (uint file = 0; file < 8; file++) {
            Square square = (Square)(8 * rank + file);
            PieceType piece = this->pieces[square];
            switch (piece) {
                case W_KING:
                    rank_string += letterMode ? "K " : isDark(square) ? darkSquare("\u2654") : lightSquare("\u2654");
                    break;
                case B_KING:
                    rank_string += letterMode ? "k " : isDark(square) ? darkSquare("\u265A") : lightSquare("\u265A");
                    break;
                case W_QUEEN:
                    rank_string += letterMode ? "Q " : isDark(square) ? darkSquare("\u2655") : lightSquare("\u2655");
                    break;
                case B_QUEEN:
                    rank_string += letterMode ? "q " : isDark(square) ? darkSquare("\u265B") : lightSquare("\u265B");
                    break;
                case W_ROOK:
                    rank_string += letterMode ? "R " : isDark(square) ? darkSquare("\u2656") : lightSquare("\u2656");
                    break;
                case B_ROOK:
                    rank_string += letterMode ? "r " : isDark(square) ? darkSquare("\u265C") : lightSquare("\u265C");
                    break;
                case W_BISHOP:
                    rank_string += letterMode ? "B " : isDark(square) ? darkSquare("\u2657") : lightSquare("\u2657");
                    break;
                case B_BISHOP:
                    rank_string += letterMode ? "b " : isDark(square) ? darkSquare("\u265D") : lightSquare("\u265D");
                    break;
                case W_KNIGHT:
                    rank_string += letterMode ? "N " : isDark(square) ? darkSquare("\u2658") : lightSquare("\u2658");
                    break;
                case B_KNIGHT:
                    rank_string += letterMode ? "n " : isDark(square) ? darkSquare("\u265E") : lightSquare("\u265E");
                    break;
                case W_PAWN:
                    rank_string += letterMode ? "P " : isDark(square) ? darkSquare("\u2659") : lightSquare("\u2659");
                    break;
                case B_PAWN:
                    rank_string += letterMode ? "p " : isDark(square) ? darkSquare("\u265F") : lightSquare("\u265F");
                    break;
                default:
                    rank_string += isDark(square) ? darkSquare(" ") : lightSquare(" ");
            }
        }
        std::cout << rank_string << '\n';
    }
    std::cout << "  a b c d e f g h\n";

    // Print turn
    if (this->turn) {
        std::cout << "Turn: white\n";
    } else {
        std::cout << "Turn: black\n";
    }

    // Print castling
    std::string castling = "Castling: ";
    if (this->castling & (1 << WKSC)) castling += "K";
    if (this->castling & (1 << WQSC)) castling += "Q";
    if (this->castling & (1 << BKSC)) castling += "k";
    if (this->castling & (1 << BQSC)) castling += "q";
    if (!(this->castling & (1 << WKSC)) && !(this->castling & (1 << WQSC)) &&
            !(this->castling & (1 << BKSC)) &&
            !(this->castling & (1 << BQSC))) {
        castling += "-";
    }
    std::cout << castling << '\n';

    // Print en-passant
    std::string ep = "En-passant: ";
    if (this->en_passant == NONE) {
        ep += "-\n";
    } else {
        ep += squareName[this->en_passant] + "\n";
    }
    std::cout << ep;

    // Clock cycles
    std::cout << "Halfmove: " << this->halfmove << '\n';
    std::cout << "Fullmove: " << this->fullmove << '\n';

    // New line for spacing
    std::cout << '\n';
}

void Position::displayBitboards() {
    displayBB(sides[0]);
    displayBB(sides[1]);
    displayBB(kings);
    displayBB(queens);
    displayBB(rooks);
    displayBB(bishops);
    displayBB(knights);
    displayBB(pawns);
}

std::string concatFEN(std::vector<std::string> strings) {
    std::string result = "";
    for (uint i = 2; i < strings.size(); i++) {
        result += strings[i] + " ";
    }
    return result;
}

MoveList::MoveList(Position& position) {
    position.getMoves(moves_index, moveSets);
}

uint64_t MoveList::size() {
    uint64_t count = 0;
    for (uint i = 0; i < moves_index; i++) {
        count += moveSets[i]->size();
    }
    return count;
}

bool MoveList::contains(Move targetMove) {
    for (uint i = 0; i < moves_index; i++) {
        for (Move move : *moveSets[i]) {
            if (move == targetMove) return true;
        }
    }
    return false;
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

std::array<std::array<Hash, SQUARE_COUNT>, 12> generatePieceHashes() {
    std::default_random_engine generator(42); // Fixed seed
    std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
    std::array<std::array<Hash, SQUARE_COUNT>, 12> hashes;
    for (uint piece = 0; piece < PIECE_TYPE_COUNT; piece++) {
        for (Square square = A1; square <= H8; square++) {
            hashes[piece][square] = distribution(generator);
        }
    }
    return hashes;
}

Hash generateTurnHash() {
    std::default_random_engine generator(314); // Fixed seed
    std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
    return distribution(generator);
}

std::array<Hash, 16> generateCastlingHash() {
    std::default_random_engine generator(272); // Fixed seed
    std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
    std::array<Hash, CASTLING_COMBOS> hashes;
    for (uint i = 0; i < CASTLING_COMBOS; i++) {
        hashes[i] = distribution(generator);
    }
    return hashes;
}

std::array<Hash, 8> generateEnPassantHash() {
    std::default_random_engine generator(162); // Fixed seed
    std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
    std::array<Hash, FILE_COUNT> hashes;
    for (uint i = 0; i < FILE_COUNT; i++) {
        hashes[i] = distribution(generator);
    }
    return hashes;
}