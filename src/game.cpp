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

Position::Position(const std::string fen) {
	parseFen(fen);
}

void Position::parseFen(const std::string fen) {
	// Zero out variables.
	resetPosition();

	// Split the string into parts and ranks
	std::vector<std::string> parts = split(fen, " ");
	std::vector<std::string> ranks = split(parts[FEN_BOARD_INDEX], "/");

	// Parse the board
	std::reverse(ranks.begin(), ranks.end());
	int squareIndex = A1;
	for (std::string& rank : ranks) {
		for (char c : rank) {
			if (std::isdigit(c)) {
				squareIndex += (c - '0');
				continue;
			}
			parseFenChar(c, squareIndex);

			// Set sides and piece count
			if (std::isupper(c))
				this->sides[WHITE] |= ONE_BB << squareIndex;
			else
				this->sides[BLACK] |= ONE_BB << squareIndex;
			this->piece_cnt++;
			squareIndex++;
		}
	}

	parseFenMove(parts[FEN_MOVE_INDEX]);
	parseFenCastling(parts[FEN_CASTLING_INDEX]);
	parseFenEnPassant(parts[FEN_EN_PASSANT_INDEX]);
	parseFenMoves(parts[FEN_HALFMOVE_INDEX], parts[FEN_FULLMOVE_INDEX]);

	initialiseHash();
	incrementPositionCounter();
}

void Position::resetPosition() {
	// Non-positional variables
	this->turn = WHITE;
	this->castling = 0;
	this->en_passant = NONE;
	this->halfmove = 0;
	this->fullmove = 1;

	// Bitboards
	this->sides[WHITE] = ZERO_BB;
	this->sides[BLACK] = ZERO_BB;
	this->kings = ZERO_BB;
	this->queens = ZERO_BB;
	this->rooks = ZERO_BB;
	this->bishops = ZERO_BB;
	this->knights = ZERO_BB;
	this->pawns = ZERO_BB;
	this->rook_pins = ZERO_BB;
	this->bishop_pins = ZERO_BB;
	this->check_rays = ZERO_BB;
	this->checkers = ZERO_BB;
	this->rook_ep_pins = ZERO_BB;

	// Piece positions
	std::fill(std::begin(this->piece_index), std::end(this->piece_index), 0);
	std::fill(this->piece_list[0] + 0, this->piece_list[PIECE_TYPE_COUNT] + MAX_PIECE_COUNT, NONE);
	std::fill(std::begin(this->pieces), std::end(this->pieces), NO_PIECE);

	// Piece counts
	this->piece_cnt = 0;
	this->knight_cnt = 0;
	this->bishop_cnt = 0;
	this->light_bishop_cnt = 0;
	this->dark_bishop_cnt = 0;

	// History
	this->history.clear();
	this->positionCounts.clear();
	this->hash = ZERO_BB;
}

