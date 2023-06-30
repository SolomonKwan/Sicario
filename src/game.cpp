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

	// Auxiliary variable
	this->epHashed = false;
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

void Position::getMoves(MoveList& moves) {
	setCheckers();
	if (inDoubleCheck()) {
		getKingMoves(moves);
	} else if (inCheck()) {
		setPinAndCheckRayBitboards();
		getCheckMoves(moves);
	} else {
		setPinAndCheckRayBitboards();
		getNormalMoves(moves);
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
	zeroAndSetBit<Bitboard>(this->sides[this->turn], start(move), end(move));
	zeroBit<Bitboard>(this->sides[!this->turn], end(move));
	zeroBit<Bitboard>(this->pawns, start(move));
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
	zeroAndSetBit<Bitboard>(this->sides[this->turn], start(move), end(move));
	zeroBit<Bitboard>(this->pawns, start(move));
	addToPromotionBitboard(move);

	// Update piece_list and pieces
	this->turn == WHITE ? removePiece<W_PAWN>(start(move)) : removePiece<B_PAWN>(start(move));

	// Add the new piece
	addPiecePromotion(move);
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
	// Update piece hash
	this->hash ^= Hashes::PIECES[K][KS];
	this->hash ^= Hashes::PIECES[K][KE];
	this->hash ^= Hashes::PIECES[R][RS];
	this->hash ^= Hashes::PIECES[R][RE];

	// Update bitboards
	zeroAndSetBit<Bitboard>(this->sides[this->turn], KS, KE);
	zeroAndSetBit<Bitboard>(this->sides[this->turn], RS, RE);
	zeroAndSetBit<Bitboard>(this->kings, KS, KE);
	zeroAndSetBit<Bitboard>(this->rooks, RS, RE);

	// Update piece_list and pieces
	movePiece<K>(KS, KE);
	movePiece<R>(RS, RE);
}

template<>
void Position::makeMove<NORMAL, NON_CAPTURE>(const Move move) {
	// Update hash
	PieceType piece_moved = this->pieces[start(move)];
	this->hash ^= Hashes::PIECES[piece_moved][start(move)];
	this->hash ^= Hashes::PIECES[piece_moved][end(move)];

	// Update bitboards, pieces and piece_list.
	zeroAndSetBit<Bitboard>(this->sides[this->turn], start(move), end(move));
	movePieceAndUpdateBitboard(piece_moved, start(move), end(move));
}

template<>
void Position::makeMove<NORMAL, CAPTURE>(const Move move) {
	// Update hash
	PieceType piece_captured = this->pieces[end(move)];
	PieceType piece_moved = this->pieces[start(move)];
	this->hash ^= Hashes::PIECES[piece_moved][start(move)];
	this->hash ^= Hashes::PIECES[piece_moved][end(move)];
	this->hash ^= Hashes::PIECES[piece_captured][end(move)];

	// Update pieces, piece_list and bitboard.
	removePiece(end(move), piece_captured);
	zeroAndSetBit<Bitboard>(this->sides[this->turn], start(move), end(move));
	zeroBit<Bitboard>(this->sides[!this->turn], end(move));
	updatePieceCounts(piece_captured, move);
	movePieceAndUpdateBitboard(piece_moved, start(move), end(move));
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
	// Update hash
	Square takenPawnSquare = end(move) + (this->turn == WHITE ? S : N);
	this->hash ^= Hashes::PIECES[getPieceType<PAWN>()][start(move)];
	this->hash ^= Hashes::PIECES[getPieceType<PAWN>()][end(move)];
	this->hash ^= Hashes::PIECES[getPieceType<PAWN>(true)][takenPawnSquare];

	// Update bitboards
	zeroAndSetBit<Bitboard>(this->sides[this->turn], start(move), end(move));
	zeroBit<Bitboard>(this->sides[!this->turn], takenPawnSquare);
	zeroAndSetBit<Bitboard>(this->pawns, start(move), end(move));
	zeroBit<Bitboard>(this->pawns, takenPawnSquare);

	// Update piece positions
	this->turn == WHITE ? movePiece<W_PAWN>(start(move), end(move)) : movePiece<B_PAWN>(start(move), end(move));
	this->turn == WHITE ? removePiece<B_PAWN>(takenPawnSquare) : removePiece<W_PAWN>(takenPawnSquare);

	// Update piece count
	this->piece_cnt--;
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
}

void Position::makeMove(const Move move, const bool hash) {
	if (move == NULL_MOVE) return;
	saveHistory(move);
	updateEnPassant(move);
	updateCastling(start(move), end(move));
	updateHalfmove(move);

	PieceType piece_captured = this->pieces[end(move)];
	switch (type(move)) {
		case NORMAL:
			piece_captured == NO_PIECE ? makeMove<NORMAL, NON_CAPTURE>(move) : makeMove<NORMAL, CAPTURE>(move);
			break;
		case CASTLING:
			makeMove<CASTLING, NON_CAPTURE>(move);
			break;
		case EN_PASSANT:
			makeMove<EN_PASSANT, CAPTURE>(move);
			break;
		default:
			piece_captured == NO_PIECE ? makeMove<PROMOTION, NON_CAPTURE>(move) : makeMove<PROMOTION, CAPTURE>(move);
			break;
	}

	updateFullmove();
	updateTurn();

	if (!hash) return;
	updateEnPassantHash();
	incrementPositionCounter();
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
	if (this->turn == BLACK)
		this->fullmove--;

	// Change pieces bitboards, piece list and indices and piece counts
	zeroAndSetBit<Bitboard>(this->sides[this->turn], end(prev.move), start(prev.move));
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
	if (this->turn == BLACK)
		this->fullmove--;

	// Remove promoted piece
	PieceType promoted = this->pieces[end(prev.move)];
	removePiece(end(prev.move), promoted);
	zeroBit<Bitboard>(this->sides[this->turn], end(prev.move));
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
	setBit<Bitboard>(this->sides[this->turn], start(prev.move));
	setBit<Bitboard>(this->pawns, start(prev.move));
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
	if (this->turn == BLACK)
		this->fullmove--;

	// Change bitboards
	Square captured_sq = end(prev.move) + (this->turn == WHITE ? S : N);
	zeroAndSetBit<Bitboard>(this->sides[this->turn], end(prev.move), start(prev.move));
	setBit<Bitboard>(this->sides[!this->turn], captured_sq);
	zeroAndSetBit<Bitboard>(this->pawns, end(prev.move), start(prev.move));
	setBit<Bitboard>(this->pawns, captured_sq);

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
	if (this->turn == BLACK)
		this->fullmove--;

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

	// Change bitboards
	zeroAndSetBit<Bitboard>(this->sides[this->turn], end(prev.move), start(prev.move));
	zeroAndSetBit<Bitboard>(this->sides[this->turn], rook_end, rook_start);
	zeroAndSetBit<Bitboard>(this->kings, end(prev.move), start(prev.move));
	zeroAndSetBit<Bitboard>(this->rooks, rook_end, rook_start);

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
	return this->castling & (1U << WKSC);
}

template <>
bool Position::castleBit<WQSC>() const {
	return this->castling & (1U << WQSC);
}

template <>
bool Position::castleBit<BKSC>() const {
	return this->castling & (1U << BKSC);
}

template <>
bool Position::castleBit<BQSC>() const {
	return this->castling & (1U << BQSC);
}

void Position::updateTurn() {
	this->turn = !this->turn;
	this->hash ^= Hashes::TURN;
}

void Position::display(const bool letterMode) const {
	// Print the pieces
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
	uint prevCastling = this->castling;

	// Update appropriate castling bits.
	if (start == E1 && (this->castleBit<WKSC>() || this->castleBit<WQSC>())) {
		this->castling &= ~WHITE_CASTLING;
	} else if (start == E8 && (this->castleBit<BKSC>() || this->castleBit<BQSC>())) {
		this->castling &= ~BLACK_CASTLING;
	} else {
		if (start == H1 || end == H1) zeroBit<uint>(this->castling, WKSC);
		if (start == A1 || end == A1) zeroBit<uint>(this->castling, WQSC);
		if (start == H8 || end == H8) zeroBit<uint>(this->castling, BKSC);
		if (start == A8 || end == A8) zeroBit<uint>(this->castling, BQSC);
	}

	// Update hash.
	if (prevCastling != this->castling) {
		this->hash ^= Hashes::CASTLING[prevCastling];
		this->hash ^= Hashes::CASTLING[this->castling];
	}
}

void Position::updateEnPassant(const Move move) {
	// Update existing en passant.
	if (this->en_passant != NONE && this->epHashed)
		this->hash ^= Hashes::EN_PASSANT[file(this->en_passant)];
	this->en_passant = NONE;
	this->epHashed = false;

	// Update to new en passant.
	bool pawnMove = this->pieces[start(move)] == W_PAWN || this->pieces[start(move)] == B_PAWN;
	bool doublePush = std::abs(rank(start(move)) - rank(end(move))) == DOUBLE_PAWN_PUSH;
	if (pawnMove && doublePush)
		this->en_passant = end(move) + (this->turn == WHITE ? S : N);
}

void Position::updateEnPassantHash() {
	if (this->en_passant == NONE) return;

	setPinAndCheckRayBitboards();
	MoveList moves = MoveList(*this);
	getEnPassantMoves(moves);
	if (moves.moves_index != 0) return;
	this->hash ^= Hashes::EN_PASSANT[file(this->en_passant)];
	this->epHashed = true;
}

void Position::updateHalfmove(const Move move) {
	bool pawnMove = this->pieces[start(move)] == W_PAWN || this->pieces[start(move)] == B_PAWN;
	bool captureMove = this->pieces[end(move)] != NO_PIECE;
	if (pawnMove || captureMove)
		this->halfmove = 0;
	else
		this->halfmove++;
}

void Position::updateFullmove() {
	if (this->turn == BLACK)
		this->fullmove++;
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
			setBit<Bitboard>(this->kings, square);
			break;
		case W_QUEEN:
		case B_QUEEN:
			this->turn == WHITE ? addPiece<B_QUEEN>(square) : addPiece<W_QUEEN>(square);
			setBit<Bitboard>(this->queens, square);
			break;
		case W_ROOK:
		case B_ROOK:
			this->turn == WHITE ? addPiece<B_ROOK>(square) : addPiece<W_ROOK>(square);
			setBit<Bitboard>(this->rooks, square);
			break;
		case W_BISHOP:
		case B_BISHOP:
			this->turn == WHITE ? addPiece<B_BISHOP>(square) : addPiece<W_BISHOP>(square);
			setBit<Bitboard>(this->bishops, square);
			break;
		case W_KNIGHT:
		case B_KNIGHT:
			this->turn == WHITE ? addPiece<B_KNIGHT>(square) : addPiece<W_KNIGHT>(square);
			setBit<Bitboard>(this->knights, square);
			break;
		case W_PAWN:
		case B_PAWN:
			this->turn == WHITE ? addPiece<B_PAWN>(square) : addPiece<W_PAWN>(square);
			setBit<Bitboard>(this->pawns, square);
			break;
		default:
			std::cerr << "This shouldn't be happening... " << piece << '\n';
			assert(false);
	}
}

