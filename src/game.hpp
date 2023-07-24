#pragma once

#include <cmath>

#include "constants.hpp"
#include "movegen.hpp"

/**
 * Forward declarations.
 */
class MoveList;

/**
 * A struct representing the current board position.
 */
class Position {
	public:
		/**
		 * @brief Initialise the Position object. Uses a FEN string of a normal starting position if none is given.
		 *
		 * @param fen FEN string of the state to instantiate the object with. Default is the standard starting position.
		 */
		Position(const std::string fen = STANDARD_GAME);

		/**
		 * @brief Parses the given FEN string.
		 *
		 * @param fen The FEN string to parse.
		 */
		void parseFen(const std::string fen);

		/**
		 * @brief Zeros out the class variables to make way for parsing a new FEN string.
		 */
		void resetPosition();

		/**
		 * @brief Check if the game is finished.
		 *
		 * @return Current game state.
		 */
		ExitCode isEOG(MoveList& moves) const;

		/**
		 * @brief Check if a game is in a drawn position.
		 *
		 * @param moves Moves of the current position.
		 * @return ExitCode of the current position.
		 */
		ExitCode isDraw(MoveList& moves) const;

		/**
		 * @brief Check if the current position is a checkmate.
		 *
		 * @param moves Moves of the current position.
		 * @return ExitCode of the current position.
		 */
		ExitCode isCheckmate(MoveList& moves) const;

		/**
		 * @brief Check if the current position is a draw by stalemate.
		 *
		 * @param move_list The movelist of the current position.
		 * @return True if the current position is stalemate, else false.
		 */
		bool isDrawStalemate(MoveList& move_list) const;

		/**
		 * @brief Retrives all legal moves of the current position.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getMoves(MoveList& moves);

		/**
		 * @brief Performs the given move on the board position.
		 *
		 * @param move The move to perform.
		 * @param hash Whether or not to hash the positions. Defaulted to true.
		 */
		void makeMove(const Move move, const bool hash = true);

		/**
		 * @brief Undoes the last move made.
		 */
		void undoMove();

		/**
		 * @brief Display the board position and information.
		 */
		void display(const bool letterMode = false) const;

		/**
		 * @brief Display the bitboards. Used for debugging.
		 */
		void displayBitboards() const;

		/**
		 * @brief Get the current turn.
		 *
		 * @return Turn of current position.
		 */
		inline const Player getTurn() const {
			return turn;
		}

		/**
		 * @brief Get the hash.
		 *
		 * @return Hash of current position.
		 */
		inline const Hash getHash() const {
			return this->hash;
		}

		/**
		 * @brief Get the history vector.
		 *
		 * @return Reference to history vector.
		 */
		inline const std::vector<History>& getHistory() const {
			return this->history;
		}

		/**
		 * @brief Get the positionCounts map.
		 *
		 * @return Reference to the positionCounts map.
		 */
		inline const std::unordered_map<Bitboard, int>& getPositionCounts() const {
			return this->positionCounts;
		}

		/**
		 * @brief Get the castling permissions.
		 *
		 * @return Integer representing castling permissions.
		 */
		inline const uint getCastling() const {
			return this->castling;
		}

		/**
		 * @brief Get the piece count.
		 *
		 * @return Piece count.
		 */
		inline const uint getPieceCnt() const {
			return this->pieceCnt;
		}

		/**
		 * @brief Get the queen count.
		 *
		 * @return Queen count.
		 */
		inline const uint getQueenCount() const {
			return this->pieceIndex[W_QUEEN] + this->pieceIndex[B_QUEEN];
		}

		/**
		 * @brief Get the index of the next available slot in the piece list. The index will also be the current size of
		 * the piece list.
		 *
		 * @tparam T Piece type.
		 * @return Index of the next available slot/current piece list size.
		 */
		template <PieceType T>
		inline const uint getPieceIndex() const {
			return this->pieceIndex[T];
		}