ExitCode Position::isEOG(MoveList& move_list) const {
	if (this->isDrawFiftyMoveRule()) return FIFTY_MOVES_RULE;
	if (this->isDrawThreeFoldRep()) return THREE_FOLD_REPETITION;
	if (this->isDrawInsufficientMaterial()) return INSUFFICIENT_MATERIAL;
	if (this->isDrawStalemate(move_list)) return STALEMATE;
	if (this->isCheckmate(move_list)) {
		if (this->turn == WHITE) return BLACK_WINS;
		return WHITE_WINS;
	}
	return NORMAL_PLY;
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

template <PieceType T>
void Position::movePiece(const Square start, const Square end) {
	for (uint i = 0; i < this->piece_index[T]; i++) {
		if (this->piece_list[T][i] != start) continue;
		this->piece_list[T][i] = end;
		break;
	}
	this->pieces[start] = NO_PIECE;
	this->pieces[end] = T;
}

// TODO Research template conditional (enable_if?)
template<>
void Position::movePiece<W_KING>(const Square start, const Square end) {
	this->piece_list[W_KING][KING_INDEX] = end;
	this->pieces[start] = NO_PIECE;
	this->pieces[end] = W_KING;
}

template<>
void Position::movePiece<B_KING>(const Square start, const Square end) {
	this->piece_list[B_KING][KING_INDEX] = end;
	this->pieces[start] = NO_PIECE;
	this->pieces[end] = B_KING;
}

template<>
inline PieceType Position::getPieceType<KING>(const bool enemy) const {
	return ((turn == WHITE) != enemy) ? W_KING : B_KING;
}

template<>
inline PieceType Position::getPieceType<QUEEN>(const bool enemy) const {
	return ((turn == WHITE) != enemy) ? W_QUEEN : B_QUEEN;
}

template<>
inline PieceType Position::getPieceType<ROOK>(const bool enemy) const {
	return ((turn == WHITE) != enemy) ? W_ROOK : B_ROOK;
}

template<>
inline PieceType Position::getPieceType<BISHOP>(const bool enemy) const {
	return ((turn == WHITE) != enemy) ? W_BISHOP : B_BISHOP;
}

template<>
inline PieceType Position::getPieceType<KNIGHT>(const bool enemy) const {
	return ((turn == WHITE) != enemy) ? W_KNIGHT : B_KNIGHT;
}

template<>
inline PieceType Position::getPieceType<PAWN>(const bool enemy) const {
	return ((turn == WHITE) != enemy) ? W_PAWN : B_PAWN;
}

template <>
void Position::makePromotionMove<CAPTURE>(const Move move) {
	// Update bitboards.
	zeroAndSetBit(this->sides[this->turn], start(move), end(move));
	zeroBit(this->sides[!this->turn], end(move));
	zeroBit(this->pawns, start(move));
	removeFromBitboardPromotion(move);
	addToPromotionBitboard(move);

	// Update piece_list and pieces.
	this->turn == WHITE ? removePiece<W_PAWN>(start(move)) : removePiece<B_PAWN>(start(move));

	// Remove piece if the promotion included a capture.
	removePiecePromotion(move);

	// Add the new piece
	addPiecePromotion(move);
}

template <>
void Position::makePromotionMove<NON_CAPTURE>(const Move move) {
	// Update bitboards
	zeroAndSetBit(this->sides[this->turn], start(move), end(move));
	zeroBit(this->pawns, start(move));
	addToPromotionBitboard(move);

	// Update piece_list and pieces
	this->turn == WHITE ? removePiece<W_PAWN>(start(move)) : removePiece<B_PAWN>(start(move));

	// Add the new piece
	addPiecePromotion(move);
}

template <>
void Position::normalMoveSidesBitboards<CAPTURE>(const Move move) {
	this->sides[this->turn] ^= ONE_BB << start(move) | ONE_BB << end(move);
	this->sides[!this->turn] ^= ONE_BB << end(move);
}

template <>
void Position::normalMoveSidesBitboards<NON_CAPTURE>(const Move move) {
	this->sides[this->turn] ^= ONE_BB << start(move) | ONE_BB << end(move);
}

template <PieceType T>
void Position::removePiece(const Square square) {
	for (uint i = 0; i < this->piece_index[T]; i++) {
		if (this->piece_list[T][i] != square) continue;
		this->piece_list[T][i] = this->piece_list[T][--this->piece_index[T]];
		break;
	}
	this->pieces[square] = NO_PIECE;
}

template <PieceType T>
void Position::addPiece(const Square square) {
	assert(T != W_KING && T != B_KING);
	assert(this->pieces[square] == NO_PIECE);
	this->piece_list[T][this->piece_index[T]++] = square;
	this->pieces[square] = T;
}

template <Square KS, Square KE, Square RS, Square RE, PieceType K, PieceType R>
void Position::makeCastlingMove() {
	// Update bitboards
	this->sides[this->turn] ^= ONE_BB << KS | ONE_BB << KE | ONE_BB << RS | ONE_BB << RE;
	this->kings ^= ONE_BB << KS | ONE_BB << KE;
	this->rooks ^= ONE_BB << RS | ONE_BB << RE;

	// Update piece_list and pieces
	movePiece<K>(KS, KE);
	movePiece<R>(RS, RE);

	// Update piece hash
	this->hash ^= Hashes::PIECES[K][KS] ^ Hashes::PIECES[K][KE] ^ Hashes::PIECES[R][RS] ^ Hashes::PIECES[R][RE];

	// Update non-position information
	updateCastling(KS, KE);
	updateEnPassant(true);
	updateFullmove();
	updateHalfmove(false);
	updateTurn();
}

template<>
void Position::movePieceBitboard<KING>(const Square start, const Square end) {
	this->kings ^= ONE_BB << start | ONE_BB << end;
}

template<>
void Position::movePieceBitboard<QUEEN>(const Square start, const Square end) {
	this->queens ^= ONE_BB << start | ONE_BB << end;
}

template<>
void Position::movePieceBitboard<ROOK>(const Square start, const Square end) {
	this->rooks ^= ONE_BB << start | ONE_BB << end;
}

template<>
void Position::movePieceBitboard<BISHOP>(const Square start, const Square end) {
	this->bishops ^= ONE_BB << start | ONE_BB << end;
}

template<>
void Position::movePieceBitboard<KNIGHT>(const Square start, const Square end) {
	this->knights ^= ONE_BB << start | ONE_BB << end;
}

template<>
void Position::movePieceBitboard<PAWN>(const Square start, const Square end) {
	bool notUndoMove = (start < end && this->turn == WHITE) || (start > end && this->turn == BLACK);
	if (std::abs(rank(end) - rank(start)) == 2 && notUndoMove) {
		this->en_passant = start + (this->turn == WHITE ? N : S);
		this->hash ^= Hashes::EN_PASSANT[file(this->en_passant)];
	}
	this->pawns ^= ONE_BB << start | ONE_BB << end;
}

template<>
void Position::addToBitboard<KING>(const Square square) {
	this->kings ^= ONE_BB << square;
}

template<>
void Position::addToBitboard<QUEEN>(const Square square) {
	this->queens ^= ONE_BB << square;
}

template<>
void Position::addToBitboard<ROOK>(const Square square) {
	this->rooks ^= ONE_BB << square;
}

template<>
void Position::addToBitboard<BISHOP>(const Square square) {
	this->bishops ^= ONE_BB << square;
}

template<>
void Position::addToBitboard<KNIGHT>(const Square square) {
	this->knights ^= ONE_BB << square;
}

template<>
void Position::addToBitboard<PAWN>(const Square square) {
	this->pawns ^= ONE_BB << square;
}

template<>
void Position::makeMove<NORMAL, NON_CAPTURE>(const Move move) {
	updateEnPassant(true);
	PieceType piece_moved = this->pieces[start(move)];

	// Update pieces, piece_list and bitboard.
	normalMoveSidesBitboards<NON_CAPTURE>(move);
	movePieceAndUpdateBitboard(piece_moved, start(move), end(move));

	// Update hash
	this->hash ^= Hashes::PIECES[piece_moved][start(move)];
	this->hash ^= Hashes::PIECES[piece_moved][end(move)];

	// Update non-position information
	updateCastling(start(move), end(move));
	updateEnPassant(false);
	updateFullmove();
	updateHalfmove(piece_moved == W_PAWN || piece_moved == B_PAWN);
	updateTurn();
}

template<>
void Position::makeMove<NORMAL, CAPTURE>(const Move move) {
	updateEnPassant(true);

	PieceType piece_captured = this->pieces[end(move)];
	PieceType piece_moved = this->pieces[start(move)];

	// Update pieces, piece_list and bitboard.
	removePiece(end(move), piece_captured);
	normalMoveSidesBitboards<CAPTURE>(move);
	updatePieceCounts(piece_captured, move);
	movePieceAndUpdateBitboard(piece_moved, start(move), end(move));

	// Update hash
	this->hash ^= Hashes::PIECES[piece_moved][start(move)];
	this->hash ^= Hashes::PIECES[piece_moved][end(move)];
	this->hash ^= Hashes::PIECES[piece_captured][end(move)];

	// Update non-position information
	updateCastling(start(move), end(move));
	updateEnPassant(false);
	updateFullmove();
	updateHalfmove(true);
	updateTurn();
}

template<>
void Position::makeMove<CASTLING, NON_CAPTURE>(const Move move) {
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

template<>
void Position::makeMove<EN_PASSANT, CAPTURE>(const Move move) {
	Square takenPawnSquare = this->en_passant + (this->turn == WHITE ? S : N);

	// Update bitboards
	this->sides[this->turn] ^= (ONE_BB << start(move) | ONE_BB << end(move));
	this->sides[!this->turn] ^= ONE_BB << takenPawnSquare;
	this->pawns ^= (ONE_BB << start(move) | ONE_BB << end(move) | ONE_BB << takenPawnSquare);

	// Update piece positions
	this->turn == WHITE ? movePiece<W_PAWN>(start(move), end(move)) : movePiece<B_PAWN>(start(move), end(move));
	this->turn == WHITE ? removePiece<B_PAWN>(takenPawnSquare) : removePiece<W_PAWN>(takenPawnSquare);

	// Update piece count
	this->piece_cnt--;

	// Update hash
	this->hash ^= Hashes::PIECES[getPieceType<PAWN>()][start(move)];
	this->hash ^= Hashes::PIECES[getPieceType<PAWN>()][end(move)];
	this->hash ^= Hashes::PIECES[takenPawnSquare][takenPawnSquare];

	// Update non-position information
	updateEnPassant(true);
	updateFullmove();
	updateHalfmove(true);
	updateTurn();
}

template<>
void Position::makeMove<PROMOTION, CAPTURE>(const Move move) {
	PieceType piece_captured = this->pieces[end(move)];
	PieceType piece_moved = this->pieces[start(move)];

	updatePieceCounts(piece_captured, move);
	makePromotionMove<CAPTURE>(move);

	// Update promotion piece hash and/or piece count
	switch (promo(move)) {
		case pQUEEN:
			this->hash ^= Hashes::PIECES[getPieceType<QUEEN>()][end(move)];
			break;
		case pROOK:
			this->hash ^= Hashes::PIECES[getPieceType<ROOK>()][end(move)];
			break;
		case pKNIGHT:
			this->knight_cnt++;
			this->hash ^= Hashes::PIECES[getPieceType<KNIGHT>()][end(move)];
			break;
		case pBISHOP:
			this->bishop_cnt++;
			isDark(end(move)) ? this->dark_bishop_cnt++ : this->light_bishop_cnt++;
			this->hash ^= Hashes::PIECES[getPieceType<BISHOP>()][end(move)];
			break;
	}

	// Update hash
	this->hash ^= Hashes::PIECES[piece_moved][start(move)];
	this->hash ^= Hashes::PIECES[piece_captured][end(move)];

	// Update non-position information
	updateCastling(start(move), end(move));
	updateEnPassant(true);
	updateFullmove();
	updateHalfmove(true);
	updateTurn();
}

template<>
void Position::makeMove<PROMOTION, NON_CAPTURE>(const Move move) {
	PieceType piece_moved = this->pieces[start(move)];
	makePromotionMove<NON_CAPTURE>(move);

	// Update promotion piece hash and/or piece count
	switch (promo(move)) {
		case pQUEEN:
			this->hash ^= Hashes::PIECES[getPieceType<QUEEN>()][end(move)];
			break;
		case pROOK:
			this->hash ^= Hashes::PIECES[getPieceType<ROOK>()][end(move)];
			break;
		case pKNIGHT:
			this->knight_cnt++;
			this->hash ^= Hashes::PIECES[getPieceType<KNIGHT>()][end(move)];
			break;
		case pBISHOP:
			this->bishop_cnt++;
			isDark(end(move)) ? this->dark_bishop_cnt++ : this->light_bishop_cnt++;
			this->hash ^= Hashes::PIECES[getPieceType<BISHOP>()][end(move)];
			break;
	}

	// Update hash
	this->hash ^= Hashes::PIECES[piece_moved][start(move)];

	// Update non-position information
	updateCastling(start(move), end(move));
	updateEnPassant(true);
	updateFullmove();
	updateHalfmove(true);
	updateTurn();
}

void Position::makeMove(const Move move, const bool hash) {
	if (move == NULL_MOVE) return;
	saveHistory(move);

	PieceType piece_captured = this->pieces[end(move)];
	switch (type(move)) {
		case NORMAL:
			piece_captured == NO_PIECE ?
					makeMove<NORMAL, NON_CAPTURE>(move) :
					makeMove<NORMAL, CAPTURE>(move);
			break;
		case CASTLING:
			makeMove<CASTLING, NON_CAPTURE>(move);
			break;
		case EN_PASSANT:
			makeMove<EN_PASSANT, CAPTURE>(move);
			break;
		default:
			piece_captured == NO_PIECE ?
					makeMove<PROMOTION, NON_CAPTURE>(move) :
					makeMove<PROMOTION, CAPTURE>(move);
			break;
	}

	if (hash) incrementPositionCounter();
}

template<>
void Position::undoMove<NORMAL>() {
	// Change game history
	History prev = this->history.back();
	this->history.pop_back();

	// Revert non-position information
	this->turn = !this->turn;
	this->hash = prev.hash;
	this->castling = prev.castling;
	this->en_passant = prev.en_passant;
	this->halfmove = prev.halfmove;
	if (this->turn == BLACK) this->fullmove--;

	// Change pieces bitboards, piece list and indices and piece counts
	this->sides[this->turn] ^= ONE_BB << end(prev.move) | ONE_BB << start(prev.move);
	movePieceAndUpdateBitboard(this->pieces[end(prev.move)], end(prev.move), start(prev.move));

	if (prev.captured == NO_PIECE) return;
	placeCapturedPiece(prev.captured, end(prev.move));
	this->piece_cnt++;
	switch (prev.captured) {
		case W_KNIGHT:
		case B_KNIGHT:
			this->knight_cnt++;
			break;
		case W_BISHOP:
		case B_BISHOP:
			this->bishop_cnt++;
			isDark(end(prev.move)) ? this->dark_bishop_cnt++ : this->light_bishop_cnt++;
			break;
		default:
			break;
	}
}

template<>
void Position::undoMove<PROMOTION>() {
	// Change game history
	History prev = history.back();
	this->history.pop_back();

	// Revert non-position information
	this->turn = !this->turn;
	this->hash = prev.hash;
	this->castling = prev.castling;
	this->en_passant = prev.en_passant;
	this->halfmove = prev.halfmove;
	if (this->turn == BLACK) this->fullmove--;

	// Remove promoted piece
	PieceType promoted = this->pieces[end(prev.move)];
	removePiece(end(prev.move), promoted);
	this->sides[this->turn] &= ~(ONE_BB << end(prev.move));
	switch (promoted) {
		case W_KNIGHT:
		case B_KNIGHT:
			this->knight_cnt--;
			break;
		case W_BISHOP:
		case B_BISHOP:
			this->bishop_cnt--;
			isDark(end(prev.move)) ? this->dark_bishop_cnt-- : this->light_bishop_cnt--;
			break;
		default:
			break;
	}

	// Replace captured piece
	if (prev.captured != NO_PIECE) {
		this->piece_cnt++;
		switch (prev.captured) {
			case W_KNIGHT:
			case B_KNIGHT:
				this->knight_cnt++;
				break;
			case W_BISHOP:
			case B_BISHOP:
				this->bishop_cnt++;
				isDark(end(prev.move)) ? this->dark_bishop_cnt++ : this->light_bishop_cnt++;
				break;
			default:
				break;
		}
		placeCapturedPiece(prev.captured, end(prev.move));
	}

	// Replace pawn
	this->turn == WHITE ? addPiece<W_PAWN>(start(prev.move)) : addPiece<B_PAWN>(start(prev.move));
	this->sides[this->turn] ^= ONE_BB << start(prev.move);
	addToBitboard<PAWN>(start(prev.move));
}

template<>
void Position::undoMove<EN_PASSANT>() {
	// Change game history
	History prev = history.back();
	this->history.pop_back();

	// Revert non-position information
	this->turn = !this->turn;
	this->hash = prev.hash;
	this->castling = prev.castling;
	this->en_passant = prev.en_passant;
	this->halfmove = prev.halfmove;
	if (this->turn == BLACK) this->fullmove--;

	// Change bitboards
	Square captured_sq = end(prev.move) + (this->turn == WHITE ? S : N);
	this->sides[this->turn] ^= ONE_BB << end(prev.move) | ONE_BB << start(prev.move);
	this->sides[!this->turn] ^= ONE_BB << captured_sq;
	this->pawns ^= ONE_BB << end(prev.move) | ONE_BB << start(prev.move) | ONE_BB << captured_sq;

	// Update piece list, indices and counts
	this->turn == WHITE ? movePiece<W_PAWN>(end(prev.move), start(prev.move)) :
			movePiece<B_PAWN>(end(prev.move), start(prev.move));
	this->turn == WHITE ? addPiece<B_PAWN>(captured_sq) : addPiece<W_PAWN>(captured_sq);

	// Update piece count
	this->piece_cnt++;
}

template<>
void Position::undoMove<CASTLING>() {
	// Change game history
	History prev = history.back();
	this->history.pop_back();

	// Revert non-position information
	this->turn = !this->turn;
	this->hash = prev.hash;
	this->castling = prev.castling;
	this->en_passant = prev.en_passant;
	this->halfmove = prev.halfmove;
	if (this->turn == BLACK) this->fullmove--;

	// Rook changes
	Square rook_start, rook_end;
	if (end(prev.move) == G1) {
		rook_start = H1;
		rook_end = F1;
	} else if (end(prev.move) == C1) {
		rook_start = A1;
		rook_end = D1;
	} else if (end(prev.move) == G8) {
		rook_start = H8;
		rook_end = F8;
	} else {
		rook_start = A8;
		rook_end = D8;
	}

	// Change sides bitboards
	this->sides[this->turn] ^= ONE_BB << end(prev.move) | ONE_BB << start(prev.move);
	this->sides[this->turn] ^= ONE_BB << rook_end | ONE_BB << rook_start;

	// Change king bitboards
	this->kings ^= ONE_BB << end(prev.move) | ONE_BB << start(prev.move);
	this->rooks ^= ONE_BB << rook_end | ONE_BB << rook_start;

	// Update piece list, indices and counts
	this->turn == WHITE ? movePiece<W_KING>(end(prev.move), start(prev.move)) :
			movePiece<B_KING>(end(prev.move), start(prev.move));
	this->turn == WHITE ? movePiece<W_ROOK>(rook_end, rook_start) : movePiece<B_ROOK>(rook_end, rook_start);
}

void Position::undoMove() {
	decrementPositionCounter(this->hash);
	switch (type(this->history.back().move)) {
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

template <>
bool Position::castleBit<WKSC>() const {
	return this->castling & (1 << WKSC);
}

template <>
bool Position::castleBit<WQSC>() const {
	return this->castling & (1 << WQSC);
}

template <>
bool Position::castleBit<BKSC>() const {
	return this->castling & (1 << BKSC);
}

template <>
bool Position::castleBit<BQSC>() const {
	return this->castling & (1 << BQSC);
}

void Position::updateTurn() {
	this->turn = !this->turn;
}

void Position::display() const {
	// Print the pieces
	bool letterMode = false;
	for (int rank = RANK_8; rank >= RANK_1; rank--) {
		std::cout << static_cast<char>((rank - 7) + '8') << ' ';
		for (int file = FILE_A; file <= FILE_H; file++)
			std::cout << getSquareCharacters(static_cast<Square>(RANK_COUNT * rank + file), letterMode);
		std::cout << '\n';
	}
	std::cout << "  a b c d e f g h\n";

	// Print turn
	if (this->turn)
		std::cout << "Turn: white\n";
	else
		std::cout << "Turn: black\n";

	// Print castling
	std::string castling = "Castling: ";
	if (this->castleBit<WKSC>()) castling += "K";
	if (this->castleBit<WQSC>()) castling += "Q";
	if (this->castleBit<BKSC>()) castling += "k";
	if (this->castleBit<BQSC>()) castling += "q";
	if (this->getCastling() == 0) castling += "-";
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

void Position::updateCastling(const Square start, const Square end) {
	if (start == E1 && WHITE_CASTLING & this->castling) {
		this->hash ^= Hashes::CASTLING[this->castling];
		this->castling &= BLACK_CASTLING;
		this->hash ^= Hashes::CASTLING[this->castling];
	} else if (start == E8 && BLACK_CASTLING & this->castling) {
		this->hash ^= Hashes::CASTLING[this->castling];
		this->castling &= WHITE_CASTLING;
		this->hash ^= Hashes::CASTLING[this->castling];
	} else {
		if ((start == H1 || end == H1) && isSet(this->castling, WKSC)) {
			this->hash ^= Hashes::CASTLING[this->castling];
			this->castling &= ~(1 << WKSC);
			this->hash ^= Hashes::CASTLING[this->castling];
		}

		if ((start == A1 || end == A1) && isSet(this->castling, WQSC)) {
			this->hash ^= Hashes::CASTLING[this->castling];
			this->castling &= ~(1 << WQSC);
			this->hash ^= Hashes::CASTLING[this->castling];
		}

		if ((start == H8 || end == H8) && isSet(this->castling, BKSC)) {
			this->hash ^= Hashes::CASTLING[this->castling];
			this->castling &= ~(1 << BKSC);
			this->hash ^= Hashes::CASTLING[this->castling];
		}

		if ((start == A8 || end == A8) && isSet(this->castling, BQSC)) {
			this->hash ^= Hashes::CASTLING[this->castling];
			this->castling &= ~(1 << BQSC);
			this->hash ^= Hashes::CASTLING[this->castling];
		}
	}
}

void Position::updateEnPassant(const bool clear) {
	if (this->en_passant != NONE)
		this->hash ^= Hashes::EN_PASSANT[file(this->en_passant)];
	if (clear)
		this->en_passant = NONE;
}

void Position::updateHalfmove(const bool zero) {
	if (zero) {
		this->halfmove = 0;
	} else {
		this->halfmove++;
	}
}

void Position::updateFullmove() {
	if (this->turn == BLACK) {
		this->fullmove++;
		this->hash ^= Hashes::TURN;
	}
}

void Position::updatePieceCounts(const PieceType piece_captured, const Move move) {
	this->piece_cnt--;
	if (piece_captured == W_KNIGHT || piece_captured == B_KNIGHT)
		this->knight_cnt--;
	if (piece_captured == W_BISHOP || piece_captured == B_BISHOP) {
		this->bishop_cnt--;
		isDark(end(move)) ? this->dark_bishop_cnt-- : this->light_bishop_cnt--;
	}
}

void Position::placeCapturedPiece(const PieceType piece, const Square square) {
	this->sides[!this->turn] ^= ONE_BB << square;
	switch (piece) {
		case W_KING:
		case B_KING:
			this->turn == WHITE ? addPiece<B_KING>(square) : addPiece<W_KING>(square);
			addToBitboard<KING>(square);
			break;
		case W_QUEEN:
		case B_QUEEN:
			this->turn == WHITE ? addPiece<B_QUEEN>(square) : addPiece<W_QUEEN>(square);
			addToBitboard<QUEEN>(square);
			break;
		case W_ROOK:
		case B_ROOK:
			this->turn == WHITE ? addPiece<B_ROOK>(square) : addPiece<W_ROOK>(square);
			addToBitboard<ROOK>(square);
			break;
		case W_BISHOP:
		case B_BISHOP:
			this->turn == WHITE ? addPiece<B_BISHOP>(square) : addPiece<W_BISHOP>(square);
			addToBitboard<BISHOP>(square);
			break;
		case W_KNIGHT:
		case B_KNIGHT:
			this->turn == WHITE ? addPiece<B_KNIGHT>(square) : addPiece<W_KNIGHT>(square);
			addToBitboard<KNIGHT>(square);
			break;
		case W_PAWN:
		case B_PAWN:
			this->turn == WHITE ? addPiece<B_PAWN>(square) : addPiece<W_PAWN>(square);
			addToBitboard<PAWN>(square);
			break;
		default:
			assert(false);
	}
}

void Position::movePieceAndUpdateBitboard(const PieceType piece, const Square start, const Square end) {
	switch (piece) {
		case W_KING:
		case B_KING:
			this->turn == WHITE ? movePiece<W_KING>(start, end) : movePiece<B_KING>(start, end);
			movePieceBitboard<KING>(start, end);
			break;
		case W_QUEEN:
		case B_QUEEN:
			this->turn == WHITE ? movePiece<W_QUEEN>(start, end) : movePiece<B_QUEEN>(start, end);
			movePieceBitboard<QUEEN>(start, end);
			break;
		case W_ROOK:
		case B_ROOK:
			this->turn == WHITE ? movePiece<W_ROOK>(start, end) : movePiece<B_ROOK>(start, end);
			movePieceBitboard<ROOK>(start, end);
			break;
		case W_BISHOP:
		case B_BISHOP:
			this->turn == WHITE ? movePiece<W_BISHOP>(start, end) : movePiece<B_BISHOP>(start, end);
			movePieceBitboard<BISHOP>(start, end);
			break;
		case W_KNIGHT:
		case B_KNIGHT:
			this->turn == WHITE ? movePiece<W_KNIGHT>(start, end) : movePiece<B_KNIGHT>(start, end);
			movePieceBitboard<KNIGHT>(start, end);
			break;
		case W_PAWN:
		case B_PAWN:
			this->turn == WHITE ? movePiece<W_PAWN>(start, end) : movePiece<B_PAWN>(start, end);
			movePieceBitboard<PAWN>(start, end);
			break;
		default:
			std::cout << "This shouldn't be happening... " << piece << '\n';
			assert(false);
	}
}

void Position::saveHistory(const Move move) {
	this->history.push_back({
		this->castling,
		this->en_passant,
		this->halfmove,
		this->hash,
		move,
		this->pieces[end(move)],
	});
}

void Position::addToPromotionBitboard(const Move move) {
	switch (promo(move)) {
		case pQUEEN:
			setBit(this->queens, end(move));
			break;
		case pROOK:
			setBit(this->rooks, end(move));
			break;
		case pBISHOP:
			setBit(this->bishops, end(move));
			break;
		case pKNIGHT:
			setBit(this->knights, end(move));
			break;
		default:
			assert(false);
	}
}

void Position::removeFromBitboardPromotion(const Move move) {
	switch (this->pieces[end(move)]) {
		case W_QUEEN:
		case B_QUEEN:
			zeroBit(this->queens, end(move));
			break;
		case W_ROOK:
		case B_ROOK:
			zeroBit(this->rooks, end(move));
			break;
		case W_BISHOP:
		case B_BISHOP:
			zeroBit(this->bishops, end(move));
			break;
		case W_KNIGHT:
		case B_KNIGHT:
			zeroBit(this->knights, end(move));
			break;
		default:
			assert(false);
	}
}

void Position::addPiecePromotion(const Move move) {
	switch (promo(move)) {
		case pQUEEN:
			this->turn == WHITE ? addPiece<W_QUEEN>(end(move)) : addPiece<B_QUEEN>(end(move));
			break;
		case pROOK:
			this->turn == WHITE ? addPiece<W_ROOK>(end(move)) : addPiece<B_ROOK>(end(move));
			break;
		case pBISHOP:
			this->turn == WHITE ? addPiece<W_BISHOP>(end(move)) : addPiece<B_BISHOP>(end(move));
			break;
		case pKNIGHT: // pKNIGHT
			this->turn == WHITE ? addPiece<W_KNIGHT>(end(move)) : addPiece<B_KNIGHT>(end(move));
			break;
		default:
			assert(false);
	}
}

void Position::removePiecePromotion(const Move move) {
	switch (this->pieces[end(move)]) {
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

inline PieceType Position::getPromotionPiece(const Move& move) const {
	switch (promo(move)) {
		case pQUEEN:
			return (this->turn == WHITE) ? W_QUEEN : B_QUEEN;
		case pROOK:
			return (this->turn == WHITE) ? W_ROOK : B_ROOK;
		case pBISHOP:
			return (this->turn == WHITE) ? W_BISHOP : B_BISHOP;
		default: // pKNIGHT
			return (this->turn == WHITE) ? W_KNIGHT : B_KNIGHT;
	}
}

inline void Position::setCheckers() {
	this->checkers = isAttacked(getKingSquare(), !this->turn);
}

inline bool Position::isPawnPinnedByRookHorizontally(const Square square) const {
	return (rank(getKingSquare()) == rank(square)) && isPinnedByRook(square);
}

inline bool Position::isPawnPinnedByRookVertically(const Square square) const {
	return (file(getKingSquare()) == file(square)) && isPinnedByRook(square);
}

void Position::setPinAndCheckRayBitboards() {
	this->rook_pins = ZERO_BB;
	this->bishop_pins = ZERO_BB;
	this->rook_ep_pins = ZERO_BB;
	this->check_rays = checkers;

	// Enemy queen rays
	for (uint i = 0; i < this->piece_index[getPieceType<QUEEN>(true)]; i++) {
		Bitboard ray = Rays::DIAGONAL[getKingSquare()][this->piece_list[getPieceType<QUEEN>(true)][i]];
		if (oneBitSet(ray & this->sides[this->turn]) && oneBitSet(ray & this->sides[!this->turn]))
			this->bishop_pins |= ray;
		if (oneBitSet(ray & getPieces()))
			this->check_rays |= ray;

		ray = Rays::LEVEL[getKingSquare()][this->piece_list[getPieceType<QUEEN>(true)][i]];
		if (oneBitSet(ray & this->sides[this->turn]) && oneBitSet(ray & this->sides[!this->turn]))
			this->rook_pins |= ray;
		if (oneBitSet(ray & getPieces()))
			this->check_rays |= ray;
		if (rank(getKingSquare()) == rank(this->piece_list[getPieceType<QUEEN>(true)][i]))
			this->rook_ep_pins |= ray;
	}

	// Enemy rooks
	for (uint i = 0; i < this->piece_index[getPieceType<ROOK>(true)]; i++) {
		Bitboard ray = Rays::LEVEL[getKingSquare()][this->piece_list[getPieceType<ROOK>(true)][i]];
		if (oneBitSet(ray & this->sides[this->turn]) && oneBitSet(ray & this->sides[!this->turn]))
			this->rook_pins |= ray;
		if (oneBitSet(ray & getPieces()))
			this->check_rays |= ray;
		if (rank(getKingSquare()) == rank(this->piece_list[getPieceType<ROOK>(true)][i]))
			this->rook_ep_pins |= ray;
	}

	// Enemy bishops
	for (uint i = 0; i < this->piece_index[getPieceType<BISHOP>(true)]; i++) {
		Bitboard ray = Rays::DIAGONAL[getKingSquare()][this->piece_list[getPieceType<BISHOP>(true)][i]];
		if (oneBitSet(ray & this->sides[this->turn]) && oneBitSet(ray & this->sides[!this->turn]))
			this->bishop_pins |= ray;
		if (oneBitSet(ray & getPieces()))
			this->check_rays |= ray;
	}
}

void Position::parseFenMove(const std::string& fenMove) {
	if (fenMove == "w")
		this->turn = WHITE;
	else
		this->turn = BLACK;
}

void Position::parseFenCastling(const std::string& fenCastling) {
	this->castling = 0;
	if (fenCastling == "-") return;
	for (char c : fenCastling) {
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

void Position::parseFenEnPassant(const std::string& fenEnPassant) {
	if (fenEnPassant == "-") {
		this->en_passant = NONE;
		return;
	}

	int value = -1;
	for (char c : fenEnPassant) {
		if (value == -1)
			value = (c - 'a');
		else
			value += 8 * (c - '1');
	}
	this->en_passant = (Square) value;
}

void Position::parseFenMoves(const std::string& halfmove, const std::string& fullmove) {
	this->halfmove = std::stoi(halfmove);
	this->fullmove = std::stoi(fullmove);
}

void Position::addFenPiece(Bitboard& pieceBB, const PieceType piece, const int squareIndex) {
	pieceBB |= ONE_BB << squareIndex;
	this->piece_list[piece][this->piece_index[piece]] = static_cast<Square>(squareIndex);
	this->piece_index[piece]++;
	this->pieces[squareIndex] = piece;
}

void Position::parseFenChar(const char piece, const int squareIndex) {
	if (piece == 'K') addFenPiece(this->kings, W_KING, squareIndex);
	if (piece == 'Q') addFenPiece(this->queens, W_QUEEN, squareIndex);
	if (piece == 'R') addFenPiece(this->rooks, W_ROOK, squareIndex);
	if (piece == 'P') addFenPiece(this->pawns, W_PAWN, squareIndex);
	if (piece == 'k') addFenPiece(this->kings, B_KING, squareIndex);
	if (piece == 'q') addFenPiece(this->queens, B_QUEEN, squareIndex);
	if (piece == 'r') addFenPiece(this->rooks, B_ROOK, squareIndex);
	if (piece == 'p') addFenPiece(this->pawns, B_PAWN, squareIndex);

	if (piece == 'B') {
		addFenPiece(this->bishops, W_BISHOP, squareIndex);
		this->bishop_cnt++;
		isDark(static_cast<Square>(squareIndex)) ? this->dark_bishop_cnt++ : this->light_bishop_cnt++;
	}

	if (piece == 'N') {
		addFenPiece(this->knights, W_KNIGHT, squareIndex);
		this->knight_cnt++;
	}

	if (piece == 'b') {
		addFenPiece(this->bishops, B_BISHOP, squareIndex);
		this->bishop_cnt++;
		isDark(static_cast<Square>(squareIndex)) ? this->dark_bishop_cnt++ : this->light_bishop_cnt++;
	}

	if (piece == 'n') {
		addFenPiece(this->knights, B_KNIGHT, squareIndex);
		this->knight_cnt++;
	}
}

bool Position::isDrawInsufficientMaterial() const {
	// King vs king
	if (this->piece_cnt == 2) return true;

	// King vs king and bishop
	if (this->piece_cnt == 3 && this->bishop_cnt == 1) return true;

	// King vs king and knight
	if (this->piece_cnt == 3 && this->knight_cnt == 1) return true;

	// King and bishop vs king and bishop (bishops same colour)
	uint nonBishopCnt = this->piece_cnt - this->bishop_cnt;
	if (nonBishopCnt == 2 && (static_cast<bool>(this->light_bishop_cnt) != static_cast<bool>(this->dark_bishop_cnt)))
		return true;

	return false;
}

bool Position::isDrawStalemate(MoveList& move_list) const {
	return move_list.moves_index == 0 && !inCheck();
}

bool Position::isCheckmate(MoveList& move_list) const {
	return move_list.moves_index == 0 && inCheck();
}

inline bool Position::inCheck() const {
	return this->checkers;
}

void Position::getKingMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	Bitboard reachBB = Masks::KING[getKingSquare()] & ~this->sides[this->turn];

	if (reachBB == ZERO_BB) return;
	for (Square square : KingReach::SQUARES[getKingSquare()][getKingMovesIndex(reachBB, getKingSquare())]) {
		if (!isAttacked(square, (Player) !this->turn, true)) continue;
		reachBB &= ~(ONE_BB << square);
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
	getEnPassantCheckMoves(moves_index, pos_moves);
}

void Position::getQueenCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const { // TODO continue hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
	for (uint i = 0; i < this->piece_index[getPieceType<QUEEN>()]; i++) {
		Square queenSquare = this->piece_list[getPieceType<QUEEN>()][i];
		if (isPinned(queenSquare)) continue;
		Bitboard reach = getRookReachBB(Masks::ROOK[queenSquare] & getPieces(), queenSquare) & this->check_rays;
		if (reach != ZERO_BB) pos_moves[moves_index++] =
				&Moves::Blocks::ROOK[queenSquare][getRookBlockIndex(reach, queenSquare)];
		reach = getBishopReachBB(Masks::BISHOP[queenSquare] & getPieces(), queenSquare) & this->check_rays;
		if (reach != ZERO_BB) pos_moves[moves_index++] =
				&Moves::Blocks::BISHOP[queenSquare][getBishopBlockIndex(reach, queenSquare)];
	}
}

void Position::getRookCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	for (uint i = 0; i < this->piece_index[getPieceType<ROOK>()]; i++) {
		Square rookSquare = this->piece_list[getPieceType<ROOK>()][i];
		if (isPinned(rookSquare)) continue;
		Bitboard reach = getRookReachBB(Masks::ROOK[rookSquare] & getPieces(), rookSquare) & this->check_rays;
		if (reach != ZERO_BB) pos_moves[moves_index++] =
				&Moves::Blocks::ROOK[rookSquare][getRookBlockIndex(reach, rookSquare)];
	}
}

void Position::getBishopCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	for (uint i = 0; i < this->piece_index[getPieceType<BISHOP>()]; i++) {
		Square bishopSquare = this->piece_list[getPieceType<BISHOP>()][i];
		if (isPinned(bishopSquare)) continue;
		Bitboard reach = getBishopReachBB(Masks::BISHOP[bishopSquare] & getPieces(), bishopSquare) &
				this->check_rays;
		if (reach != ZERO_BB) pos_moves[moves_index++] =
				&Moves::Blocks::BISHOP[bishopSquare][getBishopBlockIndex(reach, bishopSquare)];
	}
}

void Position::getKnightCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	for (uint i = 0; i < this->piece_index[getPieceType<KNIGHT>()]; i++) {
		Square knightSquare = this->piece_list[getPieceType<KNIGHT>()][i];
		if (!isPinned(knightSquare)) {
			Bitboard reach = Masks::KNIGHT[knightSquare] & this->check_rays;
			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::KNIGHT[knightSquare][getKnightMovesIndex(reach, knightSquare)];
		}
	}
}

void Position::getPawnCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	for (uint i = 0; i < this->piece_index[getPieceType<PAWN>()]; i++) {
		Square pawnSquare = this->piece_list[getPieceType<PAWN>()][i];
		if (!isPinned(pawnSquare)) {
			Bitboard reach = ZERO_BB;
			Square pawnPush = pawnSquare + (this->turn == WHITE ? N : S);
			if (!((ONE_BB << pawnPush) & getPieces())) {
				Rank startRank = this->turn == WHITE ? RANK_2 : RANK_7;
				Square pawnDoublePush = pawnSquare + (this->turn == WHITE ? NN : SS);
				if ((ONE_BB << pawnPush) & (this->check_rays & ~this->sides[!this->turn])) {
					reach |= ONE_BB << pawnPush;
				} else if (rank(pawnSquare) == startRank && !isOccupied(pawnDoublePush) && (ONE_BB << pawnDoublePush) &
						(this->check_rays & ~this->sides[!this->turn])) {
					reach |= ONE_BB << pawnDoublePush;
				}
			}

			reach |= Masks::PAWN[this->turn][pawnSquare] & ~Masks::FILE[file(pawnSquare)] &
					(this->check_rays & this->sides[!this->turn]);
			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::PAWN[this->turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, this->turn)];
		}
	}
}

void Position::initialiseHash() {
	this->hash = ZERO_BB;

	// Hash the pieces
	for (Square sq = A1; sq <= H8; sq++) {
		if (this->pieces[sq] != NO_PIECE) this->hash ^= Hashes::PIECES[this->pieces[sq]][sq];
	}

	// Hash turn
	if (this->turn == BLACK) this->hash ^= Hashes::TURN;

	// Hash the castling
	this->hash ^= Hashes::CASTLING[castling];

	// Hash en-passant
	if (this->en_passant != NONE)
		this->hash ^= Hashes::EN_PASSANT[file(this->en_passant)];
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
	for (uint i = 0; i < this->piece_index[getPieceType<QUEEN>()]; i++) {
		Square queenSquare = this->piece_list[getPieceType<QUEEN>()][i];
		if (isPinnedByBishop(queenSquare)) {
			getBishopPinMoves(moves_index, pos_moves, queenSquare);
		} else if (isPinnedByRook(queenSquare)) {
			getRookPinMoves(moves_index, pos_moves, queenSquare);
		} else {
			Bitboard reach = getBishopReachBB(Masks::BISHOP[queenSquare] & getPieces(), queenSquare) &
					~this->sides[this->turn];
			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::BISHOP[queenSquare][getBishopMovesIndex(reach, queenSquare)];

			reach = getRookReachBB(Masks::ROOK[queenSquare] & getPieces(), queenSquare) & ~this->sides[this->turn];
			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::ROOK[queenSquare][getRookMovesIndex(reach, queenSquare)];
		}
	}
}

void Position::getRookMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	for (uint i = 0; i < this->piece_index[getPieceType<ROOK>()]; i++) {
		Square rookSquare = this->piece_list[getPieceType<ROOK>()][i];
		if (isPinnedByBishop(rookSquare)) {
			continue;
		} else if (isPinnedByRook(rookSquare)) {
			getRookPinMoves(moves_index, pos_moves, rookSquare);
		} else {
			Bitboard reach = getRookReachBB(Masks::ROOK[rookSquare] & getPieces(), rookSquare) &
					~this->sides[this->turn];
			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::ROOK[rookSquare][getRookMovesIndex(reach, rookSquare)];
		}
	}
}

void Position::getBishopMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	for (uint i = 0; i < this->piece_index[getPieceType<BISHOP>()]; i++) {
		Square bishopSquare = this->piece_list[getPieceType<BISHOP>()][i];
		if (isPinnedByBishop(bishopSquare)) {
			getBishopPinMoves(moves_index, pos_moves, bishopSquare);
		} else if (isPinnedByRook(bishopSquare)) {
			continue;
		} else {
			Bitboard reach = getBishopReachBB(Masks::BISHOP[bishopSquare] & getPieces(), bishopSquare) &
					~this->sides[this->turn];
			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::BISHOP[bishopSquare][getBishopMovesIndex(reach, bishopSquare)];
		}
	}
}

void Position::getKnightMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	for (uint i = 0; i < this->piece_index[getPieceType<KNIGHT>()]; i++) {
		Square knightSquare = this->piece_list[getPieceType<KNIGHT>()][i];
		if (!isPinnedByBishop(knightSquare) && !isPinnedByRook(knightSquare)) {
			Bitboard reach = Masks::KNIGHT[knightSquare] & ~this->sides[this->turn];
			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::KNIGHT[knightSquare][getKnightMovesIndex(reach, knightSquare)];
		}
	}
}

void Position::getPawnMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	for (uint i = 0; i < this->piece_index[getPieceType<PAWN>()]; i++) {
		Square pawnSquare = this->piece_list[getPieceType<PAWN>()][i];
		if (isPinnedByBishop(pawnSquare)) {
			Bitboard reach = (Masks::PAWN[this->turn][pawnSquare] & ~Masks::FILE[file(pawnSquare)]);
			reach &= this->bishop_pins & (this->sides[!this->turn] & (this->queens | this->bishops));
			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::PAWN[this->turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, this->turn)];
		} else if (isPawnPinnedByRookHorizontally(pawnSquare)) {
			continue; // Can't move if horizontally pinned to king.
		} else {
			Bitboard reach = ZERO_BB;

			// Can't capture if vertically pinned to king.
			if (!isPawnPinnedByRookVertically(pawnSquare)) reach |= Masks::PAWN[this->turn][pawnSquare] &
					~Masks::FILE[file(pawnSquare)] & this->sides[!this->turn];

			Direction advance = this->turn == WHITE ? N : S;
			if (!(ONE_BB << (pawnSquare + advance) & getPieces())) {
				Rank startRank = this->turn == WHITE ? RANK_2 : RANK_7;
				reach |= ONE_BB << (pawnSquare + advance);
				if (rank(pawnSquare) == startRank && !(ONE_BB << (pawnSquare + advance + advance) & getPieces())) {
					reach |= ONE_BB << (pawnSquare + advance + advance);
				}
			}

			if (reach != ZERO_BB) pos_moves[moves_index++] =
					&Moves::PAWN[this->turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, this->turn)];
		}
	}
}

