
#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include "constants.hpp"
#include "game.hpp"

namespace PSQT {
    extern const float PAWN[64];
    extern const float KNIGHT[64];
    extern const float BISHOP[64];
    extern const float ROOK[64];
    extern const float QUEEN[64];
    extern const float KING[64];
    extern const float KING_ENDGAME[64];
}

Square mirrored(Square);

class Evaluator {
    public:
        Evaluator(const Pos&);
        float evaluate(Pos&);

    private:
        const Pos& pos;
};

#endif