		/**
		 * @brief Get the square on the board of the piece at the given index.
		 *
		 * @tparam T Piece type.
		 * @param index Index of the piece in the piece list.
		 * @return Square which the piece is currently on.
		 */
		template <PieceType T>
		inline const Square getPieceSquare(const uint index) const {
			return this->pieceList[T][index];
		}

		/**
		 * @brief Clears the history and positionCount data. Should be called when the ucinewgame command is received.
		 * Leaves the positionCounts object with a single entry with a count of 1 for the current position.
		 */
		void clearData();

		/**
		 * @brief Convert a string in algebraic move format to a Move object and return it.
		 *
		 * @param string Move string in algebraic notation.
		 * @return Move object.
		 */
		Move getMovefromAlgebraic(const std::string& string) const;

	private:
		// Non-position information
		Player turn;
		uint castling;
		Square enPassant;
		uint halfmove, fullmove;

		// Bitboards
		Bitboard sides[PLAYER_COUNT];
		Bitboard kings, queens, rooks, bishops, knights, pawns;
		Bitboard rookPins;
		Bitboard bishopPins;
		Bitboard checkRays;
		Bitboard checkers;
		Bitboard rookEpPins;

		// Piece positions
		uint pieceIndex[PIECE_TYPE_COUNT];
		Square pieceList[PIECE_TYPE_COUNT][MAX_PIECE_COUNT];
		PieceType pieces[SQUARE_COUNT];

		// Piece counts for insufficient material checks.
		uint pieceCnt;
		uint knightCnt;
		uint bishopCnt;
		uint lightBishopCnt;
		uint darkBishopCnt;

		// Position history
		std::vector<History> history;
		std::unordered_map<Hash, int> positionCounts;
		Hash hash;

		// Auxiliary variable to assist with en passant hashing.
		bool epHashed;

		/**
		 * @brief Check if the move will be a capturing move.
		 *
		 * @param move The move to check.
		 * @return True if the move will be a capturing move, else false.
		 */
		inline bool capturingMove(const Move move) {
			return this->pieces[end(move)] != NO_PIECE;
		}

		/**
		 * @brief Moves the piece from start to end square. Updates the pieceList and pieces array for the moved piece.
		 * Does not take into account possible captures or promotions (i.e. assumes a normal move).
		 *
		 * @tparam T Piece type.
		 * @param start Start square.
		 * @param end End square.
		 */
		template <PieceType T>
		void movePiece(const Square start, const Square end);

		/**
		 * @brief Get the piece type based on the base piece type, the side to move, and the enemy argument. E.g. if it
		 * is white's turn to move, the template parameter is KING, and the argument enemy = false, then the function
		 * returns W_KING. If instead enemy = true, it returns B_KING.
		 *
		 * @tparam T Base piece type.
		 * @param enemy Whether or not to get the enemy piece instead. Default is false.
		 * @return The piece type.
		 */
		template<BasePieceType T>
		inline PieceType getPieceType(const bool enemy = false) const;

		/**
		 * @brief Checks if the specified castling bit is set.
		 *
		 * @tparam C The castling bit to check.
		 * @return True if the castling bit still exists, else false.
		 */
		template <Castling C>
		bool castleBit() const;

		/**
		 * @brief Return the character for the dark square of the printed board.
		 *
		 * @param str The character of the piece to display.
		 * @return A string of the dark square with the piece character.
		 */
		std::string darkSquare(const std::string str) const;

		/**
		 * @brief Return the character for the light square of the printed board.
		 *
		 * @param str The character of the piece to display.
		 * @return A string of the light square with the piece character.
		 */
		std::string lightSquare(const std::string str) const;

		/**
		 * @brief Get the ANSI characters to display a single square.
		 *
		 * @param square The square to display on (used for distinguishing light and dark squares).
		 * @param letterMode Whether or to display in letter mode.
		 * @return The string to display the square with the piece.
		 */
		std::string getSquareCharacters(const Square square, const bool letterMode) const;

		/**
		 * @brief Parse the turn part of the fen and set the turn variable accordingly.
		 *
		 * @param fenMove Turn substring of the FEN string.
		 */
		void parseFenMove(const std::string& fenMove);

