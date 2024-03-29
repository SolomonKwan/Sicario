
#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include "constants.hpp"
#include "game.hpp"

namespace PieceValue {
	const float PAWN = 1;
	const float KNIGHT = 3;
	const float BISHOP = 3.25;
	const float ROOK = 5;
	const float QUEEN = 9;
}

enum GameStage {
	OPENING, MIDDLEGAME, ENDGAME
};

namespace PSQT::Opening {
	const float KING[SQUARE_COUNT] = {
		 20,  30,  10,   0,   0,  10,  30,  20,
		 20,  20,   0,   0,   0,   0,  20,  20,
		-10, -20, -20, -20, -20, -20, -20, -10,
		-20, -30, -30, -40, -40, -30, -30, -20,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30
	};

	const float QUEEN[SQUARE_COUNT] = {
		-20, -10, -10, -5, -5, -10, -10, -20,
		-10,   0,   5,  0,  0,   0,   0, -10,
		-10,   5,   5,  5,  5,   5,   0, -10,
		  0,   0,   5,  5,  5,   5,   0,  -5,
		 -5,   0,   5,  5,  5,   5,   0,  -5,
		-10,   0,   5,  5,  5,   5,   0, -10,
		-10,   0,   0,  0,  0,   0,   0, -10,
		-20, -10, -10, -5, -5, -10, -10, -20
	};

	const float ROOK[SQUARE_COUNT] = {
		 0,  0,  0,  5,  5,  0,  0,  0
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		 5, 10, 10, 10, 10, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	const float BISHOP[SQUARE_COUNT] = {
		-20, -10, -10, -10, -10, -10, -10, -20,
		-10,   5,   0,   0,   0,   0,   5, -10,
		-10,  10,  10,  10,  10,  10,  10, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   5,   5,  10,  10,   5,   5, -10,
		-10,   0,   5,  10,  10,   5,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-20, -10, -10, -10, -10, -10, -10, -20
	};

	const float KNIGHT[SQUARE_COUNT] = {
		-50, -40, -30, -30, -30, -30, -40, -50,
		-40, -20,   0,   5,   5,   0, -20, -40,
		-30,   5,  10,  15,  15,  10,   5, -30,
		-30,   0,  15,  20,  20,  15,   0, -30,
		-30,   5,  15,  20,  20,  15,   5, -30,
		-30,   0,  10,  15,  15,  10,   0, -30,
		-40, -20,   0,   0,   0,   0, -20, -40,
		-50, -40, -30, -30, -30, -30, -40, -50
	};

	const float PAWN[SQUARE_COUNT] = {
		 0,  0,   0,   0,   0,   0,  0,  0,
		 5, 10,  10, -20, -20,  10, 10,  5,
		 5, -5, -10,   0,   0, -10, -5,  5,
		 0,  0,   0,  20,  20,   0,  0,  0,
		 5,  5,  10,  25,  25,  10,  5,  5,
		10, 10,  20,  30,  30,  20, 10, 10,
		50, 50,  50,  50,  50,  50, 50, 50,
		 0,  0,   0,   0,   0,   0,  0,  0
	};
}

namespace PSQT::MiddleGame {
	const float KING[SQUARE_COUNT] = {
		 20,  30,  10,   0,   0,  10,  30,  20,
		 20,  20,   0,   0,   0,   0,  20,  20,
		-10, -20, -20, -20, -20, -20, -20, -10,
		-20, -30, -30, -40, -40, -30, -30, -20,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30
	};

	const float QUEEN[SQUARE_COUNT] = {
		-20, -10, -10, -5, -5, -10, -10, -20,
		-10,   0,   5,  0,  0,   0,   0, -10,
		-10,   5,   5,  5,  5,   5,   0, -10,
		  0,   0,   5,  5,  5,   5,   0,  -5,
		 -5,   0,   5,  5,  5,   5,   0,  -5,
		-10,   0,   5,  5,  5,   5,   0, -10,
		-10,   0,   0,  0,  0,   0,   0, -10,
		-20, -10, -10, -5, -5, -10, -10, -20
	};