void Position::movePieceAndUpdateBitboard(const PieceType piece, const Square start, const Square end) {
	switch (piece) {
		case W_KING:
		case B_KING:
			zeroAndSetBit<Bitboard>(this->kings, start, end);
			this->turn == WHITE ? movePiece<W_KING>(start, end) : movePiece<B_KING>(start, end);
			break;
		case W_QUEEN:
		case B_QUEEN:
			this->turn == WHITE ? movePiece<W_QUEEN>(start, end) : movePiece<B_QUEEN>(start, end);
			zeroAndSetBit<Bitboard>(this->queens, start, end);
			break;
		case W_ROOK:
		case B_ROOK:
			this->turn == WHITE ? movePiece<W_ROOK>(start, end) : movePiece<B_ROOK>(start, end);
			zeroAndSetBit<Bitboard>(this->rooks, start, end);
			break;
		case W_BISHOP:
		case B_BISHOP:
			this->turn == WHITE ? movePiece<W_BISHOP>(start, end) : movePiece<B_BISHOP>(start, end);
			zeroAndSetBit<Bitboard>(this->bishops, start, end);
			break;
		case W_KNIGHT:
		case B_KNIGHT:
			this->turn == WHITE ? movePiece<W_KNIGHT>(start, end) : movePiece<B_KNIGHT>(start, end);
			zeroAndSetBit<Bitboard>(this->knights, start, end);
			break;
		case W_PAWN:
		case B_PAWN:
			this->turn == WHITE ? movePiece<W_PAWN>(start, end) : movePiece<B_PAWN>(start, end);
			zeroAndSetBit<Bitboard>(this->pawns, start, end);
			break;
		default:
			std::cerr << "This shouldn't be happening... " << piece << '\n';
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
			setBit<Bitboard>(this->queens, end(move));
			break;
		case pROOK:
			setBit<Bitboard>(this->rooks, end(move));
			break;
		case pBISHOP:
			setBit<Bitboard>(this->bishops, end(move));
			break;
		case pKNIGHT:
			setBit<Bitboard>(this->knights, end(move));
			break;
		default:
			assert(false);
	}
}