inline Bitboard Position::getRookReachBB(const Bitboard occupancy, const Square square) const {
	return Reach::ROOK[square][Indices::ROOK[square][getRookReachIndex(occupancy, square)]];
}

inline Bitboard Position::getBishopReachBB(const Bitboard occupancy, const Square square) const {
	return Reach::BISHOP[square][Indices::BISHOP[square][getBishopReachIndex(occupancy, square)]];
}

void Position::getBishopPinMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE], const Square square) const {
	Bitboard reach = this->bishop_pins & ~(ONE_BB << square) &
			getBishopReachBB(Masks::BISHOP[square] & getPieces(), square);
	if (reach == ZERO_BB) return;
	pos_moves[moves_index++] = &Moves::BISHOP[square][getBishopMovesIndex(reach, square)];
}

void Position::getRookPinMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE], const Square square) const {
	Bitboard reach = this->rook_pins & ~(ONE_BB << square) & getRookReachBB(Masks::ROOK[square] & getPieces(), square);
	if (reach == ZERO_BB) return;
	pos_moves[moves_index++] = &Moves::ROOK[square][getRookMovesIndex(reach, square)];
}

Bitboard Position::isOccupied(const Square square) const {
	return getPieces() & (ONE_BB << square);
}

void Position::getCastlingMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	if (this->turn) {
		if (this->castling & (1 << WKSC)) {
			if (!isOccupied(F1) && !isOccupied(G1) && !isAttacked(F1, !this->turn) && !isAttacked(G1, !this->turn)) {
				pos_moves[moves_index++] = &Moves::CASTLING[WKSC];
			}
		}

		if (this->castling & (1 << WQSC)) {
			if (!isOccupied(D1) && !isOccupied(C1) && !isOccupied(B1) && !isAttacked(D1, !this->turn) &&
					!isAttacked(C1, !this->turn)) {
				pos_moves[moves_index++] = &Moves::CASTLING[WQSC];
			}
		}
	} else {
		if (this->castling & (1 << BKSC)) {
			if (!isOccupied(F8) && !isOccupied(G8) && !isAttacked(F8, !this->turn) && !isAttacked(G8, !this->turn)) {
				pos_moves[moves_index++] = &Moves::CASTLING[BKSC];
			}
		}

		if (this->castling & (1 << BQSC)) {
			if (!isOccupied(C8) && !isOccupied(B8) && !isOccupied(D8) && !isAttacked(D8, !this->turn) &&
					!isAttacked(C8, !this->turn)) {
				pos_moves[moves_index++] = &Moves::CASTLING[BQSC];
			}
		}
	}
}

