
#ifndef GAME_HPP
#define GAME_HPP

#include "constants.hpp"
#include "movegen.hpp"

/**
 * A struct representing the current board position.
 */
class Pos {
    public:
        std::string original_fen;
        int halfmove, fullmove;
        int piece_index[12];

        void displayAll(Pos* game);
        ExitCode parseFen(std::string fen);
        void zero();
        void handleGame(Computed* moves, CmdLine* args, char *argv[]);
        int getMoves(Computed* moves, uint64_t* enemy_attacks, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS]);
        void getEnemyAttacks(Computed* moves, uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, uint64_t* kEnemy_attacks);


        bool isDoubleChecked(Computed* moves);
        MovesStruct* getRookFamily(Computed* moves, Square square);
        MovesStruct* getBishopFamily(Computed* moves, Square square);
        void display(CmdLine* args);
        ExitCode isEOG(Computed* moves, uint64_t enemy_attacks, int move_index);
        bool isThreeFoldRep();
        bool insufficientMaterial();
        bool isChecked(uint64_t enemy_attacks);
        void getKingMoves(Computed* computed_moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        void getCheckedMoves(Computed* moves, uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        uint64_t getBishopCheckRays(Computed* moves, Square square, uint64_t* checkers_only);
        uint64_t getRookCheckRays(Computed* moves, Square square, uint64_t* checkers);
        uint64_t getPawnCheckers(Square square, uint64_t* checkers_only);
        uint64_t getKnightCheckers(Computed* moves, Square square, uint64_t* checkers_only);
        void getCheckedEp(uint64_t* rook_pins, uint64_t* bishop_pins, uint64_t checkers, Computed* moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getNormalMoves(Computed* moves, uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        void getQueenMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getRookPinMoves(Computed* moves, int square, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getBishopPinMoves (Computed* moves, int square, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getRookMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getBishopMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getKnightMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getPawnMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        uint64_t pawnMoveArgs(Square square);
        void getCastlingMoves(uint64_t enemy_attacks, Computed* moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getEpMoves(uint64_t rook_pins, uint64_t bishop_pins, Computed* moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void horizontalPinEp(int king, bool turn, int attacker_sq, int captured_pawn, Computed* moves, int ep, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void diagonalPinEp(int king, bool turn, int attacker_sq, int captured_pawn, Computed* moves, int ep, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        uint16_t chooseMove(int white, int black, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, Computed* moves);
        void fen();
        std::string getFEN();
        void getSquares(std::string move_string, uint16_t* move, uint* start, uint* end);
        void checkCastlingEnPassantMoves(uint start, uint end, uint16_t* move);
        bool validMove(uint16_t move, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void makeMove(uint16_t move);
        void undoMove();
        void undoNormal();
        void findAndRemovePiece(PieceType piece, Square square);
        bool isDark(int square);
        void undoPromotion();
        void addPiece(PieceType piece, Square square);
        void undoEnPassant();
        void undoCastling();
        void saveHistory(uint16_t move);
        void makeKingMoves(uint16_t move);
        void removePiece();
        void handleCastle();
        void makeQueenMoves(uint16_t move);
        void makeRookMoves(uint16_t move);
        void makeBishopMoves(uint16_t move);
        void makeKnightMoves(uint16_t move);
        void makePawnMoves(uint16_t move);
        double alphaBeta(int depth, double alpha, double beta, bool max, Computed* moves);
        double evaluate();
        void printMove(uint16_t move, bool extraInfo);
        void showEOG(ExitCode code, char *argv[]);
        const int rookBlockIndex(uint64_t pos, Computed* moves, Square square);

    private:
        uint64_t sides[2];
        uint64_t kings, queens, rooks, bishops, knights, pawns;
        bool turn;
        int castling;
        Square en_passant;

        // The piece positions.
        Square piece_list[12][10];
        PieceType pieces[64];

        // Piece counts for insufficient material checks.
        int piece_cnt = 0;
        int knight_cnt = 0;
        int wdsb_cnt = 0, wlsb_cnt = 0;
        int bdsb_cnt = 0, blsb_cnt = 0;

        uint16_t last_move;
        PieceType piece_moved;
        PieceType piece_captured;
        MoveType last_move_type;

        // Pos history
        History history[MAX_MOVES];
        int ply;
        uint64_t hash;
};

namespace Play {
    void init(Pos* game, CmdLine* args,char *argv[], std::string input);
}

/**
 * Checks if end of game has been reached.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves.
 * @param enemy_attacks: Bitboard of squares attacked by enemy pieces.
 * @param moves_index: Pointer to number of vectors in pos_moves.
 * @return: The appropriate ExitCode.
 */
ExitCode isEOG(Pos* game, Computed* moves, uint64_t enemy_attacks, 
        int move_index);

/**
 * Show the usage of the program.
 * 
 * @param argv: Array of command line arguments.
 */
void showUsage(char *argv[]);

/**
 * Show the end of game message.
 * @param code: The exitcode of the game.
 * @param argv: Array of command line arguments.
 */
void showEOG(ExitCode code, char *argv[]);

/**
 * Handles a single game.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param args: Pointer to command line arguments struct.
 */
void handleGame(Pos* game, Computed* Moves, CmdLine* args, char *argv[]);

/**
 * Make the moves.
 * @param move: The moves to make.
 * @param game: Pointer to game struct.
 */
void makeMove(uint16_t move, Pos* game);

/**
 * Undo moves.
 * @param game: Pointer to game struct.
 */
void undoMove(Pos* game);

/**
 * Retrives all legal moves of the current position.
 * 
 * @param game: Pointer to Pos struct of the current position.
 * @param moves: Pointer to the precomputed moves struct.
 * @param enemy_attacks: Pointer to bitboard of all squares attacked by the 
 *  enemy.
 * 
 * @return: The number of move sets.
 */
int getMoves(Pos* game, Computed* moves, uint64_t* enemy_attacks, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS]);

#endif