void Position::removeFromBitboardPromotion(const Move move) {
	switch (this->pieces[end(move)]) {
		case W_QUEEN:
		case B_QUEEN:
			zeroBit<Bitboard>(this->queens, end(move));
			break;
		case W_ROOK:
		case B_ROOK:
			zeroBit<Bitboard>(this->rooks, end(move));
			break;
		case W_BISHOP:
		case B_BISHOP:
			zeroBit<Bitboard>(this->bishops, end(move));
			break;
		case W_KNIGHT:
		case B_KNIGHT:
			zeroBit<Bitboard>(this->knights, end(move));
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

void Position::getKingMoves(MoveList& moves) const {
	Bitboard reachBB = Masks::KING[getKingSquare()] & ~this->sides[this->turn];

	if (reachBB == ZERO_BB) return;
	for (Square square : KingReach::SQUARES[getKingSquare()][getKingMovesIndex(reachBB, getKingSquare())]) {
		if (!isAttacked(square, (Player) !this->turn, true)) continue;
		reachBB &= ~(ONE_BB << square);
	}

	if (reachBB == ZERO_BB) return;
	moves.addMoves(&Moves::KING[getKingSquare()][getKingMovesIndex(reachBB, getKingSquare())]);
}

void Position::getCheckMoves(MoveList& moves) const {
	getKingMoves(moves);
	getQueenCheckedMoves(moves);
	getRookCheckedMoves(moves);
	getBishopCheckedMoves(moves);
	getKnightCheckedMoves(moves);
	getPawnCheckedMoves(moves);
	getEnPassantMoves(moves);
}

void Position::getQueenCheckedMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<QUEEN>()]; i++) {
		Square queenSquare = this->piece_list[getPieceType<QUEEN>()][i];
		if (isPinned(queenSquare)) continue;

		Bitboard reach = getRookReachBB(Masks::ROOK[queenSquare] & getPieces(), queenSquare) & this->check_rays;
		if (reach != ZERO_BB)
			moves.addMoves(&Moves::Blocks::ROOK[queenSquare][getRookBlockIndex(reach, queenSquare)]);

		reach = getBishopReachBB(Masks::BISHOP[queenSquare] & getPieces(), queenSquare) & this->check_rays;
		if (reach != ZERO_BB)
			moves.addMoves(&Moves::Blocks::BISHOP[queenSquare][getBishopBlockIndex(reach, queenSquare)]);
	}
}

