#ifdef USE_PEXT
#include <x86intrin.h>
#endif

#include <iostream>
#include <bitset>
#include <random>
#include <cassert>

#include "game.hpp"
#include "utils.hpp"
#include "bitboard.hpp"

namespace Moves {
    const MoveFamilies KING = computeKingMoves();
    const MoveFamilies ROOK = computeRookMoves();
    const MoveFamilies BISHOP = computeBishopMoves();
    const MoveFamilies KNIGHT = computeKnightMoves();
    const std::array<MoveFamilies, PLAYER_COUNT> PAWN = computePawnMoves();
    const std::array<MoveVector, CASTLING_OPTIONS> CASTLING = computeCastlingMoves();
    const std::array<std::vector<MoveFamily>, PLAYER_COUNT> EN_PASSANT = computeEnPassantMoves();
}

namespace Moves::Blocks {
    const MoveFamilies ROOK = computeRookBlockMoves();
    const MoveFamilies BISHOP = computeBishopBlockMoves();
}

namespace Indices {
    const IndicesFamily ROOK = computeRookReachIndices();
    const IndicesFamily BISHOP = computeBishopReachIndices();
}

namespace Reach {
    const BitboardFamily ROOK = computeRookReaches();
    const BitboardFamily BISHOP = computeBishopReaches();
}

namespace KingReach {
    const std::array<std::vector<std::vector<Square>>, SQUARE_COUNT> SQUARES = computeKingReachSquares();
}

namespace Rays {
    const BitboardFamily LEVEL = computeLevelRays();
    const BitboardFamily DIAGONAL = computeDiagonalRays();
}

namespace Hashes {
    std::array<std::array<Hash, SQUARE_COUNT>, PIECE_TYPE_COUNT> PIECES = generatePieceHashes();
    Hash TURN = generateTurnHash();
    std::array<Hash, CASTLING_COMBOS> CASTLING = generateCastlingHash();
    std::array<Hash, FILE_COUNT> EN_PASSANT = generateEnPassantHash();
}

template<>
inline PieceType Position::getPieceType<KING>(bool enemy) const {
    return ((turn == WHITE) != enemy) ? W_KING : B_KING;
}

template<>
inline PieceType Position::getPieceType<QUEEN>(bool enemy) const {
    return ((turn == WHITE) != enemy) ? W_QUEEN : B_QUEEN;
}

template<>
inline PieceType Position::getPieceType<ROOK>(bool enemy) const {
    return ((turn == WHITE) != enemy) ? W_ROOK : B_ROOK;
}

template<>
inline PieceType Position::getPieceType<BISHOP>(bool enemy) const {
    return ((turn == WHITE) != enemy) ? W_BISHOP : B_BISHOP;
}

template<>
inline PieceType Position::getPieceType<KNIGHT>(bool enemy) const {
    return ((turn == WHITE) != enemy) ? W_KNIGHT : B_KNIGHT;
}

template<>
inline PieceType Position::getPieceType<PAWN>(bool enemy) const {
    return ((turn == WHITE) != enemy) ? W_PAWN : B_PAWN;
}

template <PieceType T>
void Position::movePiece(const Square start, const Square end) {
    for (uint i = 0; i < piece_index[T]; i++) { // TODO check if in-built function to change and see if faster
        if (piece_list[T][i] == start) {
            piece_list[T][i] = end;
            break;
        }
    }
    pieces[start] = NO_PIECE;
    pieces[end] = T;
}

template <PieceType T>
void Position::removePiece(const Square square) {
    for (uint i = 0; i < piece_index[T]; i++) { // TODO check if in-built function to change and see if faster
        if (piece_list[T][i] == square) {
            piece_list[T][i] = piece_list[T][--piece_index[T]];
            break;
        }
    }
    pieces[square] = NO_PIECE;
}

template <PieceType T>
void Position::addPiece(const Square square) {
    piece_list[T][piece_index[T]++] = square;
    pieces[square] = T;
}

template <>
void Position::makePromotionMove<PromoMoveType::CAPTURE>(const Move move) {
    // Update bitboards
    sides[turn] ^= ONE_BB << start(move) | ONE_BB << end(move);
    sides[!turn] ^= ONE_BB << end(move);
    pawns ^= ONE_BB << start(move);
    removeFromBitboardPromotion(move);
    addToPromotionBitboard(move);

    // Update piece_list and pieces
    turn == WHITE ? removePiece<W_PAWN>(start(move)) : removePiece<B_PAWN>(start(move));
    removePiecePromotion(move);
    addPiecePromotion(move);
}

template <>
void Position::makePromotionMove<PromoMoveType::NON_CAPTURE>(const Move move) {
    // Update bitboards
    sides[turn] ^= ONE_BB << start(move) | ONE_BB << end(move);
    pawns ^= ONE_BB << start(move);
    addToPromotionBitboard(move);

    // Update piece_list and pieces
    turn == WHITE ? removePiece<W_PAWN>(start(move)) : removePiece<B_PAWN>(start(move));
    addPiecePromotion(move);
}

template<>
void Position::makeMove<NORMAL>(const Move move) {
    PieceType piece_captured = pieces[end(move)];
    PieceType piece_moved = pieces[start(move)];

    // Remove en-passant and its hash
    if (en_passant != NONE) {
        hash ^= Hashes::EN_PASSANT[file(en_passant)];
        en_passant = NONE;
    }

    clearBit(sides[turn], start(move));
    setBit(sides[turn], end(move));
    clearBit(sides[!turn], end(move));
    pieces[start(move)] = NO_PIECE;
    hash ^= Hashes::PIECES[piece_moved][start(move)];
    pieces[end(move)] = piece_moved;
    if (piece_captured != NO_PIECE) {
        hash ^= Hashes::PIECES[piece_captured][end(move)];
    }
    hash ^= Hashes::PIECES[piece_moved][end(move)];

    updateCastlingPermissionsAndHash(move);

    PieceType moved = piece_moved;
    switch (moved) {
        case W_KING:
        case B_KING:
            makeKingMoves(move, piece_captured);
            break;
        case W_QUEEN:
        case B_QUEEN:
            makeQueenMoves(move, piece_captured);
            break;
        case W_ROOK:
        case B_ROOK:
            makeRookMoves(move, piece_captured);
            break;
        case W_BISHOP:
        case B_BISHOP:
            makeBishopMoves(move, piece_captured);
            break;
        case W_KNIGHT:
        case B_KNIGHT:
            makeKnightMoves(move, piece_captured);
            break;
        default: // W_PAWN or B_PAWN
            makePawnMoves(move, piece_captured);
            break;
    }

    turn = !turn;
    if (turn == BLACK) {
        fullmove++;
        hash ^= Hashes::TURN;
    }
    if (moved == W_PAWN || moved == B_PAWN || piece_captured != NO_PIECE) {
        halfmove = 0;
    } else {
        halfmove++;
    }
}

