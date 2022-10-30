#include "evaluate.hpp"

template<>
float Evaluator::psqtEvaluation<OPENING>(Position& position) {
	float value = 0;
	return value;
}

template<>
float Evaluator::psqtEvaluation<MIDDLEGAME>(Position& position) {
	float value = 0;
	return value;
}

template<>
float Evaluator::psqtEvaluation<ENDGAME>(Position& position) {
	float value = 0;
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