void Position::getEnPassantMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	if (!this->en_passant) return;

	std::vector<Square> pawnSquares;
	if (file(this->en_passant) != FILE_A)
		pawnSquares.push_back(this->en_passant + (this->turn == WHITE ? SW : NW));
	if (file(this->en_passant) != FILE_H)
		pawnSquares.push_back(this->en_passant + (this->turn == WHITE ? SE : NE));

	for (Square square : pawnSquares) {
		bool attackPawn = this->pieces[square] == getPieceType<PAWN>();
		bool pinnedByBishop = isPinnedByBishop(square);
		bool enPassantPinned = isPinnedByBishop(this->en_passant);
		bool pinnedByRook = isPinnedByRook(square);
		if (attackPawn && (!pinnedByBishop || (pinnedByBishop && enPassantPinned)) && !pinnedByRook) { // TODO simplify
			bool pinned = oneBitSet(getPieces() & rook_ep_pins & ~(ONE_BB << square | ONE_BB << (this->en_passant +
					(this->turn == WHITE ? S : N))));
			if (!pinned) pos_moves[moves_index++] = &Moves::EN_PASSANT[this->turn][file(this->en_passant)]
					[file(square) < file(this->en_passant) ? 0 : 1];
		}
	}
}

void Position::getEnPassantCheckMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const {
	if (!this->en_passant) return;

	std::vector<Square> pawnSquares;
	if (file(this->en_passant) != FILE_A)
		pawnSquares.push_back(this->en_passant + (this->turn == WHITE ? SW : NW));
	if (file(this->en_passant) != FILE_H)
		pawnSquares.push_back(this->en_passant + (this->turn == WHITE ? SE : NE));

	for (Square square : pawnSquares) {
		if ((this->pieces[square] == getPieceType<PAWN>()) && (!isPinnedByBishop(square) ||
				(isPinnedByBishop(square) && isPinnedByBishop(this->en_passant))) && !isPinnedByRook(square)) { // TODO simplify
			bool pinned = oneBitSet(getPieces() & this->rook_ep_pins & ~(ONE_BB << square |
					ONE_BB << (this->en_passant + (this->turn == WHITE ? S : N))));
			if (!pinned) pos_moves[moves_index++] = &Moves::EN_PASSANT[turn][file(this->en_passant)]
					[file(square) < file(this->en_passant) ? 0 : 1];
		}
	}
}

