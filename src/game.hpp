#ifndef GAME_HPP
#define GAME_HPP

#include <cmath>

#include "constants.hpp"
#include "movegen.hpp"

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
        void getMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]);

        /**
         * @brief Performs the given move on the board position.
         *
         * @param move The move to perform.
         * @param hash Whether or not to hash the positions. Defaulted to true.
         */
        void processMakeMove(const Move move, const bool hash = true);

        /**
         * @brief Undoes the last move made.
         */
        void processUndoMove();

        /**
         * @brief Display the board position and information.
         */
        void display();

        void displayBitboards();

        Move pseudoRandomMove(MoveList&, Player);

        // Checkers
        ExitCode isEOG(MoveList&);

    private:
        // Non-position information
        Player turn;
        uint castling;
        Square en_passant;
        uint halfmove, fullmove;

        // Bitboards
        Bitboard sides[PLAYER_COUNT];
        Bitboard kings, queens, rooks, bishops, knights, pawns;
        Bitboard rook_pins;
        Bitboard bishop_pins;
        Bitboard check_rays;
        Bitboard checkers;
        Bitboard rook_ep_pins;

        // Piece positions
        uint piece_index[PIECE_TYPE_COUNT];
        Square piece_list[PIECE_TYPE_COUNT][MAX_PIECE_COUNT];
        PieceType pieces[SQUARE_COUNT];

        // Piece counts for insufficient material checks.
        uint piece_cnt;
        uint knight_cnt;
        uint bishop_cnt;
        uint light_bishop_cnt;
        uint dark_bishop_cnt;

        // Position history
        std::vector<History> history;
        std::unordered_map<Bitboard, int> positionCounts;
        Hash hash;

        /**
         * @brief Get the piece type based on the base piece type.
         *
         * @tparam T Base piece type.
         * @param enemy Whether or not to get the enemy piece instead. Default is false.
         * @return The piece type.
         */
        template<BasePieceType T>
        inline PieceType getPieceType(bool enemy = false) const;

        /**
         * @brief
         *
         * @tparam T
         * @param start
         * @param end
         */
        template <PieceType T>
        void movePiece(const Square start, const Square end);

        void makeMovePieces(const Square start, const Square end);

        void makeMovePieces(const Square square);

        void parseFenMove(std::string& fenMove);

        void parseFenCastling(std::string& fenCastling);

        void parseFenEnPassant(std::string& fenEnPassant);

        void parseFenMoves(std::string& halfmove, std::string& fullmove);

        void updatePieceInfo(Bitboard& pieceBB, PieceType piece, Square square);

        /**
         * @brief Get bitboard of all pieces on the board.
         *
         * @return Bitboard of all pieces.
         */
        inline Bitboard getPieces() const {
            return sides[WHITE] | sides[BLACK];
        }

        /**
         * @brief Get the king square.
         *
         * @return The king square.
         */
        inline Square getKingSquare() const {
            return piece_list[turn][KING_INDEX];
        }

        /**
         * @brief Get the king square.
         *
         * @param turn The player who's king we want.
         * @return The king square.
         */
        inline Square getKingSquare(Player player) {
            return piece_list[turn][KING_INDEX];
        }

        /**
         * @brief Checks if the specified square is attacked by the specified player.
         *
         * @param square Square to check if attacked by "player".
         * @param player Attacking player to check.
         * @param ignoreKing Ignore the !"player" king when calculating. Used for king check moves. Defaulted to false.
         * @return Bitboard whose set bits indicate a piece belonging to "player" that attacks the given square.
         */
        Bitboard isAttacked(const Square square, const Player player, const bool ignoreKing = false) const;

        /**
         * @brief Check if a piece located on "square" is pinned.
         *
         * @param square The square of the piece to check.
         * @return True if the piece on "square" is pinned, else false.
         */
        inline bool isPinned(const Square square) const {
            return isPinnedByRook(square) || isPinnedByBishop(square);
        }

        /**
         * @brief Check if a piece located on "square" is pinned by a rook (or queen in the same manner).
         *
         * @param square The square of the piece to check.
         * @return True if the piece on "square" is pinned, else false.
         */
        inline bool isPinnedByRook(const Square square) const {
            return (ONE_BB << square) & rook_pins;
        }

        /**
         * @brief Check if a piece located on "square" is pinned by a bishop (or queen in the same manner).
         *
         * @param square The square of the piece to check.
         * @return True if the piece on "square" is pinned, else false.
         */
        inline bool isPinnedByBishop(const Square square) const {
            return (ONE_BB << square) & bishop_pins;
        }

        /**
         * @brief Checks if a pawn is pinned by a rook horizontally.
         *
         * @param square The square that the pawn of concern is on.
         * @return True if pinned by a rook/queen horizontally, else false.
         */
        bool isPawnPinnedByRookHorizontally(const Square square) const;

        /**
         * @brief Checks if a pawn is pinned by a rook vertically.
         *
         * @param square The square that the pawn of concern is on.
         * @return True if pinned by a rook/queen vertically, else false.
         */
        bool isPawnPinnedByRookVertically(const Square square) const;

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
        void getKingMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Calls other functions to add the legal moves for when the king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getCheckMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retries and adds the vector of legal moves for the queens when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getQueenCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retries and adds the vector of legal moves for the rooks when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getRookCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retries and adds the vector of legal moves for the bishop when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getBishopCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retries and adds the vector of legal moves for the knight when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getKnightCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retries and adds the vector of legal moves for the pawn when king is in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getPawnCheckedMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Calls other functions to add the legal moves for when there is no check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getNormalMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retrives and adds the vector of legal queen moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getQueenMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retrives and adds the vector of legal rook moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getRookMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retrives and adds the vector of legal bishop moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getBishopMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retrives and adds the vector of legal knight moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getKnightMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retrives and adds the vector of legal castling moves of the side to move to the pos_moves array.
         * Assumes that the king is not in check.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getCastlingMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retrives and adds the vector of legal en-passant moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getEnPassantMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        void getEnPassantCheckMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Retrives and adds the vector of legal pawn moves of the side to move to the pos_moves array.
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         */
        void getPawnMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE]) const;

        /**
         * @brief Get the reach bitboard of a rook on the indicated square.
         *
         * @param occupancy The occupancy bitboard.
         * @param square The square of concern.
         * @return Bitboard of the rook reach from the square.
         */
        Bitboard getRookReachBB(Bitboard occupancy, Square square) const;

        /**
         * @brief Get the reach bitboard of a bishop on the indicated square.
         *
         * @param occupancy The occupancy bitboard.
         * @param square The square of concern.
         * @return Bitboard of the bishop reach from the square.
         */
        Bitboard getBishopReachBB(Bitboard occupancy, Square square) const;

        /**
         * @brief Get the diagonal ranged moves for a diagonally pinned piece on "square".
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         * @param square The square of the piece of concern.
         */
        void getBishopPinMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE], Square square) const;

        /**
         * @brief Get the horizontal and vertical ranged moves for a horizontally or vertically  pinned piece on
         * "square".
         *
         * @param moves_index Current index of the first empty position in the pos_moves array.
         * @param pos_moves Array that holds pointers to vectors of moves.
         * @param square The square of the piece of concern.
         */
        void getRookPinMoves(uint& moves_index, MoveSet pos_moves[MOVESET_SIZE], Square square) const;

        /**
         * @brief Checks if the given square is occupied.
         *
         * @param square The square to check.
         * @return True if the square is occupied, else false.
         */
        Bitboard isOccupied(const Square square) const;

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
        void saveHistory(const Move move);

        /**
         * @brief Get the promotion piece type.
         *
         * @param move The move from which to retrieve the promotion move.
         * @param incHash Whether or not to increment the hash.
         * @return Promotion piece type.
         */
        inline PieceType getPromotionPiece(Move& move);

        template<MoveType T>
        void makeMove(const Move move);

        template<MoveType T>
        void undoMove();

        template <Square KS, Square KE, Square RS, Square RE, PieceType K, PieceType R>
        void makeCastlingMove();

        template <PieceType T>
        void removePiece(const Square square);

        template <PieceType T>
        void addPiece(const Square square);

        PieceType promoPiece(const Move move);

        template <PromoMoveType::MoveType T>
        void makePromotionMove(const Move move);

        template <NormalMoveType::MoveType T>
        void makeNormalMove(const Move move);

        /**
         * @brief Assumes that end square is empty.
         *
         * @param move
         */
        void addToPromotionBitboard(const Move move);

        void removeFromBitboardPromotion(const Move move);

        void addPiecePromotion(const Move move);

        void removePiecePromotion(const Move move);

        template<BasePieceType T>
        void movePieceBitboard(const Square start, const Square end);

        template<BasePieceType T>
        void addToBitboard(const Square square);

        void movePieceAndUpdateBitboard(PieceType piece, const Square start, const Square end);

        void placeCapturedPiece(PieceType piece, const Square square);

        /**
         * @brief Checks if the game has ended by insufficient material.
         *
         * @return True, if draw by insufficient material, else false.
         */
        bool insufficientMaterial();

        void removePiece(const Square square, const PieceType piece_captured); // NOTE might update

        void updateCastling(const Square start, const Square end);
        void updateEnPassant(const bool clear);
        void updateHalfmove(const bool zero);
        void updateFullmove();
        void updateTurn();




        // EOG checks
        bool isThreeFoldRep();

        // Miscellaneous
        void incrementHash();
        void decrementHash(Hash);
};

class MoveList {
    public:
        MoveList(Position&);
        uint64_t size();
        bool contains(Move move);
        Move randomMove();

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

        uint moves_index = 0;
        MoveSet moveSets[MOVESET_SIZE];
        const Move* endMove = nullptr; // Dummy move for end of iterator. Just need the address.
};

std::string concatFEN(std::vector<std::string> strings);

/**
 * Generate zobrist piece hashes using a predefined seed.
 * @return: Array of size 12 (each piece for each colour) containing arrays of size SQUARE_COUNT Hashes.
 */
std::array<std::array<Hash, SQUARE_COUNT>, 12> generatePieceHashes();

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