template<>
void Position::makeMove<PROMOTION>(const Move move) {
    if (pieces[end(move)] != NO_PIECE) {
        makePromotionMove<PromoMoveType::CAPTURE>(move);
    } else {
        makePromotionMove<PromoMoveType::NON_CAPTURE>(move);
    }

    // Update turn
    turn = !turn;

    // Update castling and hash
    if (isSet(castling, WKSC) && end(move) == H1) {
        hash ^= Hashes::CASTLING[castling];
        castling &= ~(1 << WKSC);
        hash ^= Hashes::CASTLING[castling];
    } else if (isSet(castling, WQSC) && end(move) == A1) {
        hash ^= Hashes::CASTLING[castling];
        castling &= ~(1 << WQSC);
        hash ^= Hashes::CASTLING[castling];
    } else if (isSet(castling, BKSC) && end(move) == H8) {
        hash ^= Hashes::CASTLING[castling];
        castling &= ~(1 << BKSC);
        hash ^= Hashes::CASTLING[castling];
    } else if (isSet(castling, BQSC) && end(move) == A8) {
        hash ^= Hashes::CASTLING[castling];
        castling &= ~(1 << BQSC);
        hash ^= Hashes::CASTLING[castling];
    }

    // Update en-passant and hash
    if (en_passant != NONE) {
        hash ^= Hashes::EN_PASSANT[file(en_passant)];
        en_passant = NONE;
    }

    // Update fullmove and halfmove
    if (turn == BLACK) fullmove++;
    halfmove = 0;

    // Update remaining hash
    hash ^= Hashes::PIECES[turn == WHITE ? W_PAWN : B_PAWN][start(move)]; // TODO do hashes on the fly instead
}

template<>
void Position::makeMove<EN_PASSANT>(const Move move) {
    Square takenPawnSquare = en_passant + (turn == WHITE ? S : N);

    // Update bitboards
    sides[turn] ^= (ONE_BB << start(move) | ONE_BB << end(move));
    sides[!turn] ^= ONE_BB << takenPawnSquare;
    pawns ^= (ONE_BB << start(move) | ONE_BB << end(move) | ONE_BB << takenPawnSquare);

    // Update piece positions
    turn == WHITE ? movePiece<W_PAWN>(start(move), end(move)) : movePiece<B_PAWN>(start(move), end(move));
    turn == WHITE ? removePiece<B_PAWN>(takenPawnSquare) : removePiece<W_PAWN>(takenPawnSquare);
    hash ^= Hashes::PIECES[getPieceType<PAWN>()][start(move)] ^ Hashes::PIECES[getPieceType<PAWN>()][end(move)] ^
            Hashes::PIECES[takenPawnSquare][takenPawnSquare];

    // Update turn
    turn = !turn;

    // Update en-passant and hash
    hash ^= Hashes::EN_PASSANT[file(en_passant)];
    en_passant = NONE;

    // Update fullmove and halfmove
    if (turn == BLACK) fullmove++;
    halfmove = 0;
}

template<>
void Position::makeMove<CASTLING>(const Move move) {
    switch (end(move)) {
        case G1:
            assert(start(move) == E1);
            assert(end(move) == G1);
            makeCastlingMove<E1, G1, H1, F1, W_KING, W_ROOK>();
            break;
        case G8:
            assert(start(move) == E8);
            assert(end(move) == G8);
            makeCastlingMove<E8, G8, H8, F8, B_KING, B_ROOK>();
            break;
        case C1:
            assert(start(move) == E1);
            assert(end(move) == C1);
            makeCastlingMove<E1, C1, A1, D1, W_KING, W_ROOK>();
            break;
        default: // C8
            assert(start(move) == E8);
            assert(end(move) == C8);
            makeCastlingMove<E8, C8, A8, D8, B_KING, B_ROOK>();
            break;
    }
}

template <Square KS, Square KE, Square RS, Square RE, PieceType K, PieceType R>
void Position::makeCastlingMove() { // TODO Is it faster to combine makeCastlingMove and makeMove<CASTLING> as one?
    // Update bitboards
    sides[turn] ^= (ONE_BB << KS | ONE_BB << KE | ONE_BB << RS | ONE_BB << RE);
    kings ^= (ONE_BB << KS | ONE_BB << KE);
    rooks ^= (ONE_BB << RS | ONE_BB << RE);

    // Update piece_list and pieces
    piece_list[turn][KING_INDEX] = KE;
    for (uint i = 0; i < piece_index[R]; i++) { // TODO check if in-built function to change and see if faster
        if (piece_list[R][i] == RS) {
            piece_list[R][i] = RE;
            break;
        }
    }
    pieces[KS] = NO_PIECE;
    pieces[KE] = K;
    pieces[RS] = NO_PIECE;
    pieces[RE] = R;

    // Update turn
    turn = !turn;

    // Update castling and hash
    hash ^= Hashes::CASTLING[castling];
    castling &= (KS == E1 ? BLACK_CASTLING : WHITE_CASTLING);
    hash ^= Hashes::CASTLING[castling];

    // Update en-passant and hash
    if (en_passant != NONE) {
        hash ^= Hashes::EN_PASSANT[file(en_passant)];
        en_passant = NONE;
    }

    // Update fullmove and halfmove
    if (turn == BLACK) fullmove++;
    halfmove++;

    // Update remaining hash
    hash ^= (Hashes::PIECES[K][KS] ^ Hashes::PIECES[K][KE] ^ Hashes::PIECES[R][RS] ^ Hashes::PIECES[R][RE]);
    if (turn == BLACK) hash ^= Hashes::TURN;

    assert(pieces[KS] == NO_PIECE);
    assert(pieces[KE] == K);
    assert(pieces[RS] == NO_PIECE);
    assert(pieces[RE] == R);
}

PieceType Position::promoPiece(const Move move) {
    switch (promo(move)) {
        case pQUEEN:
            return turn == WHITE ? W_QUEEN : B_QUEEN;
        case pROOK:
            return turn == WHITE ? W_ROOK : B_ROOK;
        case pBISHOP:
            return turn == WHITE ? W_BISHOP : B_BISHOP;
        default: // pKNIGHT
            return turn == WHITE ? W_KNIGHT : B_KNIGHT;
    }
}

void Position::addToPromotionBitboard(const Move move) {
    switch (promo(move)) {
        case pQUEEN:
            queens ^= ONE_BB << end(move);
            break;
        case pROOK:
            rooks ^= ONE_BB << end(move);
            break;
        case pBISHOP:
            bishops ^= ONE_BB << end(move);
            break;
        default: // pKNIGHT
            knights ^= ONE_BB << end(move);
            break;
    }
}

void Position::removeFromBitboardPromotion(const Move move) {
    switch (pieces[end(move)]) {
        case W_QUEEN:
        case B_QUEEN:
            queens ^= ONE_BB << end(move);
            break;
        case W_ROOK:
        case B_ROOK:
            rooks ^= ONE_BB << end(move);
            break;
        case W_BISHOP:
        case B_BISHOP:
            bishops ^= ONE_BB << end(move);
            break;
        default: // W_KNIGHT or B_KNIGHT
            knights ^= ONE_BB << end(move);
            break;
    }
}