void Position::getRookCheckedMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<ROOK>()]; i++) {
		Square rookSquare = this->piece_list[getPieceType<ROOK>()][i];
		if (isPinned(rookSquare)) continue;

		Bitboard reach = getRookReachBB(Masks::ROOK[rookSquare] & getPieces(), rookSquare) & this->check_rays;
		if (reach != ZERO_BB)
			moves.addMoves(&Moves::Blocks::ROOK[rookSquare][getRookBlockIndex(reach, rookSquare)]);
	}
}

void Position::getBishopCheckedMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<BISHOP>()]; i++) {
		Square bishopSquare = this->piece_list[getPieceType<BISHOP>()][i];
		if (isPinned(bishopSquare)) continue;

		Bitboard reach = getBishopReachBB(Masks::BISHOP[bishopSquare] & getPieces(), bishopSquare) & this->check_rays;
		if (reach != ZERO_BB)
			moves.addMoves(&Moves::Blocks::BISHOP[bishopSquare][getBishopBlockIndex(reach, bishopSquare)]);
	}
}

void Position::getKnightCheckedMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<KNIGHT>()]; i++) {
		Square knightSquare = this->piece_list[getPieceType<KNIGHT>()][i];
		if (isPinned(knightSquare)) continue;

		Bitboard reach = Masks::KNIGHT[knightSquare] & this->check_rays;
		if (reach != ZERO_BB)
			moves.addMoves(&Moves::KNIGHT[knightSquare][getKnightMovesIndex(reach, knightSquare)]);
	}
}