		/**
		 * @brief Parse the castling part of the fen and set the castling variable accordingly.
		 *
		 * @param fenMove Castling substring of the FEN string.
		 */
		void parseFenCastling(const std::string& fenCastling);

		/**
		 * @brief Parse the en passant part of the fen and set the en passant variable accordingly.
		 *
		 * @param fenMove En passant substring of the FEN string.
		 */
		void parseFenEnPassant(const std::string& fenEnPassant);

		/**
		 * @brief Parse the halfmove/fullmove part of the fen and set the variables accordingly.
		 *
		 * @param fenMove Halfmove and fullmove substrings of the FEN string.
		 */
		void parseFenMoves(const std::string& halfmove, const std::string& fullmove);

		/**
		 * @brief Adds a piece to the Position object. Updates the pieceList, pieceIndex and pieces arrays as well as
		 * the corresponding piece bitboard.
		 *
		 * @param pieceBB The corresponding piece bitboard to update.
		 * @param piece Piece type to add.
		 * @param squareIndex The square index of the piece.
		 */
		void addFenPiece(Bitboard& pieceBB, const PieceType piece, const int squareIndex);

		/**
		 * @brief Parses a character from the FEN string representing a piece and updates the internal variables.
		 *
		 * @param piece Char representing the piece to add.
		 * @param squareIndex The square index of the piece.
		 */
		void parseFenChar(const char piece, const int squareIndex);

		/**
		 * @brief Get bitboard of all pieces on the board.
		 *
		 * @return Bitboard of all pieces.
		 */
		inline Bitboard getPieces() const {
			return this->sides[WHITE] | this->sides[BLACK];
		}

		/**
		 * @brief Get the king square of the player to move.
		 *
		 * @return The king square of the player to move.
		 */
		inline Square getKingSquare() const {
			return this->pieceList[this->turn][KING_INDEX];
		}

		/**
		 * @brief Checks if the specified square is attacked by the specified player.
		 *
		 * @param square Square to check.
		 * @param player Attacking player.
		 * @param ignoreKing Ignore the defending king when calculating. Used for king check moves. Defaulted to false.
		 * @return Bitboard where the set bits indicate a piece belonging to "player" that attacks the given square.
		 */
		Bitboard isAttacked(const Square square, const Player player, const bool ignoreKing = false) const;

		/**
		 * @brief Check if a piece located on the specified square is pinned.
		 *
		 * @param square Square to check.
		 * @return True if the piece is pinned, else false.
		 */
		inline bool isPinned(const Square square) const {
			return isPinnedByRook(square) || isPinnedByBishop(square);
		}

		/**
		 * @brief Check if a piece located on the specified square is pinned by a rook (or queen in the same manner).
		 *
		 * @param square Square to check.
		 * @return True if the piece on is pinned, else false.
		 */
		inline bool isPinnedByRook(const Square square) const {
			return (ONE_BB << square) & this->rookPins;
		}

		/**
		 * @brief Check if a piece located on the specified square is pinned by a bishop (or queen in the same manner).
		 *
		 * @param square Square to check.
		 * @return True if the piece is pinned, else false.
		 */
		inline bool isPinnedByBishop(const Square square) const {
			return (ONE_BB << square) & this->bishopPins;
		}

		/**
		 * @brief Check if a pawn is pinned by a rook horizontally.
		 *
		 * @param square Square that the pawn is on.
		 * @return True if pinned by a rook/queen horizontally, else false.
		 */
		bool isPawnPinnedByRookHorizontally(const Square square) const;

		/**
		 * @brief Check if a pawn is pinned by a rook vertically.
		 *
		 * @param square Square that the pawn is on.
		 * @return True if pinned by a rook/queen vertically, else false.
		 */
		bool isPawnPinnedByRookVertically(const Square square) const;

		/**
		 * @brief Set the bitboard of checkers.
		 */
		void setCheckers();

