
#include "evaluate.hpp"
#include "game.hpp"

/**
 * Piece square tables in the view of white.
 */
namespace PSQT {
    const int PAWN[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    const int KNIGHT[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    const int BISHOP[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    const int ROOK[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    };

    const int QUEEN[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
         0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    const int KING[64] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    };

    const int KING_ENDGAME[64] = {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    };
}

/**
 * Returns PSQT index for black.
 * @param square: The desired square to convert.
 * @note The PSQT value will still need to be multiplied by -1.
 */
inline Square mirrored(Square square) {
    return (Square) (square ^ 56);
}

// Evaluator::Evaluator(const Pos& pos) : pos(pos) {}

/**
 * Returns true if it is an endgame with queens, else false.
 * @param rookCnt: The count of rooks.
 * @param bishopCnt: The count of bishops.
 * @param knightCnt: The count of the knights.
 * @return: True, if an endgame with queens, else false.
 */
inline bool isEndGameWithQueen(int rookCnt, int bishopCnt, int knightCnt) {
    return (rookCnt == 0 && bishopCnt + knightCnt <= 1) ? true : false;
}

/**
 * Returns true if it is an endgame, else false.
 * @return bool.
 */
bool Pos::isEndGame() const {
    if (this->piece_index[W_QUEEN] == 0 && this->piece_index[B_QUEEN] == 0) return true;

    // Every side which has a queen has additionally no other pieces or one minorpiece maximum.
    if (this->piece_index[W_QUEEN] <= 1 && isEndGameWithQueen(this->piece_index[W_ROOK], this->piece_index[W_BISHOP],
            this->piece_index[W_KNIGHT]) && this->piece_index[B_QUEEN] <= 1 && isEndGameWithQueen(
            this->piece_index[B_ROOK], this->piece_index[B_BISHOP], this->piece_index[B_KNIGHT])) {
        return true;
    }

    return false;
}

/**
 * Returns a piece square table value representation of the board.
 * @param int: A value representing the value of the board.
 */
int Pos::psqt() const {
    int value = 0;

    // King value
    if (this->isEndGame()) {
        value += PSQT::KING_ENDGAME[this->piece_list[W_KING][0]];
        value += -1 * PSQT::KING_ENDGAME[mirrored(this->piece_list[B_KING][0])];
    } else {
        value += PSQT::KING[this->piece_list[W_KING][0]];
        value += -1 * PSQT::KING[mirrored(this->piece_list[B_KING][0])];
    }

    // Queen value
    for (int i = 0; i < this->piece_index[W_QUEEN]; i++) {
        value += PSQT::QUEEN[this->piece_list[W_QUEEN][i]];
    }
    for (int i = 0; i < this->piece_index[B_QUEEN]; i++) {
        value += -1 * PSQT::QUEEN[mirrored(this->piece_list[B_QUEEN][i])];
    }

    // Rook value
    for (int i = 0; i < this->piece_index[W_ROOK]; i++) {
        value += PSQT::ROOK[this->piece_list[W_ROOK][i]];
    }
    for (int i = 0; i < this->piece_index[B_ROOK]; i++) {
        value += -1 * PSQT::ROOK[mirrored(this->piece_list[B_ROOK][i])];
    }

    // Bishop value
    for (int i = 0; i < this->piece_index[W_BISHOP]; i++) {
        value += PSQT::BISHOP[this->piece_list[W_BISHOP][i]];
    }
    for (int i = 0; i < this->piece_index[B_BISHOP]; i++) {
        value += -1 * PSQT::BISHOP[mirrored(this->piece_list[B_BISHOP][i])];
    }

    // Knight value
    for (int i = 0; i < this->piece_index[W_KNIGHT]; i++) {
        value += PSQT::KNIGHT[this->piece_list[W_KNIGHT][i]];
    }
    for (int i = 0; i < this->piece_index[B_KNIGHT]; i++) {
        value += -1 * PSQT::KNIGHT[mirrored(this->piece_list[B_KNIGHT][i])];
    }

    // Pawn value
    for (int i = 0; i < this->piece_index[W_PAWN]; i++) {
        value += PSQT::PAWN[this->piece_list[W_PAWN][i]];
    }
    for (int i = 0; i < this->piece_index[B_PAWN]; i++) {
        value += -1 * PSQT::PAWN[mirrored(this->piece_list[B_PAWN][i])];
    }

    return value;
}

/**
 * Returns a material representation of the board.
 * @param int: An integer representing the piece value of the table.
 */
int Pos::material() const {
    int value = 0;

    if (this->isEndGame()) {
        value += PieceValue::EndGame::values[W_QUEEN] * this->piece_index[W_QUEEN];
        value += PieceValue::EndGame::values[W_ROOK] * this->piece_index[W_ROOK];
        value += PieceValue::EndGame::values[W_BISHOP] * this->piece_index[W_BISHOP];
        value += PieceValue::EndGame::values[W_KNIGHT] * this->piece_index[W_KNIGHT];
        value += PieceValue::EndGame::values[W_PAWN] * this->piece_index[W_PAWN];

        value += PieceValue::EndGame::values[B_QUEEN] * this->piece_index[B_QUEEN];
        value += PieceValue::EndGame::values[B_ROOK] * this->piece_index[B_ROOK];
        value += PieceValue::EndGame::values[B_BISHOP] * this->piece_index[B_BISHOP];
        value += PieceValue::EndGame::values[B_KNIGHT] * this->piece_index[B_KNIGHT];
        value += PieceValue::EndGame::values[B_PAWN] * this->piece_index[B_PAWN];
    } else {
        value += PieceValue::Opening::values[W_QUEEN] * this->piece_index[W_QUEEN];
        value += PieceValue::Opening::values[W_ROOK] * this->piece_index[W_ROOK];
        value += PieceValue::Opening::values[W_BISHOP] * this->piece_index[W_BISHOP];
        value += PieceValue::Opening::values[W_KNIGHT] * this->piece_index[W_KNIGHT];
        value += PieceValue::Opening::values[W_PAWN] * this->piece_index[W_PAWN];

        value += PieceValue::Opening::values[B_QUEEN] * this->piece_index[B_QUEEN];
        value += PieceValue::Opening::values[B_ROOK] * this->piece_index[B_ROOK];
        value += PieceValue::Opening::values[B_BISHOP] * this->piece_index[B_BISHOP];
        value += PieceValue::Opening::values[B_KNIGHT] * this->piece_index[B_KNIGHT];
        value += PieceValue::Opening::values[B_PAWN] * this->piece_index[B_PAWN];
    }

    return value;
}

/**
 * Returns a pseudorandom move from the provided MoveList based off of various heuristics.
 * @param moves: MoveList of the legal moves in the postion.
 * @return: A pseudorandom move.
 */
Move Pos::pseudoRandomMove(MoveList& moves) {
    Move bestMove = 0;
    int bestValue = 0;
    for (Move move : moves) {
        this->makeMove(move);

        int value = this->psqt();
        value += this->material();
        if ((this->turn == WHITE && value <= bestValue) || (this->turn == BLACK && value >= bestValue)) {
            bestValue = value;
            bestMove = move;
        }

        this->undoMove();
    }

    return bestMove;
}