void Position::getPawnCheckedMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<PAWN>()]; i++) {
		Square pawnSquare = this->piece_list[getPieceType<PAWN>()][i];
		if (isPinned(pawnSquare)) continue;

		Bitboard reach = ZERO_BB;
		Bitboard checkRaysOnly = this->check_rays & ~this->sides[!this->turn];
		Bitboard checkersOnly = this->check_rays & this->sides[!this->turn];
		Square pawnPush = pawnSquare + (this->turn == WHITE ? N : S);
		Square pawnDoublePush = pawnSquare + (this->turn == WHITE ? NN : SS);
		bool emptyPush = !isOccupied(pawnPush);
		bool emptyDoublePush = !isOccupied(pawnDoublePush);
		bool onStartRank = rank(pawnSquare) == (this->turn == WHITE ? RANK_2 : RANK_7);
		bool pawnPushBlock = (ONE_BB << pawnPush) & checkRaysOnly;
		bool pawnDoublePushBlock = (ONE_BB << pawnDoublePush) & checkRaysOnly;

		// Pawn pushes to block check.
		if (emptyPush && pawnPushBlock)
			reach |= ONE_BB << pawnPush;
		else if (onStartRank && emptyPush && emptyDoublePush && pawnDoublePushBlock)
			reach |= ONE_BB << pawnDoublePush;

		// Capture checking pieces.
		reach |= Masks::PAWN[this->turn][pawnSquare] & ~Masks::FILE[file(pawnSquare)] & checkersOnly;

		if (reach != ZERO_BB)
			moves.addMoves(&Moves::PAWN[this->turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, this->turn)]);
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
	setPinAndCheckRayBitboards();
	MoveList moves = MoveList(*this);
	getEnPassantMoves(moves);
	if (this->en_passant != NONE && moves.moves_index != 0) {
		this->hash ^= Hashes::EN_PASSANT[file(this->en_passant)];
		this->epHashed = true;
	}
}

void Position::getNormalMoves(MoveList& moves) const {
	this->getKingMoves(moves);
	this->getQueenMoves(moves);
	this->getRookMoves(moves);
	this->getBishopMoves(moves);
	this->getKnightMoves(moves);
	this->getPawnMoves(moves);
	this->getCastlingMoves(moves);
	this->getEnPassantMoves(moves);
}

void Position::getQueenMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<QUEEN>()]; i++) {
		Square queenSquare = this->piece_list[getPieceType<QUEEN>()][i];
		if (isPinnedByBishop(queenSquare)) {
			getBishopPinMoves(moves, queenSquare);
		} else if (isPinnedByRook(queenSquare)) {
			getRookPinMoves(moves, queenSquare);
		} else {
			Bitboard reach = getBishopReachBB(Masks::BISHOP[queenSquare] & getPieces(), queenSquare);
			reach &= ~this->sides[this->turn];
			if (reach != ZERO_BB)
				moves.addMoves(&Moves::BISHOP[queenSquare][getBishopMovesIndex(reach, queenSquare)]);

			reach = getRookReachBB(Masks::ROOK[queenSquare] & getPieces(), queenSquare);
			reach &= ~this->sides[this->turn];
			if (reach != ZERO_BB)
				moves.addMoves(&Moves::ROOK[queenSquare][getRookMovesIndex(reach, queenSquare)]);
		}
	}
}

void Position::getRookMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<ROOK>()]; i++) {
		Square rookSquare = this->piece_list[getPieceType<ROOK>()][i];
		if (isPinnedByRook(rookSquare)) {
			getRookPinMoves(moves, rookSquare);
		} else if (!isPinnedByBishop(rookSquare)) {
			Bitboard reach = getRookReachBB(Masks::ROOK[rookSquare] & getPieces(), rookSquare);
			reach &= ~this->sides[this->turn];
			if (reach != ZERO_BB)
				moves.addMoves(&Moves::ROOK[rookSquare][getRookMovesIndex(reach, rookSquare)]);
		} else continue;
	}
}

void Position::getBishopMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<BISHOP>()]; i++) {
		Square bishopSquare = this->piece_list[getPieceType<BISHOP>()][i];
		if (isPinnedByBishop(bishopSquare)) {
			getBishopPinMoves(moves, bishopSquare);
		} else if (!isPinnedByRook(bishopSquare)) {
			Bitboard reach = getBishopReachBB(Masks::BISHOP[bishopSquare] & getPieces(), bishopSquare);
			reach &= ~this->sides[this->turn];
			if (reach != ZERO_BB)
				moves.addMoves(&Moves::BISHOP[bishopSquare][getBishopMovesIndex(reach, bishopSquare)]);
		} else continue;
	}
}