void Position::addPiecePromotion(const Move move) {
    switch (promo(move)) {
        case pQUEEN:
            turn == WHITE ? addPiece<W_QUEEN>(end(move)) : addPiece<B_QUEEN>(end(move));
            break;
        case pROOK:
            turn == WHITE ? addPiece<W_ROOK>(end(move)) : addPiece<B_ROOK>(end(move));
            break;
        case pBISHOP:
            turn == WHITE ? addPiece<W_BISHOP>(end(move)) : addPiece<B_BISHOP>(end(move));
            break;
        case pKNIGHT: // pKNIGHT
            turn == WHITE ? addPiece<W_KNIGHT>(end(move)) : addPiece<B_KNIGHT>(end(move));
            break;
        default:
            assert(false);
    }
}

void Position::removePiecePromotion(const Move move) {
    switch (pieces[end(move)]) {
        case W_QUEEN:
            removePiece<W_QUEEN>(end(move));
            break;
        case B_QUEEN:
            removePiece<B_QUEEN>(end(move));
            break;
        case W_ROOK:
            removePiece<W_ROOK>(end(move));
            break;
        case B_ROOK:
            removePiece<B_ROOK>(end(move));
            break;
        case W_BISHOP:
            removePiece<W_BISHOP>(end(move));
            break;
        case B_BISHOP:
            removePiece<B_BISHOP>(end(move));
            break;
        case W_KNIGHT:
            removePiece<W_KNIGHT>(end(move));
            break;
        case B_KNIGHT:
            removePiece<B_KNIGHT>(end(move));
            break;
        default:
            assert(false);
    }
}

void Position::updateCastlingPermissionsAndHash(Move move) {
    if (start(move) == E1 && WHITE_CASTLING & castling) {
        hash ^= Hashes::CASTLING[castling];
        castling &= BLACK_CASTLING;
        hash ^= Hashes::CASTLING[castling];
    }

    if (start(move) == E8 && BLACK_CASTLING & castling) {
        hash ^= Hashes::CASTLING[castling];
        castling &= WHITE_CASTLING;
        hash ^= Hashes::CASTLING[castling];
    }

    if ((start(move) == H1 || end(move) == H1) && isSet(castling, WKSC)) {
        hash ^= Hashes::CASTLING[castling];
        castling &= ~(1 << WKSC);
        hash ^= Hashes::CASTLING[castling];
    }

    if ((start(move) == A1 || end(move) == A1) && isSet(castling, WQSC)) {
        hash ^= Hashes::CASTLING[castling];
        castling &= ~(1 << WQSC);
        hash ^= Hashes::CASTLING[castling];
    }

    if ((start(move) == H8 || end(move) == H8) && isSet(castling, BKSC)) {
        hash ^= Hashes::CASTLING[castling];
        castling &= ~(1 << BKSC);
        hash ^= Hashes::CASTLING[castling];
    }

    if ((start(move) == A8 || end(move) == A8) && isSet(castling, BQSC)) {
        hash ^= Hashes::CASTLING[castling];
        castling &= ~(1 << BQSC);
        hash ^= Hashes::CASTLING[castling];
    }
}

