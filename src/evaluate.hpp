
#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include "constants.hpp"

namespace Opening {
    extern const float w_pawn[64];
    extern const float b_pawn[64];
    extern const double material_value[12];
    extern const bool is_phase(Game* game);
}

namespace MiddleGame {
    extern const float w_pawn[64];
    extern const float b_pawn[64];
    extern const double material_value[12];
    extern const bool is_phase(Game* game);
}

namespace EndGame {
    extern const float w_pawn[64];
    extern const float b_pawn[64];
    extern const double material_value[12];
    extern const bool is_phase(Game* game);
}

class Evaluator {
    public:
        Evaluator(Game* game);
    
    private:
        Game* game;
        double material_value[12] = {
            0, 0, 9, 5, 3.25, 3, 1, -9, -5, -3.25, -3, -1
        };

    public:
        int evaluate(Game* game);
        double material();
};

#endif