void Position::getKnightMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<KNIGHT>()]; i++) {
		Square knightSquare = this->piece_list[getPieceType<KNIGHT>()][i];
		if (isPinnedByBishop(knightSquare) || isPinnedByRook(knightSquare)) continue;

		Bitboard reach = Masks::KNIGHT[knightSquare] & ~this->sides[this->turn];
		if (reach != ZERO_BB)
			moves.addMoves(&Moves::KNIGHT[knightSquare][getKnightMovesIndex(reach, knightSquare)]);
	}
}

void Position::getPawnMoves(MoveList& moves) const {
	for (uint i = 0; i < this->piece_index[getPieceType<PAWN>()]; i++) {
		Square pawnSquare = this->piece_list[getPieceType<PAWN>()][i];
		if (isPinnedByBishop(pawnSquare)) {
			Bitboard reach = Masks::PAWN[this->turn][pawnSquare];
			reach &= ~Masks::FILE[file(pawnSquare)];
			reach &= this->bishop_pins;
			reach &= this->sides[!this->turn];
			reach &= (this->queens | this->bishops);
			if (reach != ZERO_BB)
				moves.addMoves(&Moves::PAWN[this->turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, this->turn)]);
		} else if (!isPawnPinnedByRookHorizontally(pawnSquare)) {
			// Can't capture if vertically pinned to king.
			Bitboard reach = ZERO_BB;
			if (!isPawnPinnedByRookVertically(pawnSquare)) {
				reach |= Masks::PAWN[this->turn][pawnSquare];
				reach &= ~Masks::FILE[file(pawnSquare)];
				reach &= this->sides[!this->turn];
			}

			// Single push
			Direction push = this->turn == WHITE ? N : S;
			bool emptyPush = !isSet<Bitboard>(getPieces(), pawnSquare + push);
			if (emptyPush)
				setBit<Bitboard>(reach, pawnSquare + push);

			// Double push
			Rank startRank = this->turn == WHITE ? RANK_2 : RANK_7;
			bool onStartRank = rank(pawnSquare) == startRank;
			bool emptyDoublePush = !isSet<Bitboard>(getPieces(), pawnSquare + push + push);
			if (onStartRank && emptyPush && emptyDoublePush)
				setBit<Bitboard>(reach, pawnSquare + push + push);

			if (reach != ZERO_BB)
				moves.addMoves(&Moves::PAWN[this->turn][pawnSquare][getPawnMovesIndex(reach, pawnSquare, this->turn)]);
		} else continue; // Can't move if horizontally pinned to king.
	}
}

inline Bitboard Position::getRookReachBB(const Bitboard occupancy, const Square square) const {
	int index = Indices::ROOK[square][getRookReachIndex(occupancy, square)];
	return Reach::ROOK[square][index];
}

inline Bitboard Position::getBishopReachBB(const Bitboard occupancy, const Square square) const {
	int index = Indices::BISHOP[square][getBishopReachIndex(occupancy, square)];
	return Reach::BISHOP[square][index];
}

void Position::getBishopPinMoves(MoveList& moves, const Square square) const {
	Bitboard reach = this->bishop_pins;
	reach &= getBishopReachBB(Masks::BISHOP[square] & getPieces(), square);
	zeroBit<Bitboard>(reach, square);
	if (reach != ZERO_BB)
		moves.addMoves(&Moves::BISHOP[square][getBishopMovesIndex(reach, square)]);
}

void Position::getRookPinMoves(MoveList& moves, const Square square) const {
	Bitboard reach = this->rook_pins;
	reach &= getRookReachBB(Masks::ROOK[square] & getPieces(), square);
	zeroBit<Bitboard>(reach, square);
	if (reach != ZERO_BB)
		moves.addMoves(&Moves::ROOK[square][getRookMovesIndex(reach, square)]);
}

Bitboard Position::isOccupied(const Square square) const {
	return this->pieces[square] != NO_PIECE;
}