template<> void Position::undoMove<NORMAL>() {
    decrementHash(hash);

    // Change game history
    History previous_pos = this->history.back();
    this->history.pop_back();

    // Retrieve last move information
    Move move = previous_pos.move;
    PieceType moved = this->pieces[end(move)];
    PieceType captured = previous_pos.captured;

    // Change turn
    this->turn = !turn;
    bool turn = this->turn;

    // Change sides bitboards
    this->sides[turn] &= ~(ONE_BB << end(move));
    this->sides[turn] |= ONE_BB << start(move);
    if (captured != NO_PIECE) this->sides[!turn] |= ONE_BB << end(move);

    // Change pieces bitboards, piece list and indices and piece counts
    if (moved == B_PAWN || moved == W_PAWN) {
        this->pawns &= ~(ONE_BB << end(move));
        this->findAndRemovePiece(moved, end(move));
        this->pawns |= ONE_BB << start(move);
        this->addPiece(moved, start(move));
    } else if (moved == B_KNIGHT || moved == W_KNIGHT) {
        this->knights &= ~(ONE_BB << end(move));
        this->findAndRemovePiece(moved, end(move));
        this->knights |= ONE_BB << start(move);
        this->addPiece(moved, start(move));
    } else if (moved == B_BISHOP || moved == W_BISHOP) {
        this->bishops &= ~(ONE_BB << end(move));
        this->findAndRemovePiece(moved, end(move));
        this->bishops |= ONE_BB << start(move);
        this->addPiece(moved, start(move));
    } else if (moved == B_ROOK || moved == W_ROOK) {
        this->rooks &= ~(ONE_BB << end(move));
        this->findAndRemovePiece(moved, end(move));
        this->rooks |= ONE_BB << start(move);
        this->addPiece(moved, start(move));
    } else if (moved == B_QUEEN || moved == W_QUEEN) {
        this->queens &= ~(ONE_BB << end(move));
        this->findAndRemovePiece(moved, end(move));
        this->queens |= ONE_BB << start(move);
        this->addPiece(moved, start(move));
    } else {
        this->kings &= ~(ONE_BB << end(move));
        this->findAndRemovePiece(moved, end(move));
        this->kings |= ONE_BB << start(move);
        this->addPiece(moved, start(move));
    }

    if (captured == NO_PIECE) {
        /* Do nothing */
    } else if (captured == B_PAWN || captured == W_PAWN) {
        this->pawns |= ONE_BB << end(move);
        this->addPiece(captured, end(move));
    } else if (captured == B_KNIGHT || captured == W_KNIGHT) {
        this->knights |= ONE_BB << end(move);
        this->addPiece(captured, end(move));
    } else if (captured == B_BISHOP || captured == W_BISHOP) {
        this->bishops |= ONE_BB << end(move);
        addPiece(captured, end(move));
    } else if (captured == B_ROOK || captured == W_ROOK) {
        this->rooks |= ONE_BB << end(move);
        this->addPiece(captured, end(move));
    } else {
        this->queens |= ONE_BB << end(move);
        this->addPiece(captured, end(move));
    }

    // Undo fullmove and History struct information
    if (turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update pieces array
    this->pieces[start(move)] = moved;
    this->pieces[end(move)] = captured;
}

template<> void Position::undoMove<PROMOTION>() {
    decrementHash(hash);

    // Change game history
    History previous_pos = this->history.back();
    this->history.pop_back();

    // Change turn
    this->turn = !turn;
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    PieceType promoted = this->pieces[end(move)];
    PieceType captured = previous_pos.captured;

    // Remove promoted piece
    this->findAndRemovePiece(promoted, end(move));
    this->sides[turn] &= ~(ONE_BB << end(move));
    this->pieces[end(move)] = NO_PIECE;
    if (promoted == W_QUEEN || promoted == B_QUEEN) {
        this->queens &= ~(ONE_BB << end(move));
    } else if (promoted == W_ROOK || promoted == B_ROOK) {
        this->rooks &= ~(ONE_BB << end(move));
    } else if (promoted == W_BISHOP || promoted == B_BISHOP) {
        this->bishops &= ~(ONE_BB << end(move));
    } else { // Knight
        this->knights &= ~(ONE_BB << end(move));
    }

    // Replace captured piece (if any)
    if (captured != NO_PIECE) {
        this->addPiece(captured, end(move));
        this->sides[!turn] |= ONE_BB << end(move);
        this->pieces[end(move)] = captured;
        if (captured == W_QUEEN || captured == B_QUEEN) {
            this->queens |= ONE_BB << end(move);
        } else if (captured == W_ROOK || captured == B_ROOK) {
            this->rooks |= ONE_BB << end(move);
        } else if (captured == W_BISHOP || captured == B_BISHOP) {
            this->bishops |= ONE_BB << end(move);
        } else { // Knight
            this->knights |= ONE_BB << end(move);
        }
    }

    // Replace pawn
    this->addPiece(getPieceType<PAWN>(), start(move));
    this->sides[turn] |= ONE_BB << start(move);
    this->pawns |= ONE_BB << start(move);
    this->pieces[start(move)] = getPieceType<PAWN>();

    // Undo fullmove and History struct information
    if (turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;
}

template<> void Position::undoMove<EN_PASSANT>() {
    decrementHash(hash);

    // Change game history
    History previous_pos = this->history.back();
    this->history.pop_back();

    // Change turn
    this->turn = (Player)(1 - this->turn);
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    PieceType moved = this->pieces[end(move)];
    Square captured_sq = end(move) + (turn ? S : N);

    // Change sides bitboards
    this->sides[turn] &= ~(ONE_BB << end(move));
    this->sides[turn] |= ONE_BB << start(move);
    this->sides[!turn] |= ONE_BB << captured_sq;

    // Change pawn bitboards
    this->pawns &= ~(ONE_BB << end(move));
    this->pawns |= ONE_BB << start(move);
    this->pawns |= ONE_BB << captured_sq;

    // Undo fullmove and History struct information
    if (turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update piece list, indices and counts
    this->findAndRemovePiece(moved, end(move));
    this->addPiece(moved, start(move));
    this->addPiece(getPieceType<PAWN>(true), captured_sq);

    // Update pieces array
    this->pieces[start(move)] = moved;
    this->pieces[end(move)] = NO_PIECE;
    this->pieces[captured_sq] = getPieceType<PAWN>(true);
}

template<> void Position::undoMove<CASTLING>() {
    decrementHash(hash);

    // Change game history
    History previous_pos = this->history.back();
    this->history.pop_back();

    // Change turn
    this->turn = (Player)(1 - this->turn);
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    PieceType moved = this->pieces[end(move)];

    // Rook changes
    Square rook_start, rook_end;
    if (end(move) == G1) {
        rook_start = H1;
        rook_end = F1;
    } else if (end(move) == C1) {
        rook_start = A1;
        rook_end = D1;
    } else if (end(move) == G8) {
        rook_start = H8;
        rook_end = F8;
    } else {
        rook_start = A8;
        rook_end = D8;
    }

    // Change sides bitboards
    this->sides[turn] &= ~(ONE_BB << end(move));
    this->sides[turn] |= ONE_BB << start(move);
    this->sides[turn] &= ~(ONE_BB << rook_end);
    this->sides[turn] |= ONE_BB << rook_start;

    // Change king bitboards
    this->kings &= ~(ONE_BB << end(move));
    this->kings |= ONE_BB << start(move);

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
    this->findAndRemovePiece(moved, end(move));
    this->addPiece(moved, start(move));
    this->findAndRemovePiece(getPieceType<ROOK>(), rook_end);
    this->addPiece(getPieceType<ROOK>(), rook_start);

    // Update pieces array
    this->pieces[start(move)] = moved;
    this->pieces[end(move)] = NO_PIECE;
    this->pieces[rook_start] = getPieceType<ROOK>();
    this->pieces[rook_end] = NO_PIECE;
}


inline PieceType Position::getPromotionPiece(Move& move) {
    switch (promo(move)) {
        case pQUEEN:
            return (turn == WHITE) ? W_QUEEN : B_QUEEN;
        case pROOK:
            return (turn == WHITE) ? W_ROOK : B_ROOK;
        case pBISHOP:
            return (turn == WHITE) ? W_BISHOP : B_BISHOP;
        default: // pKNIGHT
            return (turn == WHITE) ? W_KNIGHT : B_KNIGHT;
    }
}

inline void Position::setCheckers() {
    checkers = isAttacked(getKingSquare(), !turn);
}

inline bool Position::isPawnPinnedByRookHorizontally(const Square square) const {
    return (rank(getKingSquare()) == rank(square)) && isPinnedByRook(square);
}

inline bool Position::isPawnPinnedByRookVertically(const Square square) const {
    return (file(getKingSquare()) == file(square)) && isPinnedByRook(square);
}

void Position::setPinAndCheckRayBitboards() {
    rook_pins = ZERO_BB;
    bishop_pins = ZERO_BB;
    rook_ep_pins = ZERO_BB;
    check_rays = checkers;

    // Enemy queen rays
    for (uint i = 0; i < piece_index[getPieceType<QUEEN>(true)]; i++) {
        Bitboard ray = Rays::DIAGONAL[getKingSquare()][piece_list[getPieceType<QUEEN>(true)][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) bishop_pins |= ray;
        if (oneBitSet(ray & getPieces())) check_rays |= ray;

        ray = Rays::LEVEL[getKingSquare()][piece_list[getPieceType<QUEEN>(true)][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) rook_pins |= ray;
        if (oneBitSet(ray & getPieces())) check_rays |= ray;
        if (rank(getKingSquare()) == rank(piece_list[getPieceType<QUEEN>(true)][i])) rook_ep_pins |= ray;
    }

    // Enemy rooks
    for (uint i = 0; i < piece_index[getPieceType<ROOK>(true)]; i++) {
        Bitboard ray = Rays::LEVEL[getKingSquare()][piece_list[getPieceType<ROOK>(true)][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) rook_pins |= ray;
        if (oneBitSet(ray & getPieces())) check_rays |= ray;
        if (rank(getKingSquare()) == rank(piece_list[getPieceType<ROOK>(true)][i])) rook_ep_pins |= ray;
    }

    // Enemy bishops
    for (uint i = 0; i < piece_index[getPieceType<BISHOP>(true)]; i++) {
        Bitboard ray = Rays::DIAGONAL[getKingSquare()][piece_list[getPieceType<BISHOP>(true)][i]];
        if (oneBitSet(ray & sides[turn]) && oneBitSet(ray & sides[!turn])) bishop_pins |= ray;
        if (oneBitSet(ray & getPieces())) check_rays |= ray;
    }
}

void Position::getMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) {
    setCheckers();
    if (inCheck()) {
        if (inDoubleCheck()) {
            getKingMoves(moves_index, pos_moves);
            return;
        }
        setPinAndCheckRayBitboards();
        getCheckMoves(moves_index, pos_moves);
        return;
    }
    setPinAndCheckRayBitboards();
    getNormalMoves(moves_index, pos_moves);
}

void Position::parseFen(std::string fen) {
    // Zero out variables.
    this->resetPosition();

    // Split the string into parts and ranks
    std::vector<std::string> parts = split(fen, " ");
    std::vector<std::string> ranks = split(parts[FEN_BOARD_INDEX], "/");

    // Set the pieces
    Rank rank = RANK_8;
    for (std::string rank_string : ranks) {
        File file = FILE_A;

        for (char c : rank_string) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                for (int i = 0; i < (c - '0'); i++) {
                    pieces[RANK_COUNT * rank + file] = NO_PIECE;
                    file++;
                }
            } else {
                switch (c) {
                    case 'K':
                        updatePieceInfo(kings, W_KING, static_cast<Square>(RANK_COUNT * rank + file));
                        break;
                    case 'Q':
                        updatePieceInfo(queens, W_QUEEN, static_cast<Square>(RANK_COUNT * rank + file));
                        break;
                    case 'R':
                        updatePieceInfo(rooks, W_ROOK, static_cast<Square>(RANK_COUNT * rank + file));
                        break;
                    case 'B':
                        updatePieceInfo(bishops, W_BISHOP, static_cast<Square>(RANK_COUNT * rank + file));
                        if (isDark(static_cast<Square>(RANK_COUNT * rank + file))) {
                            this->wdsb_cnt++;
                        } else {
                            this->wlsb_cnt++;
                        }
                        break;
                    case 'N':
                        updatePieceInfo(knights, W_KNIGHT, static_cast<Square>(RANK_COUNT * rank + file));
                        this->knight_cnt++;
                        break;
                    case 'P':
                        updatePieceInfo(pawns, W_PAWN, static_cast<Square>(RANK_COUNT * rank + file));
                        break;
                    case 'k':
                        updatePieceInfo(kings, B_KING, static_cast<Square>(RANK_COUNT * rank + file));
                        break;
                    case 'q':
                        updatePieceInfo(queens, B_QUEEN, static_cast<Square>(RANK_COUNT * rank + file));
                        break;
                    case 'r':
                        updatePieceInfo(rooks, B_ROOK, static_cast<Square>(RANK_COUNT * rank + file));
                        break;
                    case 'b':
                        updatePieceInfo(bishops, B_BISHOP, static_cast<Square>(RANK_COUNT * rank + file));
                        if (isDark(static_cast<Square>(RANK_COUNT * rank + file))) {
                            this->bdsb_cnt++;
                        } else {
                            this->blsb_cnt++;
                        }
                        break;
                    case 'n':
                        updatePieceInfo(knights, B_KNIGHT, static_cast<Square>(RANK_COUNT * rank + file));
                        this->knight_cnt++;
                        break;
                    case 'p':
                        updatePieceInfo(pawns, B_PAWN, static_cast<Square>(RANK_COUNT * rank + file));
                        break;
                }

                // Set sides and piece count
                if (std::isupper(static_cast<unsigned char>(c))) {
                    this->sides[WHITE] |= ONE_BB << (RANK_COUNT * rank + file);
                } else {
                    this->sides[BLACK] |= ONE_BB << (RANK_COUNT * rank + file);
                }
                this->piece_cnt++;

                file++;
            }
        }

        rank--;
    }

    parseFenMove(parts[FEN_MOVE_INDEX]);
    parseFenCastling(parts[FEN_CASTLING_INDEX]);
    parseFenEnPassant(parts[FEN_EN_PASSANT_INDEX]);
    parseFenMoves(parts[FEN_HALFMOVE_INDEX], parts[FEN_FULLMOVE_INDEX]);

    this->hash = ZERO_BB;
}

void Position::parseFenMove(std::string& fenMove) {
    if (fenMove == "w") {
        turn = WHITE;
    } else {
        turn = BLACK;
    }
}

void Position::parseFenCastling(std::string& fenCastling) {
    castling = 0;
    if (fenCastling != "-") {
        for (char c : fenCastling) {
            if (c == 'K') {
                castling |= 1 << WKSC;
            } else if (c == 'Q') {
                castling |= 1 << WQSC;
            } else if (c == 'k') {
                castling |= 1 << BKSC;
            } else {
                castling |= 1 << BQSC;
            }
        }
    }
}

void Position::parseFenEnPassant(std::string& fenEnPassant) {
    if (fenEnPassant != "-") {
        int value = -1;
        for (char c : fenEnPassant) {
            if (value == -1) {
                value = (c - 'a');
            } else {
                value += 8 * (c - '1');
            }
        }
        en_passant = (Square) value;
    } else {
        en_passant = NONE;
    }
}

void Position::parseFenMoves(std::string& halfmove, std::string& fullmove) {
    halfmove = std::stoi(halfmove);
    fullmove = std::stoi(fullmove);
}

void Position::updatePieceInfo(Bitboard& pieceBB, PieceType piece, Square square) {
    pieceBB |= ONE_BB << square;
    piece_list[piece][piece_index[piece]] = square;
    piece_index[piece]++;
    pieces[square] = piece;
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
    // last_move = 0;
    // PieceType piece_moved = NO_PIECE;
    // piece_captured = NO_PIECE;
    // last_move_type = NORMAL;
    history.clear();
    positionCounts.clear();
    hash = ZERO_BB;
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

void Position::getKingMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    Bitboard reachBB = Masks::KING[getKingSquare()] & ~sides[turn];

    if (reachBB == ZERO_BB) return;
    for (Square square : KingReach::SQUARES[getKingSquare()][getKingMovesIndex(reachBB, getKingSquare())]) {
        if (isAttacked(square, (Player) !turn, true)) {
            reachBB &= ~(ONE_BB << square);
        }
    }

    if (reachBB == ZERO_BB) return;
    pos_moves[moves_index++] = &Moves::KING[getKingSquare()][getKingMovesIndex(reachBB, getKingSquare())];
}

void Position::getCheckMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    getKingMoves(moves_index, pos_moves);
    getQueenCheckedMoves(moves_index, pos_moves);
    getRookCheckedMoves(moves_index, pos_moves);
    getBishopCheckedMoves(moves_index, pos_moves);
    getKnightCheckedMoves(moves_index, pos_moves);
    getPawnCheckedMoves(moves_index, pos_moves);
    getEnPassantMoves(moves_index, pos_moves);
}

void Position::getQueenCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<QUEEN>()]; i++) {
        Square queenSquare = piece_list[getPieceType<QUEEN>()][i];
        if (!isPinned(queenSquare)) {
            Bitboard reach = getRookReachBB(Masks::ROOK[queenSquare] & getPieces(), queenSquare) & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::Blocks::ROOK[queenSquare][getRookBlockIndex(reach, queenSquare)];
            }

            reach = getBishopReachBB(Masks::BISHOP[queenSquare] & getPieces(), queenSquare) & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::Blocks::BISHOP[queenSquare][getBishopBlockIndex(reach, queenSquare)];
            }
        }
    }
}

