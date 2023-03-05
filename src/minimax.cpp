#include <limits>
#include <iostream>
#include "minimax.hpp"
#include "evaluate.hpp"

MiniMax::MiniMax(Position& pos, const std::atomic_bool& searchTree, const SicarioConfigs& sicarioConfigs)
	: BaseSearcher(pos, searchTree, sicarioConfigs) {}

void MiniMax::search() {
	float minInfinity = -std::numeric_limits<float>::max();
	float maxInfinity = std::numeric_limits<float>::max();
	int depth = 5;
	evaluate(pos, depth, minInfinity, maxInfinity, pos.getTurn());
}

float MiniMax::evaluate(Position& pos, int depth, float alpha, float beta, Player player) {
	MoveList moves = MoveList(pos);
	if (depth == 0) return Evaluator::evaluate(pos, moves);

	float maxEval = -std::numeric_limits<float>::max();
	float minEval = std::numeric_limits<float>::max();

	for (Move move : moves) {
		pos.processMakeMove(move);
		float eval = this->evaluate(pos, depth - 1, alpha, beta, !player);
		pos.processUndoMove();
		player == WHITE ? maxEval = std::max(maxEval, eval) : minEval = std::min(minEval, eval);
		player == WHITE ? alpha = std::max(alpha, eval) : beta = std::min(beta, eval);
		if (beta <= alpha) break;
	}
	return player == WHITE ? maxEval : minEval;
}