void Position::getCastlingMoves(MoveList& moves) const {
	if (this->turn == WHITE && this->validWKSC()) moves.addMoves(&Moves::CASTLING[WKSC]);
	if (this->turn == WHITE && this->validWQSC()) moves.addMoves(&Moves::CASTLING[WQSC]);
	if (this->turn == BLACK && this->validBKSC()) moves.addMoves(&Moves::CASTLING[BKSC]);
	if (this->turn == BLACK && this->validBQSC()) moves.addMoves(&Moves::CASTLING[BQSC]);
}

bool Position::validWKSC() const {
	bool f1Empty = !this->isOccupied(F1);
	bool g1Empty = !this->isOccupied(G1);
	bool f1NotAttacked = !this->isAttacked(F1, !this->turn);
	bool g1NotAttacked = !this->isAttacked(G1, !this->turn);
	return this->castleBit<WKSC>() && f1Empty && g1Empty && f1NotAttacked && g1NotAttacked;
}

bool Position::validWQSC() const {
	bool d1Empty = !this->isOccupied(D1);
	bool c1Empty = !this->isOccupied(C1);
	bool b1Empty = !this->isOccupied(B1);
	bool d1NotAttacked = !this->isAttacked(D1, !this->turn);
	bool c1NotAttacked = !this->isAttacked(C1, !this->turn);
	return this->castleBit<WQSC>() && d1Empty && c1Empty && b1Empty && d1NotAttacked && c1NotAttacked;
}

bool Position::validBKSC() const {
	bool f8Empty = !this->isOccupied(F8);
	bool g8Empty = !this->isOccupied(G8);
	bool f8NotAttacked = !this->isAttacked(F8, !this->turn);
	bool g8NotAttacked = !this->isAttacked(G8, !this->turn);
	return this->castleBit<BKSC>() && f8Empty && g8Empty && f8NotAttacked && g8NotAttacked;
}

bool Position::validBQSC() const {
	bool d8Empty = !this->isOccupied(D8);
	bool c8Empty = !this->isOccupied(C8);
	bool b8Empty = !this->isOccupied(B8);
	bool d8NotAttacked = !this->isAttacked(D8, !this->turn);
	bool c8NotAttacked = !this->isAttacked(C8, !this->turn);
	return this->castleBit<BQSC>() && d8Empty && c8Empty && b8Empty && d8NotAttacked && c8NotAttacked;
}