		/**
		 * @brief Set the rookPins, bishopPins and check_ray bitboards.
		 */
		void setPinAndCheckRayBitboards();

		/**
		 * @brief Check if king of the current player to move is in double-check.
		 * @return: True if in double-check, else false.
		 */
		bool inDoubleCheck() const;

		/**
		 * @brief Check if king of current player to move is in check.
		 *
		 * @return: True if in check, else false.
		 */
		bool inCheck() const;

		/**
		 * @brief Add the vector of king moves to the move list.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getKingMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of moves to the move list for when the king is in check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getCheckMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of queen moves to the move list for when the king is in check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getQueenCheckedMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of rook moves to the move list for when the king is in check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getRookCheckedMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of bishop moves to the move list for when the king is in check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getBishopCheckedMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of knight moves to the move list for when the king is in check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getKnightCheckedMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of pawn moves to the move list for when the king is in check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getPawnCheckedMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of moves to the move list for when there is no check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getNormalMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of queen moves to the move list for when there is no check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getQueenMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of rook moves to the move list for when there is no check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getRookMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of bishop moves to the move list for when there is no check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getBishopMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of knight moves to the move list for when there is no check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getKnightMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of castling moves to the move list.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getCastlingMoves(MoveList& moves) const;

		/**
		 * @brief Check if a white side king castle is valid in the current position.
		 *
		 * @return Return true if valid, else false.
		 */
		bool validWKSC() const;

		/**
		 * @brief Check if a white side queen castle is valid in the current position.
		 *
		 * @return Return true if valid, else false.
		 */
		bool validWQSC() const;

		/**
		 * @brief Check if a black side king castle is valid in the current position.
		 *
		 * @return Return true if valid, else false.
		 */
		bool validBKSC() const;

		/**
		 * @brief Check if a black side queen castle is valid in the current position.
		 *
		 * @return Return true if valid, else false.
		 */
		bool validBQSC() const;

		/**
		 * @brief Add vectors of en-passant moves to the move list for when there is no check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getEnPassantMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of en-passant check moves to the move list for when there is no check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getEnPassantCheckMoves(MoveList& moves) const;

		/**
		 * @brief Add vectors of pawn moves to the move list for when there is no check.
		 *
		 * @param moves Reference to moveList to populate.
		 */
		void getPawnMoves(MoveList& moves) const;

		/**
		 * @brief Get the reach bitboard of a rook on the specified square.
		 *
		 * @param occupancy Occupancy bitboard.
		 * @param square Square of the rook.
		 * @return Bitboard of the rook reach from the square.
		 */
		Bitboard getRookReachBB(const Bitboard occupancy, const Square square) const;

		/**
		 * @brief Get the reach bitboard of a bishop on the specified square.
		 *
		 * @param occupancy Occupancy bitboard.
		 * @param square Square of the bishop.
		 * @return Bitboard of the bishop reach from the square.
		 */
		Bitboard getBishopReachBB(const Bitboard occupancy, const Square square) const;

		/**
		 * @brief Add vector of moves to the move list for a diagonally pinned queen/bishop on the specified
		 * square.
		 *
		 * @param moves Reference to moveList to populate.
		 * @param square Square of the piece.
		 */
		void getBishopPinMoves(MoveList& moves, const Square square) const;

		/**
		 * @brief Add vector of moves to the move list for a horizontally or vertically pinned queen/bishop on the
		 * specified square.
		 *
		 * @param moves Reference to moveList to populate.
		 * @param square Square of the piece.
		 */
		void getRookPinMoves(MoveList& moves, const Square square) const;

		/**
		 * @brief Check if the given square is occupied.
		 *
		 * @param square Square to check.
		 * @return True if the square is occupied, else false.
		 */
		Bitboard isOccupied(const Square square) const;

		/**
		 * @brief Initialises the hash for the position. Assumes that the FEN string has been parsed.
		 */
		void initialiseHash();

		/**
		 * @brief Save the move to history. Must be called before making the move.
		 *
		 * @param move Move to save.
		 */
		void saveHistory(const Move move);

