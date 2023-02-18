#include "evaluate.hpp"

template<>
float Evaluator::psqtEvaluation<OPENING>(Position& position) {
	float value = 0;

	// White pieces
	for (uint index = 0; index < position.getPieceIndex<W_KING>(); index++)
		value +=  PSQT::Opening::KING[position.getPieceSquare<W_KING>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_QUEEN>(); index++)
		value +=  PSQT::Opening::QUEEN[position.getPieceSquare<W_QUEEN>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_ROOK>(); index++)
		value +=  PSQT::Opening::ROOK[position.getPieceSquare<W_ROOK>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_BISHOP>(); index++)
		value +=  PSQT::Opening::BISHOP[position.getPieceSquare<W_BISHOP>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_KNIGHT>(); index++)
		value +=  PSQT::Opening::KNIGHT[position.getPieceSquare<W_KNIGHT>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_PAWN>(); index++)
		value +=  PSQT::Opening::PAWN[position.getPieceSquare<W_PAWN>(index)];

	// Black pieces
	for (uint index = 0; index < position.getPieceIndex<B_KING>(); index++)
		value -=  PSQT::Opening::KING[mirror(position.getPieceSquare<B_KING>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_QUEEN>(); index++)
		value -=  PSQT::Opening::QUEEN[mirror(position.getPieceSquare<B_QUEEN>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_ROOK>(); index++)
		value -=  PSQT::Opening::ROOK[mirror(position.getPieceSquare<B_ROOK>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_BISHOP>(); index++)
		value -=  PSQT::Opening::BISHOP[mirror(position.getPieceSquare<B_BISHOP>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_KNIGHT>(); index++)
		value -=  PSQT::Opening::KNIGHT[mirror(position.getPieceSquare<B_KNIGHT>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_PAWN>(); index++)
		value -=  PSQT::Opening::PAWN[mirror(position.getPieceSquare<B_PAWN>(index))];

	return value;
}

template<>
float Evaluator::psqtEvaluation<MIDDLEGAME>(Position& position) {
	float value = 0;

	// White pieces
	for (uint index = 0; index < position.getPieceIndex<W_KING>(); index++)
		value +=  PSQT::MiddleGame::KING[position.getPieceSquare<W_KING>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_QUEEN>(); index++)
		value +=  PSQT::MiddleGame::QUEEN[position.getPieceSquare<W_QUEEN>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_ROOK>(); index++)
		value +=  PSQT::MiddleGame::ROOK[position.getPieceSquare<W_ROOK>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_BISHOP>(); index++)
		value +=  PSQT::MiddleGame::BISHOP[position.getPieceSquare<W_BISHOP>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_KNIGHT>(); index++)
		value +=  PSQT::MiddleGame::KNIGHT[position.getPieceSquare<W_KNIGHT>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_PAWN>(); index++)
		value +=  PSQT::MiddleGame::PAWN[position.getPieceSquare<W_PAWN>(index)];

	// Black pieces
	for (uint index = 0; index < position.getPieceIndex<B_KING>(); index++)
		value -=  PSQT::MiddleGame::KING[mirror(position.getPieceSquare<B_KING>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_QUEEN>(); index++)
		value -=  PSQT::MiddleGame::QUEEN[mirror(position.getPieceSquare<B_QUEEN>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_ROOK>(); index++)
		value -=  PSQT::MiddleGame::ROOK[mirror(position.getPieceSquare<B_ROOK>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_BISHOP>(); index++)
		value -=  PSQT::MiddleGame::BISHOP[mirror(position.getPieceSquare<B_BISHOP>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_KNIGHT>(); index++)
		value -=  PSQT::MiddleGame::KNIGHT[mirror(position.getPieceSquare<B_KNIGHT>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_PAWN>(); index++)
		value -=  PSQT::MiddleGame::PAWN[mirror(position.getPieceSquare<B_PAWN>(index))];

	return value;
}

template<>
float Evaluator::psqtEvaluation<ENDGAME>(Position& position) {
	float value = 0;

	// White pieces
	for (uint index = 0; index < position.getPieceIndex<W_KING>(); index++)
		value +=  PSQT::EndGame::KING[position.getPieceSquare<W_KING>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_QUEEN>(); index++)
		value +=  PSQT::EndGame::QUEEN[position.getPieceSquare<W_QUEEN>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_ROOK>(); index++)
		value +=  PSQT::EndGame::ROOK[position.getPieceSquare<W_ROOK>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_BISHOP>(); index++)
		value +=  PSQT::EndGame::BISHOP[position.getPieceSquare<W_BISHOP>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_KNIGHT>(); index++)
		value +=  PSQT::EndGame::KNIGHT[position.getPieceSquare<W_KNIGHT>(index)];
	for (uint index = 0; index < position.getPieceIndex<W_PAWN>(); index++)
		value +=  PSQT::EndGame::PAWN[position.getPieceSquare<W_PAWN>(index)];

	// Black pieces
	for (uint index = 0; index < position.getPieceIndex<B_KING>(); index++)
		value -=  PSQT::EndGame::KING[mirror(position.getPieceSquare<B_KING>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_QUEEN>(); index++)
		value -=  PSQT::EndGame::QUEEN[mirror(position.getPieceSquare<B_QUEEN>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_ROOK>(); index++)
		value -=  PSQT::EndGame::ROOK[mirror(position.getPieceSquare<B_ROOK>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_BISHOP>(); index++)
		value -=  PSQT::EndGame::BISHOP[mirror(position.getPieceSquare<B_BISHOP>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_KNIGHT>(); index++)
		value -=  PSQT::EndGame::KNIGHT[mirror(position.getPieceSquare<B_KNIGHT>(index))];
	for (uint index = 0; index < position.getPieceIndex<B_PAWN>(); index++)
		value -=  PSQT::EndGame::PAWN[mirror(position.getPieceSquare<B_PAWN>(index))];

	return value;
}

float Evaluator::psqtEvaluation(Position& position) {
	switch (Evaluator::getStage(position)) {
		case MIDDLEGAME:
			return psqtEvaluation<MIDDLEGAME>(position);
		case ENDGAME:
			return psqtEvaluation<ENDGAME>(position);
		default: // OPENING
			return psqtEvaluation<OPENING>(position);
	}
}

float Evaluator::getPieceEvaluation(Position& position) {
	float value = 0;

	// White piece values
	value += PieceValue::QUEEN * position.getPieceIndex<W_QUEEN>();
	value += PieceValue::ROOK * position.getPieceIndex<W_ROOK>();
	value += PieceValue::BISHOP * position.getPieceIndex<W_BISHOP>();
	value += PieceValue::KNIGHT * position.getPieceIndex<W_KNIGHT>();
	value += PieceValue::PAWN * position.getPieceIndex<W_PAWN>();

	// Black piece values
	value -= PieceValue::QUEEN * position.getPieceIndex<B_QUEEN>();
	value -= PieceValue::ROOK * position.getPieceIndex<B_ROOK>();
	value -= PieceValue::BISHOP * position.getPieceIndex<B_BISHOP>();
	value -= PieceValue::KNIGHT * position.getPieceIndex<B_KNIGHT>();
	value -= PieceValue::PAWN * position.getPieceIndex<B_PAWN>();

	return value;
}

GameStage Evaluator::getStage(Position& position) {
	if (position.getCastling() != 0) {
		return OPENING;
	} else if (position.getQueenCount() == 0 && position.getPieceCnt() <= 20) {
		return ENDGAME;
	} else {
		return MIDDLEGAME;
	}
}