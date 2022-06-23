
#ifndef GAME_HPP
#define GAME_HPP

#include <cmath>

#include "constants.hpp"
#include "movegen.hpp"

#define KIWIPETE "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define MAX_MOVES 500
#define MOVESET_SIZE 32
#define DEFAULT_HASH_SIZE 16

typedef const std::vector<Move>* MoveListArray[MOVESET_SIZE];

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

        /**
         * @brief Retrives all legal moves of the current position.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getMoves(int& moves_index, MoveListArray pos_moves);

        // Tree search
        void makeMove(Move);
        void undoMove();
        Move pseudoRandomMove(MoveList&, Player);

        // Accessors
        Hash getHash();
        Player getTurn();

        // Checkers
        ExitCode isEOG(MoveList&);

    private:
        // Non-position information
        Player turn;
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
        Bitboard checkers = 0ULL;

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

        /**
         * @brief Checks if the specified square is attacked by the specified player.
         * @param square Square to check if attacked by "player".
         * @param player Attacking player to check.
         * @param ignoreKing Ignore the !"player" king when calculating. Used for king check moves.
         * @return Bitboard whose set bits indicate a piece belonging to "player" that attacks the given square.
         */
        Bitboard isAttacked(const Square square, const Player player, const bool ignoreKing = false);

        /**
         * @brief Sets the bitboard of checkers.
         */
        void setCheckers();

        /**
         * @brief Sets the bitboard of pinned pieces.
         */
        void setPinnedPieces();

        /**
         * @brief Checks if king of the current player to move is in double-check.
         * @return: True if in double-check, else false.
         */
        bool inDoubleCheck();

        /**
         * @brief Check if king of current player to move is in check.
         * @return: True if in check, else false.
         */
        bool inCheck();

        /**
         * @brief Retrives and adds the vector of legal king moves of the side to move to the pos_moves array.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getKingMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Calls other functions to add the legal moves for when the king is in check.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getCheckMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retries and adds the vector of legal moves for the queens when king is in check.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getQueenCheckedMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retries and adds the vector of legal moves for the rooks when king is in check.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getRookCheckedMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retries and adds the vector of legal moves for the bishop when king is in check.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getBishopCheckedMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retries and adds the vector of legal moves for the knight when king is in check.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getKnightCheckedMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retries and adds the vector of legal moves for the pawn when king is in check.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getPawnCheckedMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Calls other functions to add the legal moves for when there is no check.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getNormalMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retrives and adds the vector of legal queen moves of the side to move to the pos_moves array.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getQueenMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retrives and adds the vector of legal rook moves of the side to move to the pos_moves array.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getRookMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retrives and adds the vector of legal bishop moves of the side to move to the pos_moves array.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getBishopMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retrives and adds the vector of legal knight moves of the side to move to the pos_moves array.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getKnightMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retrives and adds the vector of legal castling moves of the side to move to the pos_moves array.
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getCastlingMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Retrives and adds the vector of legal pawn moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getPawnMoves(int& moves_index, MoveListArray pos_moves);

        /**
         * @brief Get the reach bitboard of a rook on the indicated square.
         *
         * @param occupancy The occupancy bitboard.
         * @param square The square of concern.
         * @return Bitboard of the rook reach from the square.
         */
        Bitboard getRookReachBB(Bitboard occupancy, Square square);

        /**
         * @brief Get the reach bitboard of a bishop on the indicated square.
         *
         * @param occupancy The occupancy bitboard.
         * @param square The square of concern.
         * @return Bitboard of the bishop reach from the square.
         */
        Bitboard getBishopReachBB(Bitboard occupancy, Square square);








        void getRookPinMoves(int, std::vector<Move>*[MOVESET_SIZE], int&);
        void getBishopPinMoves (int, std::vector<Move>*[MOVESET_SIZE], int&);

        // EOG checks
        bool insufficientMaterial();
        bool isThreeFoldRep();

        // Game logic
        void checkCastlingEnPassantMoves(uint, uint, Move&);
        bool validMove(Move, MoveList&);
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
        // void horizontalPinEp(int, bool, int, int, int, std::vector<Move>*[MOVESET_SIZE], int&);
        // void diagonalPinEp(int, bool, int, int, int, std::vector<Move>*[MOVESET_SIZE], int&);

        // Check move generation
        // void getCheckedEp(Bitboard, std::vector<Move>*[MOVESET_SIZE], int&);

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

/**
 * Generate zobrist piece hashes using a predefined seed.
 * @return: Array of size 12 (each piece for each colour) containing arrays of size 64 Hashes.
 */
std::array<std::array<Hash, 64>, 12> generatePieceHashes();

/**
 * Generate zobrist hashes for the turn using predefined seed.
 * @return: Hash representing turn.
 */
Hash generateTurnHash();

/**
 * Generate zobrist hashes for different castling permissions combinations using a predefined seed.
 * @return: Array of size 16 (the number of possible castling permission combinations) of Hashes.
 */
std::array<Hash, 16> generateCastlingHash();

/**
 * Generate zobrist hashes for en passant file using predefined seed.
 * @return: Array of size 8 (number of files) of Hashes.
 * @note: The colour of the player who can capture en passant is taken care of by the turn hash.
 */
std::array<Hash, 8> generateEnPassantHash();

#endif