void Position::getRookCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<ROOK>()]; i++) {
        Square rookSquare = piece_list[getPieceType<ROOK>()][i];
        if (!isPinned(rookSquare)) {
            Bitboard reach = getRookReachBB(Masks::ROOK[rookSquare] & getPieces(), rookSquare) & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::Blocks::ROOK[rookSquare][getRookBlockIndex(reach, rookSquare)];
            }
        }
    }
}

void Position::getBishopCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<BISHOP>()]; i++) {
        Square bishopSquare = piece_list[getPieceType<BISHOP>()][i];
        if (!isPinned(bishopSquare)) {
            Bitboard reach = getBishopReachBB(Masks::BISHOP[bishopSquare] & getPieces(), bishopSquare) & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::Blocks::BISHOP[bishopSquare][getBishopBlockIndex(reach, bishopSquare)];
            }
        }
    }
}

void Position::getKnightCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<KNIGHT>()]; i++) {
        Square knightSquare = piece_list[getPieceType<KNIGHT>()][i];
        if (!isPinned(knightSquare)) {
            Bitboard reach = Masks::KNIGHT[knightSquare] & check_rays;
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::KNIGHT[knightSquare][getKnightMovesIndex(reach, knightSquare)];
            }
        }
    }
}

void Position::getPawnCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<PAWN>()]; i++) {
        Square pawnSquare = piece_list[getPieceType<PAWN>()][i];
        if (!isPinned(pawnSquare)) {
            Bitboard reach = ZERO_BB;
            Square pawnPush = pawnSquare + (turn == WHITE ? N : S);
            if (!((ONE_BB << pawnPush) & getPieces())) {
                Rank startRank = turn == WHITE ? RANK_2 : RANK_7;
                Square pawnDoublePush = pawnSquare + (turn == WHITE ? NN : SS);
                if ((ONE_BB << pawnPush) & (check_rays & ~sides[!turn])) {
                    reach |= ONE_BB << pawnPush;
                } else if (rank(pawnSquare) == startRank && !isOccupied(pawnDoublePush) && (ONE_BB << pawnDoublePush) &
                        (check_rays & ~sides[!turn])) {
                    reach |= ONE_BB << pawnDoublePush;
                }
            }

            reach |= Masks::PAWN[turn][pawnSquare] & ~Masks::FILE[file(pawnSquare)] & (check_rays & sides[!turn]);
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::PAWN[turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, turn)];
            }
        }
    }
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