	const float ROOK[SQUARE_COUNT] = {
		 0,  0,  0,  5,  5,  0,  0,  0
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		 5, 10, 10, 10, 10, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	const float BISHOP[SQUARE_COUNT] = {
		-20, -10, -10, -10, -10, -10, -10, -20,
		-10,   5,   0,   0,   0,   0,   5, -10,
		-10,  10,  10,  10,  10,  10,  10, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   5,   5,  10,  10,   5,   5, -10,
		-10,   0,   5,  10,  10,   5,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-20, -10, -10, -10, -10, -10, -10, -20
	};

	const float KNIGHT[SQUARE_COUNT] = {
		-50, -40, -30, -30, -30, -30, -40, -50,
		-40, -20,   0,   5,   5,   0, -20, -40,
		-30,   5,  10,  15,  15,  10,   5, -30,
		-30,   0,  15,  20,  20,  15,   0, -30,
		-30,   5,  15,  20,  20,  15,   5, -30,
		-30,   0,  10,  15,  15,  10,   0, -30,
		-40, -20,   0,   0,   0,   0, -20, -40,
		-50, -40, -30, -30, -30, -30, -40, -50
	};

	const float PAWN[SQUARE_COUNT] = {
		 0,  0,   0,   0,   0,   0,  0,  0,
		 5, 10,  10, -20, -20,  10, 10,  5,
		 5, -5, -10,   0,   0, -10, -5,  5,
		 0,  0,   0,  20,  20,   0,  0,  0,
		 5,  5,  10,  25,  25,  10,  5,  5,
		10, 10,  20,  30,  30,  20, 10, 10,
		50, 50,  50,  50,  50,  50, 50, 50,
		 0,  0,   0,   0,   0,   0,  0,  0
	};
}

namespace PSQT::EndGame {
	const float KING[SQUARE_COUNT] = {
		-10, -20, -20, -20, -20, -20, -20, -10,
		  0,   0,   0,   0,   0,   0,   0,   0,
		 10,  10,  10,  10,  10,  10,  10,  10,
		 20,  20,  15,  15,  15,  15,  20,  20,
		 20,  20,  25,  25,  25,  25,  20,  20,
		 20,  20,  25,  25,  25,  25,  20,  20,
		 20,  20,  25,  25,  25,  25,  20,  20,
		 20,  30,  30,  30,  30,  30,  30,  20
	};

	const float QUEEN[SQUARE_COUNT] = {
		-20, -10, -10, -5, -5, -10, -10, -20,
		-10,   0,   5,  0,  0,   0,   0, -10,
		-10,   5,   5,  5,  5,   5,   0, -10,
		  0,   0,   5,  5,  5,   5,   0,  -5,
		 -5,   0,   5,  5,  5,   5,   0,  -5,
		-10,   0,   5,  5,  5,   5,   0, -10,
		-10,   0,   0,  0,  0,   0,   0, -10,
		-20, -10, -10, -5, -5, -10, -10, -20
	};

	const float ROOK[SQUARE_COUNT] = {
		 0,  0,  0,  5,  5,  0,  0,  0
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		 5, 10, 10, 10, 10, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	const float BISHOP[SQUARE_COUNT] = {
		-20, -10, -10, -10, -10, -10, -10, -20,
		-10,   5,   0,   0,   0,   0,   5, -10,
		-10,  10,  10,  10,  10,  10,  10, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   5,   5,  10,  10,   5,   5, -10,
		-10,   0,   5,  10,  10,   5,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-20, -10, -10, -10, -10, -10, -10, -20
	};

	const float KNIGHT[SQUARE_COUNT] = {
		-50, -40, -30, -30, -30, -30, -40, -50,
		-40, -20,   0,   5,   5,   0, -20, -40,
		-30,   5,  10,  15,  15,  10,   5, -30,
		-30,   0,  15,  20,  20,  15,   0, -30,
		-30,   5,  15,  20,  20,  15,   5, -30,
		-30,   0,  10,  15,  15,  10,   0, -30,
		-40, -20,   0,   0,   0,   0, -20, -40,
		-50, -40, -30, -30, -30, -30, -40, -50
	};

	const float PAWN[SQUARE_COUNT] = {
		 0,   0,   0,   0,   0,   0,   0,  0,
		-5, -10, -10, -20, -20,  10, -10, -5,
		-5,  -5, -10, -10, -10, -10,  -5, -5,
		 0,   0,   0,  20,  20,   0,   0,  0,
		 5,   5,  10,  25,  25,  10,   5,  5,
		10,  10,  20,  30,  30,  20,  10, 10,
		50,  50,  50,  50,  50,  50,  50, 50,
		 0,   0,   0,   0,   0,   0,   0,  0
	};
}

namespace Evaluator {
	/**
	 * @brief Calculates and returns the piece square evaluation of the current position. Positive return value means
	 * the player to move is ahead. Negative return value means player to move is behind. Return value of 0 means equal.
	 *
	 * @tparam S Current stage the game. Can be OPENING, MIDDLEGAME or ENDGAME.
	 * @param position Position object to evaluate.
	 * @return Piece square evaluation of the current position with respec to the game stage.
	 */
	template<GameStage S>
	float psqtEvaluation(Position& position);

	/**
	 * @brief Returns the piece square evaluation of the current position.
	 *
	 * @param position Position object to evaluate.
	 * @return Piece square evaluation of the current position.
	 */
	float psqtEvaluation(Position& position);

	/**
	 * @brief Returns the raw piece evaluation of the current position.
	 *
	 * @param position Position object to evaluate.
	 * @return Raw piece evaluation of the current position.
	 */
	float getPieceEvaluation(Position& position);

	/**
	 * @brief Evaluate the current position.
	 *
	 * @param position Reference to the position to evaluate.
	 * @param moves MoveList of the current available moves.
	 * @return Evaluation of the position.
	 */
	float evaluate(Position& position, MoveList& moves);

	/**
	 * @brief Returns the stage of the position.
	 *
	 * @param position Position object to evaluate.
	 * @return GameStage of the current position.
	 */
	GameStage getStage(Position& position);
};

#endif