inline bool Position::inDoubleCheck() const {
	#ifdef USE_PEXT
	return _blsr_u64(this->checkers) != ZERO_BB;
	#else
	return (this->checkers & (this->checkers - ONE_BB)) != ZERO_BB;
	#endif
}

bool Position::isDrawThreeFoldRep() const {
	auto val = this->positionCounts.find(this->hash);
	return val != this->positionCounts.end() && val->second >= 3;
}

Bitboard Position::isAttacked(const Square square, const Player player, const bool ignoreKing) const {
	Bitboard pieces = getPieces() ^ (ignoreKing ? ONE_BB << this->piece_list[!player][0] : ZERO_BB);
	Bitboard kingBB = Masks::KING[square] & this->sides[player] & this->kings;
	Bitboard rooksBB = getRookReachBB(Masks::ROOK[square] & pieces, square) & this->sides[player] &
			(this->queens | this->rooks);
	Bitboard bishopsBB = getBishopReachBB(Masks::BISHOP[square] & pieces, square) & this->sides[player] &
			(this->queens | this->bishops);
	Bitboard knightsBB = Masks::KNIGHT[square] & this->sides[player] & this->knights;
	Bitboard pawnsBB = Masks::PAWN[!player][square] & ~Masks::FILE[file(square)] & this->sides[player] & this->pawns;
	return kingBB | rooksBB | bishopsBB | knightsBB | pawnsBB;
}

