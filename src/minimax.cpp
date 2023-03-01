#include <limits>
#include <iostream>
#include "minimax.hpp"
#include "evaluate.hpp"

void MiniMax::search() {
	float minInfinity = std::numeric_limits<float>::min();
	float maxInfinity = std::numeric_limits<float>::max();
	int depth = 6;

	auto start = std::chrono::high_resolution_clock::now();
	std::cout << evaluate(pos, depth, minInfinity, maxInfinity, pos.getTurn()) << '\n';
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns\n";
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "μs\n";
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
	std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s\n";
}

float MiniMax::evaluate(Position& pos, int depth, float alpha, float beta, Player player) {
	auto now = std::chrono::high_resolution_clock::now();
	uint64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
	uint64_t maxDuration = std::stoi(sicarioConfigs.options[EXPAND_TIME].value);
	MoveList moves = MoveList(pos);
	if (depth == 0 || duration >= maxDuration) return Evaluator::evaluate(pos, moves);

	float maxEval = std::numeric_limits<float>::min();
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

float MiniMax::iddfs() {
	float value = 0;
	float minInfinity = std::numeric_limits<float>::min();
	float maxInfinity = std::numeric_limits<float>::max();
	start = std::chrono::high_resolution_clock::now();
	uint64_t maxDuration = std::stoi(sicarioConfigs.options[EXPAND_TIME].value);
	for (int depth = 1; depth < INT_MAX && searchTree; depth++) {
		auto now = std::chrono::high_resolution_clock::now();
		uint64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
		if (duration >= maxDuration) break;
		value = evaluate(pos, depth, minInfinity, maxInfinity, pos.getTurn());
	}
	return value;
}