
#ifndef GAME_HPP
#define GAME_HPP

#include <atomic>

#include "constants.hpp"
#include "movegen.hpp"
#include "search.hpp"

void printMove(Move move, bool extraInfo);
MoveType type(Move move);
Square end(Move move);
Square start(Move move);
Promotion promo(Move move);

/**
 * Forward declarations.
 */
class SearchInfo;
class MoveList;

/**
 * A struct representing the current board position.
 */
class Pos {
    public:
        Pos(std::string = STANDARD_GAME);
        void run();
        uint64_t perft(int, bool = false);
        ExitCode parseFen(std::string);
        void display() const;
        void displayAll() const;
        void setPlayer(Player, std::string);

        // Static position evaluation
        bool isEndGame() const;
        int psqt() const;
        int material() const;

        // Tree search
        void setDepth(int);
        void setHashSize(int);
        void search(SearchParams, std::atomic_bool&);
        void makeMove(Move);
        void undoMove();

        // Move generation
        void getMoves(int&, std::vector<Move>*[MAX_MOVE_SETS]);

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

        // Pos history
        Move last_move;
        PieceType piece_moved;
        PieceType piece_captured;
        MoveType last_move_type;
        std::vector<History> history;
        std::unordered_map<Bitboard, int> hashes;
        Hash hash;
        int ply;

        // Miscellaneous info
        PlayerType white = COMPUTER, black = COMPUTER;
        bool unicodeMode = true;

        // Perft hashing
        std::unordered_map<Bitboard, uint64_t> perft_hash;

        // Evaluation and search
        SearchInfo searchInfo;
        int depth = 3;
        std::vector<std::pair<int, Move>> scoreMoves(SearchParams, MoveList&);

        // Move ordering
        int scoreMove(Move);
        int kingSafety(Move);
        int scoreCastlingSafety(Move);
        int scoreKingSafety(Move);
        int captures(Move);

        // EOG checks
        bool insufficientMaterial();
        bool isThreeFoldRep();

        // Game logic
        void checkCastlingEnPassantMoves(uint, uint, Move&);
        bool validMove(Move, MoveList&);
        bool isChecked();
        bool isDoubleChecked();
        Bitboard getBishopCheckRays(Square, Bitboard&);
        Bitboard getRookCheckRays(Square, Bitboard&);
        Bitboard getPawnCheckers(Square, Bitboard&);
        Bitboard getKnightCheckers(Square, Bitboard&);
        const int rookBlockIndex(Bitboard, Square);
        Bitboard isAttacked(const Square, const bool);
        Bitboard isOccupied(const Square);
        void setBitboards();
        bool oneBitSet(Bitboard);
        Bitboard getKingAttackers(const Square, const bool) const;
        void setCheckers();
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
        void getNormalMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getKingMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getQueenMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getRookMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getBishopMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getKnightMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getPawnMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getRookPinMoves(int, std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getBishopPinMoves (int, std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getCastlingMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getEpMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void horizontalPinEp(int, bool, int, int, int, std::vector<Move>*[MAX_MOVE_SETS], int&);
        void diagonalPinEp(int, bool, int, int, int, std::vector<Move>*[MAX_MOVE_SETS], int&);

        // Check move generation
        void getCheckedMoves(std::vector<Move>*[MAX_MOVE_SETS], int&);
        void getCheckedEp(Bitboard, std::vector<Move>*[MAX_MOVE_SETS], int&);

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
        MoveList(Pos&);
        uint64_t bulkCount();
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

        std::vector<Move>* moves[MAX_MOVE_SETS];
        int moves_index;
        Move endMove; // Dummy move for end of iterator. Just need the address.
};

namespace Play {
    void init(std::string);
}

#endif
