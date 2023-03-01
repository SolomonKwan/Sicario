#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include <chrono>
#include "search.hpp"

class MiniMax : public BaseSearcher {
	public:
		MiniMax(Position& pos, const std::atomic_bool& searchTree, const SicarioConfigs& sicarioConfigs) :
				BaseSearcher(pos, searchTree, sicarioConfigs) {}
		void search();
		float evaluate(Position& pos, int depth, float alpha, float beta, Player player);
		float iddfs();

	private:
		std::chrono::_V2::system_clock::time_point start;
};

#endif