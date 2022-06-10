
#ifndef GAME_HPP
#define GAME_HPP

#include <cmath>

#include "constants.hpp"
#include "movegen.hpp"

#define KIWIPETE "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define MAX_MOVES 500
#define MOVESET_SIZE 32
#define DEFAULT_HASH_SIZE 16

void printMove(Move move, bool extraInfo);

inline MoveType type(Move move) {
    return (MoveType)(move & (0b11 << 12));
}

inline Square end(Move move) {
    return (Square)((move >> 6) & 0b111111);
}

inline Square start(Move move) {
    return (Square)(move & 0b111111);
}

inline Promotion promo(Move move) {
    return (Promotion)(move & (0b11 << 14));
}

/**
 * Forward declarations.
 */
class MoveList;

struct PV {
    Move move;
    float score;
};

struct GoParams;

/**
 * A struct representing the current board position.
 */
class Position {
    public:
        Position(std::string = STANDARD_GAME);
        ExitCode parseFen(std::string);
        void display() const;

        // Game logic
        void getMoves(int& moves_index, std::vector<Move>* pos_moves[MOVESET_SIZE]);

        // Tree search
        void makeMove(Move);
        void undoMove();
        Move pseudoRandomMove(MoveList&, Player);

        // Accessors
        Hash getHash();
        bool getTurn();

        // Checkers
        ExitCode isEOG(MoveList&);

    private:
        // Non-position information
        bool turn;
        int castling;
        Square en_passant;
        int halfmove, fullmove;

        // Bitboards
        Bitboard sides[2];
        Bitboard kings, queens, rooks, bishops, knights, pawns;
        Bitboard enemy_attacks;
        Bitboard rook_pins;
        Bitboard bishop_pins;
        Bitboard kEnemy_attacks;
        Bitboard check_rays;
        Bitboard checkers;

        // Piece positions
        int piece_index[12];
        Square piece_list[12][10];
        PieceType pieces[64];

        // Piece counts for insufficient material checks.
        int piece_cnt = 0;
        int knight_cnt = 0;
        int wdsb_cnt = 0, wlsb_cnt = 0;
        int bdsb_cnt = 0, blsb_cnt = 0;

        // Position history
        Move last_move;
        PieceType piece_moved;
        PieceType piece_captured;
        MoveType last_move_type;
        std::vector<History> history;
        std::unordered_map<Bitboard, int> hashes;
        Hash hash;
        int ply;

        // Miscellaneous info
        PlayerType white = HUMAN, black = COMPUTER;
        bool unicodeMode = true;
        bool quiteMode = false;

        // Perft hashing
        std::unordered_map<Bitboard, uint64_t> perft_hash;

        // Evaluation and search
        int depth = 3;





        void setCheckers();
        Bitboard isAttacked(const Square square, const Player player);






        // EOG checks
        bool insufficientMaterial();
        bool isThreeFoldRep();

        // Game logic
        void checkCastlingEnPassantMoves(uint, uint, Move&);
        bool validMove(Move, MoveList&);
        bool inCheck();
        bool inDoubleCheck();
        Bitboard getBishopCheckRays(Square, Bitboard&);
        Bitboard getRookCheckRays(Square, Bitboard&);
        Bitboard getPawnCheckers(Square, Bitboard&);
        Bitboard getKnightCheckers(Square, Bitboard&);
        const int rookBlockIndex(Bitboard, Square);
        Bitboard isOccupied(const Square);
        bool oneBitSet(Bitboard);
        Bitboard getKingAttackers(const Square, const bool) const;
        Bitboard getKingAttackBitBoard() const;

        // Position updates
        void findAndRemovePiece(PieceType, Square);
        void addPiece(PieceType, Square);
        void removePiece();

        // Make move
        void makeKingMoves(Move);
        void makeQueenMoves(Move);
        void makeRookMoves(Move);
        void makeBishopMoves(Move );
        void makeKnightMoves(Move);
        void makePawnMoves(Move);
        void handleCastle();

        // Undo
        void undoNormal();
        void undoCastling();
        void undoPromotion();
        void undoEnPassant();

        // Move generation
        void getEnemyAttacks();
        MovesStruct* getRookFamily(Square);
        MovesStruct* getBishopFamily(Square);
        Bitboard pawnMoveArgs(Square);

        // Normal move generation
        void getNormalMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getKingMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getQueenMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getRookMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getBishopMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getKnightMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getPawnMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getRookPinMoves(int, std::vector<Move>*[MOVESET_SIZE], int&);
        void getBishopPinMoves (int, std::vector<Move>*[MOVESET_SIZE], int&);
        void getCastlingMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getEpMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void horizontalPinEp(int, bool, int, int, int, std::vector<Move>*[MOVESET_SIZE], int&);
        void diagonalPinEp(int, bool, int, int, int, std::vector<Move>*[MOVESET_SIZE], int&);

        // Check move generation
        void getCheckMoves(std::vector<Move>*[MOVESET_SIZE], int&);
        void getCheckedEp(Bitboard, std::vector<Move>*[MOVESET_SIZE], int&);

        // Move reading and parsing
        Move chooseMove(MoveList&);
        void getSquares(std::string, Move&, uint&, uint&);

        // Miscellaneous
        void initialiseHash();
        void showEOG(ExitCode);
        std::string getFEN();
        void saveHistory(Move);
        void zero();
        void incrementHash(Move);
        void decrementHash(Hash);
};

class MoveList {
    public:
        MoveList(Position&);
        uint64_t size();
        Move randomMove();

        struct Iterator {
            Iterator(int, int, int, std::vector<Move>**, Move&);

            Move& operator*() const;
            Move* operator->();

            Iterator& operator++(); // Prefix
            Iterator operator++(int); // Postfix

            friend bool operator== (const Iterator& a, const Iterator& b) {
                return a.ptr == b.ptr;
            }

            friend bool operator!= (const Iterator& a, const Iterator& b) {
                return a.ptr != b.ptr;
            }

            private:
                Move* ptr;
                std::vector<Move>** moves;
                int vec_cnt, i, j;
                Move* endAddr;
        };

        Iterator begin();
        Iterator end();

        std::vector<Move>* moveSets[MOVESET_SIZE];
        int moves_index = 0;
        Move endMove; // Dummy move for end of iterator. Just need the address.
};

std::string concatFEN(std::vector<std::string> strings);

#endif