void Position::removePiece(const Square square, const PieceType piece_captured) {
	switch (piece_captured) {
		case W_QUEEN:
			this->queens &= ~(ONE_BB << square);
			removePiece<W_QUEEN>(square);
			break;
		case B_QUEEN:
			this->queens &= ~(ONE_BB << square);
			removePiece<B_QUEEN>(square);
			break;
		case W_ROOK:
			this->rooks &= ~(ONE_BB << square);
			removePiece<W_ROOK>(square);
			break;
		case B_ROOK:
			this->rooks &= ~(ONE_BB << square);
			removePiece<B_ROOK>(square);
			break;
		case W_BISHOP:
			this->bishops &= ~(ONE_BB << square);
			removePiece<W_BISHOP>(square);
			break;
		case B_BISHOP:
			this->bishops &= ~(ONE_BB << square);
			removePiece<B_BISHOP>(square);
			break;
		case W_KNIGHT:
			this->knights &= ~(ONE_BB << square);
			removePiece<W_KNIGHT>(square);
			break;
		case B_KNIGHT:
			this->knights &= ~(ONE_BB << square);
			removePiece<B_KNIGHT>(square);
			break;
		case W_PAWN:
			this->pawns &= ~(ONE_BB << square);
			removePiece<W_PAWN>(square);
			break;
		case B_PAWN:
			this->pawns &= ~(ONE_BB << square);
			removePiece<B_PAWN>(square);
			break;
		default:
			std::cerr << "This shouldn't be happening... " << piece_captured << '\n';
			assert(false);
	}
}