void Position::getNormalMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    this->getKingMoves(moves_index, pos_moves);
    this->getQueenMoves(moves_index, pos_moves);
    this->getRookMoves(moves_index, pos_moves);
    this->getBishopMoves(moves_index, pos_moves);
    this->getKnightMoves(moves_index, pos_moves);
    this->getPawnMoves(moves_index, pos_moves);
    this->getCastlingMoves(moves_index, pos_moves);
    this->getEnPassantMoves(moves_index, pos_moves);
}

void Position::getQueenMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<QUEEN>()]; i++) {
        Square queenSquare = piece_list[getPieceType<QUEEN>()][i];
        if (isPinnedByBishop(queenSquare)) {
            getBishopPinMoves(moves_index, pos_moves, queenSquare);
        } else if (isPinnedByRook(queenSquare)) {
            getRookPinMoves(moves_index, pos_moves, queenSquare);
        } else {
            Bitboard reach = getBishopReachBB(Masks::BISHOP[queenSquare] & getPieces(), queenSquare) & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::BISHOP[queenSquare][getBishopMovesIndex(reach, queenSquare)];
            }

            reach = getRookReachBB(Masks::ROOK[queenSquare] & getPieces(), queenSquare) & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::ROOK[queenSquare][getRookMovesIndex(reach, queenSquare)];
            }
        }
    }
}

void Position::getRookMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<ROOK>()]; i++) {
        Square rookSquare = piece_list[getPieceType<ROOK>()][i];
        if (isPinnedByBishop(rookSquare)) {
            continue;
        } else if (isPinnedByRook(rookSquare)) {
            getRookPinMoves(moves_index, pos_moves, rookSquare);
        } else {
            Bitboard reach = getRookReachBB(Masks::ROOK[rookSquare] & getPieces(), rookSquare) & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::ROOK[rookSquare][getRookMovesIndex(reach, rookSquare)];
            }
        }
    }
}

void Position::getBishopMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<BISHOP>()]; i++) {
        Square bishopSquare = piece_list[getPieceType<BISHOP>()][i];
        if (isPinnedByBishop(bishopSquare)) {
            getBishopPinMoves(moves_index, pos_moves, bishopSquare);
        } else if (isPinnedByRook(bishopSquare)) {
            continue;
        } else {
            Bitboard reach = getBishopReachBB(Masks::BISHOP[bishopSquare] & getPieces(), bishopSquare) & ~sides[turn];
            if (reach != ZERO_BB) {
                pos_moves[moves_index++] = &Moves::BISHOP[bishopSquare][getBishopMovesIndex(reach, bishopSquare)];
            }
        }
    }
}

void Position::getKnightMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<KNIGHT>()]; i++) {
        Square knightSquare = piece_list[getPieceType<KNIGHT>()][i];
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

void Position::getPawnMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    for (uint i = 0; i < piece_index[getPieceType<PAWN>()]; i++) {
        Square pawnSquare = piece_list[getPieceType<PAWN>()][i];
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

            Direction advance = turn == WHITE ? N : S;
            if (!(ONE_BB << (pawnSquare + advance) & getPieces())) {
                Rank startRank = turn == WHITE ? RANK_2 : RANK_7;
                reach |= ONE_BB << (pawnSquare + advance);
                if (rank(pawnSquare) == startRank && !(ONE_BB << (pawnSquare + advance + advance) & getPieces())) {
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
    Bitboard reach = bishop_pins & ~(ONE_BB << square) & getBishopReachBB(Masks::BISHOP[square] & getPieces(), square);
    if (reach == ZERO_BB) return;
    pos_moves[moves_index++] = &Moves::BISHOP[square][getBishopMovesIndex(reach, square)];
}

void Position::getRookPinMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE], Square square) const {
    Bitboard reach = rook_pins & ~(ONE_BB << square) & getRookReachBB(Masks::ROOK[square] & getPieces(), square);
    if (reach == ZERO_BB) return;
    pos_moves[moves_index++] = &Moves::ROOK[square][getRookMovesIndex(reach, square)];
}

Bitboard Position::isOccupied(const Square square) const {
    return getPieces() & (ONE_BB << square);
}

void Position::getCastlingMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    if (turn) {
        if (castling & (1 << WKSC)) {
            if (!isOccupied(F1) && !isOccupied(G1) && !isAttacked(F1, !turn) && !isAttacked(G1, !turn)) {
                pos_moves[moves_index++] = &Moves::CASTLING[WKSC];
            }
        }

        if (castling & (1 << WQSC)) {
            if (!isOccupied(D1) && !isOccupied(C1) && !isOccupied(B1) && !isAttacked(D1, !turn) &&
                    !isAttacked(C1, !turn)) {
                pos_moves[moves_index++] = &Moves::CASTLING[WQSC];
            }
        }
    } else {
        if (castling & (1 << BKSC)) {
            if (!isOccupied(F8) && !isOccupied(G8) && !isAttacked(F8, !turn) && !isAttacked(G8, !turn)) {
                pos_moves[moves_index++] = &Moves::CASTLING[BKSC];
            }
        }

        if (castling & (1 << BQSC)) {
            if (!isOccupied(C8) && !isOccupied(B8) && !isOccupied(D8) && !isAttacked(D8, !turn) &&
                    !isAttacked(C8, !turn)) {
                pos_moves[moves_index++] = &Moves::CASTLING[BQSC];
            }
        }
    }
}