		/**
		 * @brief Get the promotion piece type.
		 *
		 * @param move Move from which to retrieve the promotion piece type.
		 * @return Promotion piece type.
		 */
		inline PieceType getPromotionPiece(const Move& move) const;

		/**
		 * @brief Make a move of the specified class and type.
		 *
		 * @tparam C Class of the move to make.
		 * @tparam T Type of the move to make.
		 * @param move Move to make.
		 */
		template<MoveClass C, MoveType T>
		void makeMove(const Move move);

		/**
		 * @brief Undo a move of the specified type.
		 *
		 * @tparam T Type of move to undo.
		 */
		template<MoveClass T>
		void undoMove();

		/**
		 * @brief Make a castling move.
		 *
		 * @tparam KS Start square of the king.
		 * @tparam KE End square of the king.
		 * @tparam RS Start square of the rook.
		 * @tparam RE End square of the rook.
		 * @tparam K King piece type.
		 * @tparam R Rook piece type.
		 */
		template <Square KS, Square KE, Square RS, Square RE, PieceType K, PieceType R>
		void makeCastlingMove();

		/**
		 * @brief Remove the piece from the specified square. Updates the pieceList, pieceIndex and piece arrays.
		 *
		 * @tparam T Piece type to remove.
		 * @param square Square of the piece.
		 */
		template <PieceType T>
		void removePiece(const Square square);

		/**
		 * @brief Add the piece to the specified square. Updates the pieceList, pieceIndex and piece arrays.
		 *
		 * @tparam T Piece type to add.
		 * @param square Square of the piece.
		 */
		template <PieceType T>
		void addPiece(const Square square);

		/**
		 * @brief Make a promotion move. Updates the bitboards, pieceList, pieceIndex and pieces arrays.
		 *
		 * @tparam T Move type (capture/non-capture).
		 * @param move Move to make.
		 */
		template <MoveType T>
		void makePromotionMove(const Move move);

		/**
		 * @brief Update the corresponding piece bitboard.
		 *
		 * @param move Promotion move being made.
		 */
		void addToPromotionBitboard(const Move move);

		/**
		 * @brief Remove the captured piece from the corresponding piece bitboard for a promotion move.
		 *
		 * @param move Promotion move being made.
		 */
		void removeFromBitboardPromotion(const Move move);

		/**
		 * @brief Add the promotion piece type to the corresponding pieceList, pieceIndex and pieces arrays.
		 *
		 * @param move Promotion move being made.
		 */
		void addPiecePromotion(const Move move);

		/**
		 * @brief Remove the captured piece during a promotion from the corresponding pieceList, pieceIndex and
		 * pieces arrays.
		 *
		 * @param move Promotion move being made.
		 */
		void removePiecePromotion(const Move move);

		/**
		 * @brief Move a piece from the start to end square.
		 *
		 * @param piece Piece type to move.
		 * @param start Starting square.
		 * @param end Ending square.
		 */
		void movePieceAndUpdateBitboard(const PieceType piece, const Square start, const Square end);

		/**
		 * @brief Replace a captured piece.
		 *
		 * @param piece Piece type to replace.
		 * @param square Square to put the piece.
		 */
		void placeCapturedPiece(const PieceType piece, const Square square);

		/**
		 * @brief Remove piece from the specified square.
		 *
		 * @param square Square of the piece.
		 * @param piece_captured Piece type to remove.
		 */
		void removePiece(const Square square, const PieceType piece_captured);

		/**
		 * @brief Update the castling permissions after a move.
		 *
		 * @param start Starting square of the move.
		 * @param end Ending square of the move.
		 */
		void updateCastling(const Square start, const Square end);

		/**
		 * @brief Update the en-passant state. This must be called before updates to the bitboards, pieces and
		 * pieceList.
		 *
		 * @param move The move being made.
		 */
		void updateEnPassant(const Move move);

		/**
		 * @brief Update the en-passant hash. This must be called at the end of each call to
		 * makeMove(const Move move, const bool hash).
		 */
		void updateEnPassantHash();