void Position::decrementPositionCounter(const Bitboard hash) {
	auto it = this->positionCounts.find(hash);
	if (it != this->positionCounts.end()) {
		this->positionCounts[hash]--;
		if (this->positionCounts[hash] == 0) this->positionCounts.erase(it);
	}
}

Move Position::getMovefromAlgebraic(const std::string& string) const {
	if (string[0] < 'a' || string[0] > 'h' || string[2] < 'a' || string[2] > 'h') return NULL_MOVE;
	if (string[1] < '1' || string[1] > '8' || string[3] < '1' || string[3] > '8') return NULL_MOVE;
	if (string.size() < 4 || string.size() > 5) return NULL_MOVE;

	// Get rank and file of move.
	Move move = 0;
	int start_file = string[0] - 'a';
	int start_rank = string[1] - '1';
	Square start = static_cast<Square>(RANK_COUNT * start_rank + start_file);
	int end_file = string[2] - 'a';
	int end_rank = string[3] - '1';
	Square end = static_cast<Square>(RANK_COUNT * end_rank + end_file);
	move |= (start | end << DEST_SHIFT);

	// Check if promotion move.
	if (string.length() == 5) {
		move |= PROMOTION;
		if (string[4] == 'q') {
			move |= pQUEEN;
		} else if (string[4] == 'r') {
			move |= pROOK;
		} else if (string[4] == 'b') {
			move |= pBISHOP;
		} else {
			move |= pKNIGHT;
		}
	}

	// Check if en passant move.
	bool pawnMove = this->pieces[start] == W_PAWN || this->pieces[start] == B_PAWN;
	bool endOnEnPassantSquare = this->en_passant == end;
	if (endOnEnPassantSquare && pawnMove) move |= EN_PASSANT;

	// Check if castling move.
	bool kingMove = this->pieces[start] == W_KING || this->pieces[start] == B_KING;
	bool twoSquareMove = std::abs(start_file - end_file) == 2;
	if (twoSquareMove && kingMove) move |= CASTLING;

	return move;
}

