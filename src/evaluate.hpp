
#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include "constants.hpp"

namespace PSQT {
    extern const int PAWN[64];
    extern const int KNIGHT[64];
    extern const int BISHOP[64];
    extern const int ROOK[64];
    extern const int QUEEN[64];
    extern const int KING[64];
    extern const int KING_ENDGAME[64];
}

namespace PieceValue {
    namespace Opening {
        const int values[12] = {
            0, 0, // Kings
            900, 500, 325, 300, 100, // Q, R, B, N, P
            -900, -500, -325, -300, -100, // Q, R, B, N, P
        };
    }

    namespace EndGame {
        const int values[12] = {
            0, 0, // Kings
            960, 511, 325, 300, 250, // Q, R, B, N, P
            -960, -511, -325, -300, -250,
        };
    }
}

class Pos;
class Evaluator {
    public:
        Evaluator(const Pos&);
        int evaluate();

    private:
        const Pos& pos;
};

#endif
