
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

namespace PieceValue {
    namespace Opening {
        const int values[12] = {
            0, 0, // Kings
            9100, 4350, 3225, 3100, 600, // Q, R, B, K, P
            -9100, -4350, -3225, -3100, -600,
        };
    }

    namespace EndGame {
        const int values[12] = {
            0, 0, // Kings
            9700, 5475, 3225, 3100, 925, // Q, R, B, K, P
            -9700, -5475, -3225, -3100, -925,
        };
    }
}

Square mirrored(Square);

class Evaluator {
    public:
        Evaluator(const Pos&);
        float evaluate();

    private:
        const Pos& pos;
};

#endif
