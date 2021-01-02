
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
        const float values[12] = {
            0, 0, // Kings
            91.00, 43.50, 32.25, 31.00, 6.00, // Q, R, B, K, P
            -91.00, -43.50, -32.25, -31.00, -6.00,
        };
    }

    namespace EndGame {
        const float values[12] = {
            0, 0, // Kings
            97.00, 54.75, 32.25, 31.00, 9.25, // Q, R, B, K, P
            -97.00, -54.75, -32.25, -31.00, -9.25,
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