void Position::getEnPassantMoves(MoveList& moves) const {
	if (!this->en_passant) return;

	std::vector<Square> pawnSquares;
	if (file(this->en_passant) != FILE_A) pawnSquares.push_back(this->en_passant + (this->turn == WHITE ? SW : NW));
	if (file(this->en_passant) != FILE_H) pawnSquares.push_back(this->en_passant + (this->turn == WHITE ? SE : NE));

	for (Square square : pawnSquares) {
		if (!validEnPassantMove(square)) continue;
		int rightCapture = file(square) < file(this->en_passant) ? 0 : 1;
		moves.addMoves(&Moves::EN_PASSANT[this->turn][file(this->en_passant)][rightCapture]);
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
	Bitboard queensAndRooks = this->queens | this->rooks;
	Bitboard queensAndBishops = this->queens | this->bishops;
	Bitboard pieces = getPieces() ^ (ignoreKing ? ONE_BB << this->piece_list[!player][0] : ZERO_BB);

	Bitboard kingBB = Masks::KING[square] & this->sides[player] & this->kings;
	Bitboard rooksBB = getRookReachBB(Masks::ROOK[square] & pieces, square) & this->sides[player] & queensAndRooks;
	Bitboard bishopsBB = getBishopReachBB(Masks::BISHOP[square] & pieces, square) & this->sides[player] & queensAndBishops;
	Bitboard knightsBB = Masks::KNIGHT[square] & this->sides[player] & this->knights;
	Bitboard pawnsBB = Masks::PAWN[!player][square] & ~Masks::FILE[file(square)] & this->sides[player] & this->pawns;
	return kingBB | rooksBB | bishopsBB | knightsBB | pawnsBB;
}

void Position::removePiece(const Square square, const PieceType piece_captured) {
	switch (piece_captured) {
		case W_QUEEN:
			zeroBit<Bitboard>(this->queens, square);
			this->removePiece<W_QUEEN>(square);
			break;
		case B_QUEEN:
			zeroBit<Bitboard>(this->queens, square);
			this->removePiece<B_QUEEN>(square);
			break;
		case W_ROOK:
			zeroBit<Bitboard>(this->rooks, square);
			this->removePiece<W_ROOK>(square);
			break;
		case B_ROOK:
			zeroBit<Bitboard>(this->rooks, square);
			this->removePiece<B_ROOK>(square);
			break;
		case W_BISHOP:
			zeroBit<Bitboard>(this->bishops, square);
			this->removePiece<W_BISHOP>(square);
			break;
		case B_BISHOP:
			zeroBit<Bitboard>(this->bishops, square);
			this->removePiece<B_BISHOP>(square);
			break;
		case W_KNIGHT:
			zeroBit<Bitboard>(this->knights, square);
			this->removePiece<W_KNIGHT>(square);
			break;
		case B_KNIGHT:
			zeroBit<Bitboard>(this->knights, square);
			this->removePiece<B_KNIGHT>(square);
			break;
		case W_PAWN:
			zeroBit<Bitboard>(this->pawns, square);
			this->removePiece<W_PAWN>(square);
			break;
		case B_PAWN:
			zeroBit<Bitboard>(this->pawns, square);
			this->removePiece<B_PAWN>(square);
			break;
		default:
			std::cerr << "This shouldn't be happening... " << piece_captured << '\n';
			assert(false);
	}
}

void Position::decrementPositionCounter(const Hash hash) {
	auto it = this->positionCounts.find(hash);
	if (it != this->positionCounts.end()) {
		this->positionCounts[hash]--;
		if (this->positionCounts[hash] == 0)
			this->positionCounts.erase(it);
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
		if (string[4] == 'q')
			move |= pQUEEN;
		else if (string[4] == 'r')
			move |= pROOK;
		else if (string[4] == 'b')
			move |= pBISHOP;
		else
			move |= pKNIGHT;
	}

	// Check if en passant move.
	bool pawnMove = this->pieces[start] == W_PAWN || this->pieces[start] == B_PAWN;
	bool endOnEnPassantSquare = this->en_passant == end;
	if (endOnEnPassantSquare && pawnMove)
		move |= EN_PASSANT;

	// Check if castling move.
	bool kingMove = this->pieces[start] == W_KING || this->pieces[start] == B_KING;
	bool twoSquareMove = std::abs(start_file - end_file) == 2;
	if (twoSquareMove && kingMove)
		move |= CASTLING;

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
			return letterMode ? "  " : isDark(square) ? darkSquare(" ") : lightSquare(" ");
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

bool Position::validEnPassantMove(const Square square) const {
	Square capturedPawn = this->en_passant + (this->turn == WHITE ? S : N);
	bool attackPawn = this->pieces[square] == this->getPieceType<PAWN>();
	bool bishopPinned = this->isPinnedByBishop(square);
	bool enPassantPinned = this->isPinnedByBishop(this->en_passant);
	bool verPinned = this->isPinnedByRook(square);
	bool horPinned = oneBitSet(this->getPieces() & this->rook_ep_pins & ~(ONE_BB << square | ONE_BB << capturedPawn));
	return attackPawn && !horPinned && !verPinned && (!bishopPinned || (bishopPinned && enPassantPinned));
}

MoveList::MoveList(Position& position) {
	position.getMoves(*this);
}

Move MoveList::randomMove() const {
	uint vecIndex = std::rand() % (this->moves_index);
	uint movesIndex = std::rand() % (this->moveSets[vecIndex]->size());
	return (*this->moveSets[vecIndex])[movesIndex];
}

uint64_t MoveList::size() const {
	uint64_t count = 0;
	for (uint i = 0; i < this->moves_index; i++)
		count += this->moveSets[i]->size();
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

void MoveList::addMoves(const MoveSet& moveset) {
	this->moveSets[this->moves_index++] = moveset;
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

	if (this->i != this->vecCnt)
		this->ptr = &(*this->moves[this->i])[this->j];
	else // Point to end of iterator
		this->ptr = this->endAddr;
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
	for (uint i = 0; i < CASTLING_COMBOS; i++)
		hashes[i] = distribution(generator);
	return hashes;
}

std::array<Hash, FILE_COUNT> generateEnPassantHash() {
	std::default_random_engine generator(162); // Fixed random seed
	std::uniform_int_distribution<uint64_t> distribution(0, MAX_BB);
	std::array<Hash, FILE_COUNT> hashes;
	for (uint i = 0; i < FILE_COUNT; i++)
		hashes[i] = distribution(generator);
	return hashes;
}