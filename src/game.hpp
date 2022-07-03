
#ifndef GAME_HPP
#define GAME_HPP

#include <cmath>

#include "constants.hpp"
#include "movegen.hpp"

#define KIWIPETE "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define MAX_MOVES 500
#define MOVESET_SIZE 32
#define DEFAULT_HASH_SIZE 16

typedef std::vector<Move> const* MoveSet;

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
        /**
         * @brief Initialise the Position object. Uses a FEN string of a normal starting position if none is given.
         */
        Position(std::string = STANDARD_GAME);

        /**
         * @brief Parses the given FEN string.
         *
         * @param fen The FEN string to parse.
         */
        void parseFen(std::string fen);

        /**
         * @brief Retrives all legal moves of the current position.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Performs the given move on the board position.
         *
         * @param move The move to perform.
         */
        void makeMove(Move move);

        /**
         * @brief Undos the last move made.
         */
        void undoMove();

        /**
         * @brief Display the board position and information.
         */
        void display();

        void displayBitboards();

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
        Bitboard rook_pins;
        Bitboard bishop_pins;
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

        /**
         * @brief Checks if the specified square is attacked by the specified player.
         *
         * @param square Square to check if attacked by "player".
         * @param player Attacking player to check.
         * @param ignoreKing Ignore the !"player" king when calculating. Used for king check moves.
         * @return Bitboard whose set bits indicate a piece belonging to "player" that attacks the given square.
         */
        Bitboard isAttacked(const Square square, const Player player, const bool ignoreKing = false);

        /**
         * @brief Check if a piece located on "square" is pinned.
         *
         * @param square The square of the piece to check.
         * @return True if the piece on "square" is pinned, else false.
         */
        inline bool isPinned(const Square square) {
            return isPinnedByRook(square) || isPinnedByBishop(square);
        }

        /**
         * @brief Check if a piece located on "square" is pinned by a rook (or queen in the same manner).
         *
         * @param square The square of the piece to check.
         * @return True if the piece on "square" is pinned, else false.
         */
        inline bool isPinnedByRook(const Square square) {
            return (1ULL << square) & rook_pins;
        }

        /**
         * @brief Check if a piece located on "square" is pinned by a bishop (or queen in the same manner).
         *
         * @param square The square of the piece to check.
         * @return True if the piece on "square" is pinned, else false.
         */
        inline bool isPinnedByBishop(const Square square) {
            return (1ULL << square) & bishop_pins;
        }

        /**
         * @brief Sets the bitboard of checkers.
         */
        void setCheckers();

        /**
         * @brief Sets the rook_pins, bishop_pins and check_ray bitboards.
         */
        void setPinAndCheckRayBitboards();

        /**
         * @brief Checks if king of the current player to move is in double-check.
         * @return: True if in double-check, else false.
         */
        bool inDoubleCheck();

        /**
         * @brief Check if king of current player to move is in check.
         *
         * @return: True if in check, else false.
         */
        bool inCheck();

        /**
         * @brief Retrives and adds the vector of legal king moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getKingMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Calls other functions to add the legal moves for when the king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getCheckMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retries and adds the vector of legal moves for the queens when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getQueenCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retries and adds the vector of legal moves for the rooks when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getRookCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retries and adds the vector of legal moves for the bishop when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getBishopCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retries and adds the vector of legal moves for the knight when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getKnightCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retries and adds the vector of legal moves for the pawn when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getPawnCheckedMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Calls other functions to add the legal moves for when there is no check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getNormalMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retrives and adds the vector of legal queen moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getQueenMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retrives and adds the vector of legal rook moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getRookMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retrives and adds the vector of legal bishop moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getBishopMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retrives and adds the vector of legal knight moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getKnightMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retrives and adds the vector of legal castling moves of the side to move to the pos_moves array.
         * Assumes that the king is not in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getCastlingMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retrives and adds the vector of legal en-passant moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getEnPassantMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Retrives and adds the vector of legal pawn moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getPawnMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

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

        /**
         * @brief Get the diagonal ranged moves for a diagonally pinned piece on "square".
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         * @param square The square of the piece of concern.
         */
        void getBishopPinMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE], Square square);

        /**
         * @brief Get the horizontal and vertical ranged moves for a horizontally or vertically  pinned piece on
         * "square".
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         * @param square The square of the piece of concern.
         */
        void getRookPinMoves(int& moves_index, MoveSet pos_moves[MOVESET_SIZE], Square square);

        /**
         * @brief Checks if the given square is occupied.
         *
         * @param square The square to check.
         * @return True if the square is occupied, else false.
         */
        Bitboard isOccupied(const Square square);

        /**
         * @brief Zeros out the class variables to make way for parsing a new FEN string.
         */
        void resetPosition();

        /**
         * @brief Initialises the hash for the position. Assumes that the FEN string has been parsed.
         */
        void initialiseHash();

        /**
         * @brief Save the move to history before making the move.
         *
         * @param move The move to save.
         */
        void saveHistory(Move move);







        // EOG checks
        bool insufficientMaterial();
        bool isThreeFoldRep();

        // Game logic
        void checkCastlingEnPassantMoves(uint, uint, Move&);
        bool validMove(Move, MoveList&);
        Bitboard getKnightCheckers(Square, Bitboard&);
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

        // Normal move generation
        // void horizontalPinEp(int, bool, int, int, int, std::vector<Move>*[MOVESET_SIZE], int&);
        // void diagonalPinEp(int, bool, int, int, int, std::vector<Move>*[MOVESET_SIZE], int&);

        // Check move generation
        // void getCheckedEp(Bitboard, std::vector<Move>*[MOVESET_SIZE], int&);

        // Move reading and parsing
        Move chooseMove(MoveList&);
        void getSquares(std::string, Move&, uint&, uint&);

        // Miscellaneous
        void showEOG(ExitCode);
        void incrementHash(Move);
        void decrementHash(Hash);
};

class MoveList {
    public:
        MoveList(Position&);
        uint64_t size();
        bool contains(Move move);

        struct Iterator {
            Iterator(int vecCnt, int i, int j, MoveSet* pos_moves, const Move* endMove);

            const Move& operator*() const;
            const Move* operator->() const;

            Iterator& operator++(); // Prefix
            Iterator operator++(int); // Postfix

            friend bool operator== (const Iterator& a, const Iterator& b) {
                return a.ptr == b.ptr;
            }

            friend bool operator!= (const Iterator& a, const Iterator& b) {
                return a.ptr != b.ptr;
            }

            private:
                const Move* ptr;
                MoveSet* moves;
                int vecCnt, i, j;
                const Move* endAddr;
        };

        Iterator begin();
        Iterator end();

        int moves_index = 0;
        MoveSet moveSets[MOVESET_SIZE];
        const Move* endMove = nullptr; // Dummy move for end of iterator. Just need the address.
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