		/**
		 * @brief Update the halfmove counter. This must be called before updates to the pieces array.
		 *
		 * @param move The move being made.
		 */
		void updateHalfmove(const Move move);

		/**
		 * @brief Update the fullmove counter.
		 */
		void updateFullmove();

		/**
		 * @brief Update the piece, knight and bishop counts.
		 *
		 * @param piece_captured The piece that is being captured (if any).
		 * @param move The move to make.
		 */
		void updatePieceCounts(const PieceType piece_captured, const Move move);

		/**
		 * @brief Update the turn.
		 */
		void updateTurn();

		/**
		 * @brief Check if the current position is a draw by threefold repetition.
		 *
		 * @return True if draw by threefold repetition, else false.
		 */
		bool isDrawThreeFoldRep() const;

		/**
		 * @brief Check if the current position is a draw by fifty move rule.
		 *
		 * @return True if draw by fifty move rule, else false.
		 */
		inline bool isDrawFiftyMoveRule() const {
			return this->halfmove == 100;
		}

		/**
		 * @brief Check if the game has ended by insufficient material.
		 *
		 * @return True if draw by insufficient material, else false.
		 */
		bool isDrawInsufficientMaterial() const;

		/**
		 * @brief Increment the current position count.
		 */
		void incrementPositionCounter();

		/**
		 * @brief Decrement the current position count.
		 *
		 * @param hash Hash of the position count to decrement.
		 */
		void decrementPositionCounter(Hash hash);

		/**
		 * @brief Check if the pawn on the given square can make a valid en passant capture on the current en passant
		 * square. To make this check, it checks the following:
		 * 1 - There is a pawn of the current player to move on the given square.
		 * 2 - The pawn is not pinned vertically.
		 * 3 - The pawn is not pinned horizontally (i.e. both the capturing and captured pawn are not pinned).
		 * 4 - The pawn is not pinned diagonally on the off-diagonal from the capturing diagonal.
		 *
		 * @param square The square of the potential capturing pawn.
		 * @return True if a valid en passant capture can be made by the pawn on the given square, else false.
		 */
		bool validEnPassantMove(const Square square) const;
};

class MoveList {
	public:
		/**
		 * @brief Contruct a movelist for the current position.
		 */
		MoveList(Position&);

		/**
		 * @brief Get the number of moves in the moveslist.
		 *
		 * @return Number of moves.
		 */
		uint64_t size() const;

		/**
		 * @brief Check if the movelist contains the specified move.
		 *
		 * @param move Move to look for.
		 * @return True if the movelist contains the move, else false.
		 */
		bool contains(const Move move) const;

		/**
		 * @brief Get a random move.
		 *
		 * @return A random move from the movelist.
		 */
		Move randomMove() const;

		/**
		 * @brief Add a moveset.
		 *
		 * @param moveset The moveset to add.
		 */
		void addMoves(const MoveSet& moveset);

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

/**
 * @brief Generate zobrist piece hashes using a predefined seed.
 *
 * @return Array of size 12 (each piece for each colour) containing arrays of size SQUARE_COUNT Hashes.
 */
std::array<std::array<Hash, SQUARE_COUNT>, PIECE_TYPE_COUNT> generatePieceHashes();

/**
 * @brief Generate zobrist hashes for the turn using predefined seed.
 *
 * @return Hash representing turn.
 */
Hash generateTurnHash();

/**
 * @brief Generate zobrist hashes for different castling permissions combinations using a predefined seed.
 *
 * @return Array of size 16 (the number of possible castling permission combinations) of Hashes.
 */
std::array<Hash, CASTLING_COMBOS> generateCastlingHash();

/**
 * @brief Generate zobrist hashes for en passant file using predefined seed. The colour of the player who can capture
 * en-passant is taken care of by the turn hash.
 *
 * @return Array of size 8 (number of files) of Hashes.
 */
std::array<Hash, FILE_COUNT> generateEnPassantHash();