void Position::incrementPositionCounter() {
	auto record = this->positionCounts.find(this->hash);
	if (record != this->positionCounts.end())
		this->positionCounts[this->hash]++;
	else
		this->positionCounts.insert(std::pair<Bitboard, int>(this->hash, 1));
}

inline std::string Position::darkSquare(const std::string str) const {
	return "\033[1;30;45m" + str + " \033[0m";
}

inline std::string Position::lightSquare(const std::string str) const {
	return "\033[1;30;47m" + str + " \033[0m";
}

std::string Position::getSquareCharacters(const Square square, const bool letterMode) const {
	PieceType piece = this->pieces[square];
	switch (piece) {
		case W_KING:
			return letterMode ? "K " : isDark(square) ? darkSquare("\u2654") : lightSquare("\u2654");
		case B_KING:
			return letterMode ? "k " : isDark(square) ? darkSquare("\u265A") : lightSquare("\u265A");
		case W_QUEEN:
			return letterMode ? "Q " : isDark(square) ? darkSquare("\u2655") : lightSquare("\u2655");
		case B_QUEEN:
			return letterMode ? "q " : isDark(square) ? darkSquare("\u265B") : lightSquare("\u265B");
		case W_ROOK:
			return letterMode ? "R " : isDark(square) ? darkSquare("\u2656") : lightSquare("\u2656");
		case B_ROOK:
			return letterMode ? "r " : isDark(square) ? darkSquare("\u265C") : lightSquare("\u265C");
		case W_BISHOP:
			return letterMode ? "B " : isDark(square) ? darkSquare("\u2657") : lightSquare("\u2657");
		case B_BISHOP:
			return letterMode ? "b " : isDark(square) ? darkSquare("\u265D") : lightSquare("\u265D");
		case W_KNIGHT:
			return letterMode ? "N " : isDark(square) ? darkSquare("\u2658") : lightSquare("\u2658");
		case B_KNIGHT:
			return letterMode ? "n " : isDark(square) ? darkSquare("\u265E") : lightSquare("\u265E");
		case W_PAWN:
			return letterMode ? "P " : isDark(square) ? darkSquare("\u2659") : lightSquare("\u2659");
		case B_PAWN:
			return letterMode ? "p " : isDark(square) ? darkSquare("\u265F") : lightSquare("\u265F");
		default:
			return isDark(square) ? darkSquare(" ") : lightSquare(" ");
	}
}

void Position::displayBitboards() const {
	std::cout << "White:" << '\n';
	displayBB(this->sides[WHITE]);
	std::cout << "Black:" << '\n';
	displayBB(this->sides[BLACK]);
	std::cout << "Kings:" << '\n';
	displayBB(this->kings);
	std::cout << "Queens:" << '\n';
	displayBB(this->queens);
	std::cout << "Rooks:" << '\n';
	displayBB(this->rooks);
	std::cout << "Bishops:" << '\n';
	displayBB(this->bishops);
	std::cout << "Knights:" << '\n';
	displayBB(this->knights);
	std::cout << "Pawns:" << '\n';
	displayBB(this->pawns);
}

void Position::clearData() {
	this->history.clear();
	this->positionCounts.clear();
	incrementPositionCounter();
}

std::string concatFEN(const std::vector<std::string> strings) {
	std::string result = "";
	for (uint i = 0; i < strings.size(); i++) {
		result += strings[i] + " ";
	}
	return result;
}

MoveList::MoveList(Position& position) {
	position.getMoves(this->moves_index, this->moveSets);
}

Move MoveList::randomMove() const {
	uint vecIndex = std::rand() % (this->moves_index);
	uint movesIndex = std::rand() % (this->moveSets[vecIndex]->size());
	return (*this->moveSets[vecIndex])[movesIndex];
}

uint64_t MoveList::size() const {
	uint64_t count = 0;
	for (uint i = 0; i < this->moves_index; i++) {
		count += this->moveSets[i]->size();
	}
	return count;
}

bool MoveList::contains(const Move targetMove) const {
	for (uint i = 0; i < this->moves_index; i++) {
		for (Move move : *this->moveSets[i]) {
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
	return *this->ptr;
}

const Move* MoveList::Iterator::operator->() const {
	return this->ptr;
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
	return MoveList::Iterator(this->moves_index, 0, 0, this->moveSets, this->endMove);
}

MoveList::Iterator MoveList::end() {
	return MoveList::Iterator(-1, -1, -1, this->moveSets, this->endMove);
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

std::array<Hash, CASTLING_COMBOS> generateCastlingHash() {
	std::default_random_engine generator(272); // Fixed random seed
	std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
	std::array<Hash, CASTLING_COMBOS> hashes;
	for (uint i = 0; i < CASTLING_COMBOS; i++) {
		hashes[i] = distribution(generator);
	}
	return hashes;
}

std::array<Hash, FILE_COUNT> generateEnPassantHash() {
	std::default_random_engine generator(162); // Fixed random seed
	std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
	std::array<Hash, FILE_COUNT> hashes;
	for (uint i = 0; i < FILE_COUNT; i++) {
		hashes[i] = distribution(generator);
	}
	return hashes;
}