void Position::getEnPassantMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
    if (en_passant) {
        std::vector<Square> pawnSquares;
        if (file(en_passant) != FILE_A) pawnSquares.push_back(en_passant + (turn == WHITE ? SW : NW));
        if (file(en_passant) != FILE_H) pawnSquares.push_back(en_passant + (turn == WHITE ? SE : NE));
        for (Square square : pawnSquares) {
            if ((pieces[square] == getPieceType<PAWN>()) && (!isPinnedByBishop(square) || (isPinnedByBishop(square) && isPinnedByBishop(en_passant))) && !isPinnedByRook(square)) {
                bool pinned = oneBitSet(getPieces() & rook_ep_pins & ~(ONE_BB << square | ONE_BB << (en_passant +
                        (turn == WHITE ? S : N))));
                if (!pinned) pos_moves[moves_index++] = &Moves::EN_PASSANT[turn][file(en_passant)]
                        [file(square) < file(en_passant) ? 0 : 1];
            }
        }
    }
}

inline bool Position::inDoubleCheck() {
    #ifdef USE_PEXT
    return _blsr_u64(checkers) != ZERO_BB;
    #else
    return (checkers & (checkers - ONE_BB)) != ZERO_BB;
    #endif
}

bool Position::isThreeFoldRep() {
    auto val = this->positionCounts.find(this->hash);
    return val != this->positionCounts.end() && val->second >= 3;
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

Bitboard Position::isAttacked(const Square square, const Player player, const bool ignoreKing) const {
    Bitboard pieces = getPieces() ^ (ignoreKing ? ONE_BB << piece_list[!player][0] : ZERO_BB);
    Bitboard kingBB = Masks::KING[square] & this->sides[player] & this->kings;
    Bitboard rooksBB = getRookReachBB(Masks::ROOK[square] & pieces, square) & sides[player] & (queens | rooks);
    Bitboard bishopsBB = getBishopReachBB(Masks::BISHOP[square] & pieces, square) & sides[player] & (queens | bishops);
    Bitboard knightsBB = Masks::KNIGHT[square] & sides[player] & knights;
    Bitboard pawnsBB = Masks::PAWN[!player][square] & ~Masks::FILE[file(square)] & sides[player] & pawns;
    return kingBB | rooksBB | bishopsBB | knightsBB | pawnsBB;
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

void Position::removePiece(const Move move, const PieceType piece_captured) {
    if (piece_captured == NO_PIECE) return;
    if (piece_captured == W_QUEEN || piece_captured == B_QUEEN) {
        queens &= ~(ONE_BB << end(move));
        findAndRemovePiece(piece_captured, end(move));
    } else if (piece_captured == W_ROOK || piece_captured == B_ROOK) {
        rooks &= ~(ONE_BB << end(move));
        findAndRemovePiece(piece_captured, end(move));
    } else if (piece_captured == W_BISHOP || piece_captured == B_BISHOP) {
        bishops &= ~(ONE_BB << end(move));
        findAndRemovePiece(piece_captured, end(move));
    } else if (piece_captured == W_KNIGHT || piece_captured == B_KNIGHT) {
        knights &= ~(ONE_BB << end(move));
        findAndRemovePiece(piece_captured, end(move));
    } else if (piece_captured == W_PAWN || piece_captured == B_PAWN) {
        pawns &= ~(ONE_BB << end(move));
        findAndRemovePiece(piece_captured, end(move));
    }
}

void Position::handleCastle(const Move move) {
    Square startSquare, endSquare;
    PieceType rook;

    if (end(move) == G1) {
        startSquare = H1;
        endSquare = F1;
        rook = W_ROOK;
    } else if (end(move) == C1) {
        startSquare = A1;
        endSquare = D1;
        rook = W_ROOK;
    } else if (end(move) == G8) {
        startSquare = H8;
        endSquare = F8;
        rook = B_ROOK;
    } else {
        startSquare = A8;
        endSquare = D8;
        rook = B_ROOK;
    }

    sides[turn] &= ~(ONE_BB << startSquare);
    sides[turn] |= ONE_BB << endSquare;
    pieces[startSquare] = NO_PIECE;
    hash ^= Hashes::PIECES[rook][startSquare];
    pieces[endSquare] = rook;
    hash ^= Hashes::PIECES[rook][endSquare];

    findAndRemovePiece(rook, startSquare);
    addPiece(rook, endSquare);
    rooks &= ~(ONE_BB << startSquare);
    rooks |= ONE_BB << endSquare;
}

void Position::makeKingMoves(const Move move, const PieceType piece_captured) {
    removePiece(move, piece_captured);
    piece_list[turn][KING_INDEX] = end(move);

    // Remove castling rights
    if (turn) {
        castling &= ~(1U << WKSC | 1U << WQSC);
    } else {
        castling &= ~(1U << BKSC | 1U << BQSC);
    }

    if (type(move) == CASTLING) {
        handleCastle(move);
    }

    // Update bitboards.
    kings &= ~(ONE_BB << start(move));
    kings |= ONE_BB << end(move);
}

void Position::makeQueenMoves(const Move move, const PieceType piece_captured) {
    this->removePiece(move, piece_captured);
    this->findAndRemovePiece(getPieceType<QUEEN>(), start(move));
    this->addPiece(getPieceType<QUEEN>(), end(move));
    this->queens &= ~(ONE_BB << start(move));
    this->queens |= ONE_BB << end(move);
}

void Position::makeRookMoves(const Move move, const PieceType piece_captured) {
    this->removePiece(move, piece_captured);
    this->findAndRemovePiece(getPieceType<ROOK>(), start(move));
    this->addPiece(getPieceType<ROOK>(), end(move));
    this->rooks &= ~(ONE_BB << start(move));
    this->rooks |= ONE_BB << end(move);
}

void Position::makeBishopMoves(const Move move, const PieceType piece_captured) {
    this->removePiece(move, piece_captured);
    this->findAndRemovePiece(getPieceType<BISHOP>(), start(move));
    this->addPiece(getPieceType<BISHOP>(), end(move));
    this->bishops &= ~(ONE_BB << start(move));
    this->bishops |= ONE_BB << end(move);
}

void Position::makeKnightMoves(const Move move, const PieceType piece_captured) {
    this->removePiece(move, piece_captured);
    this->findAndRemovePiece(getPieceType<KNIGHT>(), start(move));
    this->addPiece(getPieceType<KNIGHT>(), end(move));
    this->knights &= ~(ONE_BB << start(move));
    this->knights |= ONE_BB << end(move);
}

void Position::makePawnMoves(const Move move, const PieceType piece_captured) {
    this->removePiece(move, piece_captured);
    bool pawn = false;
    Square ep = (Square) (end(move) + (this->turn ? -8 : 8));
    PieceType piece = this->turn ? W_PAWN : B_PAWN;
    this->findAndRemovePiece(piece, start(move));

    if (type(move) == PROMOTION) {
        if (this->turn) {
            piece = promo(move) == pQUEEN ? W_QUEEN : promo(move) == pROOK ? W_ROOK : promo(move) == pBISHOP ? W_BISHOP : W_KNIGHT;
        } else {
            piece = promo(move) == pQUEEN ? B_QUEEN : promo(move) == pROOK ? B_ROOK : promo(move) == pBISHOP ? B_BISHOP : B_KNIGHT;
        }

        if (promo(move) == pQUEEN) {
            this->queens |= ONE_BB << end(move);
            PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
            this->addPiece(queen, end(move));
        } else if (promo(move) == pROOK) {
            this->rooks |= ONE_BB << end(move);
            PieceType ROOK = this->turn ? W_ROOK : B_ROOK;
            this->addPiece(ROOK, end(move));
        } else if (promo(move) == pBISHOP) {
            this->bishops |= ONE_BB << end(move);
            PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
            this->addPiece(bishop, end(move));
        } else {
            this->knights |= ONE_BB << end(move);
            PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
            this->addPiece(knight, end(move));
        }
        this->pieces[end(move)] = piece;
        this->hash ^= Hashes::PIECES[piece][end(move)];
    } else if (type(move) == EN_PASSANT) {
        pawn = true;
        this->addPiece(piece, end(move));
        piece = this->turn ? B_PAWN : W_PAWN;
        this->findAndRemovePiece(piece, ep);
        this->pawns &= ~(ONE_BB << ep);
        this->sides[1 - this->turn] &= ~(ONE_BB << ep);
        this->pieces[ep] = NO_PIECE;
        this->hash ^= Hashes::PIECES[piece][ep];
    } else {
        pawn = true;
        this->addPiece(piece, end(move));
        int rank_diff = rank(end(move)) - rank(start(move));
        if (std::abs(rank_diff) != 1) {
            this->en_passant = (Square) ((start(move) + 8 * (rank_diff - (rank_diff > 0 ? 1 : -1))));
            this->hash ^= Hashes::EN_PASSANT[file(en_passant)];
        }
    }

    this->pawns &= ~(ONE_BB << start(move));
    this->pawns |= (uint64_t)pawn << end(move);
}

void Position::saveHistory(const Move move) {
    history.push_back({
        castling,
        en_passant,
        halfmove,
        move,
        pieces[end(move)],
        hash,
    });
}

void Position::decrementHash(const Bitboard hash) {
    auto it = positionCounts.find(hash);
    if (it != positionCounts.end()) positionCounts[hash]--;
}

void Position::processUndoMove() {
    switch (type(history.back().move)) {
        case NORMAL:
            undoMove<NORMAL>();
            break;
        case CASTLING:
            undoMove<CASTLING>();
            break;
        case EN_PASSANT:
            undoMove<EN_PASSANT>();
            break;
        default:
            undoMove<PROMOTION>();
            break;
    }
}

void Position::processMakeMove(const Move move, const bool hash) {
    saveHistory(move);
    switch (type(move)) {
        case NORMAL:
            makeMove<NORMAL>(move);
            break;
        case CASTLING:
            makeMove<CASTLING>(move);
            break;
        case EN_PASSANT:
            makeMove<EN_PASSANT>(move);
            break;
        default:
            makeMove<PROMOTION>(move);
            break;
    }
    if (hash) incrementHash(move);
}

void Position::incrementHash(Move move) {
    auto record = this->positionCounts.find(this->hash);
    if (record != this->positionCounts.end()) {
        this->positionCounts[this->hash]++;
    } else {
        this->positionCounts.insert(std::pair<Bitboard, int>(this->hash, 1));
    }
}

Position::Position(std::string fen) {
    parseFen(fen);
    initialiseHash();
}

inline std::string darkSquare(std::string str) {
    return "\033[1;30;44m" + str + " \033[0m";
}

inline std::string lightSquare(std::string str) {
    return "\033[1;30;47m" + str + " \033[0m";
}

void Position::display() {
    bool letterMode = false;

    // Print the pieces
    for (int rank = 7; rank >= 0; rank--) {
        std::string rank_string = "";
        rank_string += (rank - 7) + '8';
        rank_string += " ";
        for (int file = 0; file < 8; file++) {
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
    displayBB(sides[WHITE]);
    displayBB(sides[BLACK]);
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

Move MoveList::randomMove() {
    uint vecIndex = std::rand() % (moves_index);
    uint movesIndex = std::rand() % (moveSets[vecIndex]->size());
    return (*moveSets[vecIndex])[movesIndex];
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

std::array<std::array<Hash, SQUARE_COUNT>, PIECE_TYPE_COUNT> generatePieceHashes() {
    std::default_random_engine generator(42); // Fixed random seed
    std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
    std::array<std::array<Hash, SQUARE_COUNT>, PIECE_TYPE_COUNT> hashes;
    for (uint piece = 0; piece < PIECE_TYPE_COUNT; piece++) {
        for (Square square = A1; square <= H8; square++) {
            hashes[piece][square] = distribution(generator);
        }
    }
    return hashes;
}

Hash generateTurnHash() {
    std::default_random_engine generator(314); // Fixed random seed
    std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
    return distribution(generator);
}

std::array<Hash, 16> generateCastlingHash() {
    std::default_random_engine generator(272); // Fixed random seed
    std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
    std::array<Hash, CASTLING_COMBOS> hashes;
    for (uint i = 0; i < CASTLING_COMBOS; i++) {
        hashes[i] = distribution(generator);
    }
    return hashes;
}

std::array<Hash, 8> generateEnPassantHash() {
    std::default_random_engine generator(162); // Fixed random seed
    std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
    std::array<Hash, FILE_COUNT> hashes;
    for (uint i = 0; i < FILE_COUNT; i++) {
        hashes[i] = distribution(generator);
    }
    return hashes;
}