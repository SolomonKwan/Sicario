
#ifndef GAME_HPP
#define GAME_HPP

#include "constants.hpp"
#include "movegen.hpp"

typedef uint64_t Hash;

/**
 * A struct representing the current board position.
 */
class Pos {
    public:
        Pos(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        // Miscellaneous
        void run();
        void display();
        void displayAll(Pos* game);
        ExitCode parseFen(std::string fen);
        uint64_t perft(int depth, bool print = false);
        void zero();

    private:
        // Non-position information
        bool turn;
        int castling;
        Square en_passant;
        int halfmove, fullmove;

        // Bitboards
        uint64_t sides[2];
        uint64_t kings, queens, rooks, bishops, knights, pawns;

        // Piece positions
        int piece_index[12];
        Square piece_list[12][10];
        PieceType pieces[64];

        // Piece counts for insufficient material checks.
        int piece_cnt = 0;
        int knight_cnt = 0;
        int wdsb_cnt = 0, wlsb_cnt = 0;
        int bdsb_cnt = 0, blsb_cnt = 0;

        // Pos history
        Move last_move;
        PieceType piece_moved;
        PieceType piece_captured;
        MoveType last_move_type;
        History history[MAX_MOVES];
        Hash hash;
        int ply;

        // AI?
        double alphaBeta(int depth, double alpha, double beta, bool max);
        double evaluate();

        // EOG checks
        ExitCode isEOG(Bitboard enemy_attacks, int move_index);
        bool insufficientMaterial();
        bool isThreeFoldRep();

        // Game logic
        void checkCastlingEnPassantMoves(uint start, uint end, Move* move);
        bool validMove(Move move, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        bool isChecked(uint64_t enemy_attacks);
        bool isDoubleChecked();
        uint64_t getBishopCheckRays(Square square, uint64_t* checkers_only);
        uint64_t getRookCheckRays(Square square, uint64_t* checkers);
        uint64_t getPawnCheckers(Square square, uint64_t* checkers_only);
        uint64_t getKnightCheckers(Square square, uint64_t* checkers_only);
        const int rookBlockIndex(uint64_t pos, Square square);

        // Position updates
        void findAndRemovePiece(PieceType piece, Square square);
        void addPiece(PieceType piece, Square square);
        void removePiece();

        // Make move
        void makeMove(Move move);
        void makeKingMoves(Move move);
        void makeQueenMoves(Move move);
        void makeRookMoves(Move move);
        void makeBishopMoves(Move move);
        void makeKnightMoves(Move move);
        void makePawnMoves(Move move);
        void handleCastle();

        // Undo
        void undoNormal();
        void undoMove();
        void undoCastling();
        void undoPromotion();
        void undoEnPassant();

        // Move generation
        int getMoves(Bitboard* enemy_attacks, std::vector<Move>* pos_moves[MAX_MOVE_SETS]);
        void getEnemyAttacks(uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, uint64_t* kEnemy_attacks);
        MovesStruct* getRookFamily(Square square);
        MovesStruct* getBishopFamily(Square square);
        uint64_t pawnMoveArgs(Square square);

        // Normal move generation
        void getNormalMoves(uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        void getKingMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        void getQueenMoves(uint64_t rook_pins, uint64_t bishop_pins, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getRookMoves(uint64_t rook_pins, uint64_t bishop_pins, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getBishopMoves(uint64_t rook_pins, uint64_t bishop_pins, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getKnightMoves(uint64_t rook_pins, uint64_t bishop_pins, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getPawnMoves(uint64_t rook_pins, uint64_t bishop_pins, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getRookPinMoves(int square, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getBishopPinMoves (int square, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getCastlingMoves(uint64_t enemy_attacks, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getEpMoves(uint64_t rook_pins, uint64_t bishop_pins, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void horizontalPinEp(int king, bool turn, int attacker_sq, int captured_pawn, int ep, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void diagonalPinEp(int king, bool turn, int attacker_sq, int captured_pawn, int ep, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);

        // Check move generation
        void getCheckedMoves(uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        void getCheckedEp(uint64_t* rook_pins, uint64_t* bishop_pins, uint64_t checkers, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);

        // Move reading and parsing
        Move chooseMove(Player white, Player black, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getSquares(std::string move_string, Move* move, uint* start, uint* end);

        // Miscellaneous
        void initialiseHash();
        void showEOG(ExitCode code);
        std::string getFEN();
        void saveHistory(Move move);
};

namespace Play {
    void init(std::string input);
}

#endif
