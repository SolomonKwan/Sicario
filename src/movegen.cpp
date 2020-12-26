
#include "bishop_moves.hpp"
#include "rook_moves.hpp"
#include "knight_moves.hpp"
#include "king_moves.hpp"
#include "pawn_moves.hpp"

#include <bitset>
#include <algorithm>
#include <iostream>

/**
 * Prints a readable version of a move.
 * @param move: The move to be printed.
 */
void Game::printMove(uint16_t move, bool extraInfo) {
    if (extraInfo) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
            0b111111] << " " << moveName[(move >> 12) & 0b11] << " " << 
            promoName[(move >> 14) & 0b11] << '\n';
    } else if ((move >> 12) & 0b11) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
            0b111111] << promoName[(move >> 14) & 0b11] << '\n';
    } else {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
            0b111111] << '\n';
    }
}

/**
 * @param pos: A bitboard.
 */
void displayBB(uint64_t pos) {
    // Convert number to binary string.
    std::string positionString = std::bitset<64>(pos).to_string();
    
    // Reverse each line then print.
    std::cout << "\n";
    for (int i = 0; i < 8; i++) {
        std::string line = positionString.substr(8 * i, 8);
        std::reverse(line.begin(), line.end());
        std::cout << line << '\n';
    }

    std::cout << '\n';
}

/**
 * Displays the bitboard.
 * @param game: A pointer to the game struct.
 * @param args: The command line arguments.
 */
void Game::display(CmdLine* args) {
    bool light_mode = !(args->dark_mode);
    std::cout << '\n';
    
    // Print the pieces
    for (int rank = 7; rank >= 0; rank--) {
        std::string rank_string = "";
        rank_string += (rank - 7) + '8';
        rank_string += " ";
        for (int file = 0; file < 8; file++) {
            int square = 8 * rank + file;
            PieceType piece = this->pieces[square];
            if (piece == W_KING || piece == B_KING) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265A " : "\u2654 ");
                } else {
                    rank_string += (light_mode ? "\u2654 " : "\u265A ");
                }
            } else if (piece == W_QUEEN || piece == B_QUEEN) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265B " : "\u2655 ");
                } else {
                    rank_string += (light_mode ? "\u2655 " : "\u265B ");
                }
            } else if (piece == W_ROOK || piece == B_ROOK) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265C " : "\u2656 ");
                } else {
                    rank_string += (light_mode ? "\u2656 " : "\u265C ");
                }
            } else if (piece == W_BISHOP || piece == B_BISHOP) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265D " : "\u2657 ");
                } else {
                    rank_string += (light_mode ? "\u2657 " : "\u265D ");
                }
            } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265E " : "\u2658 ");
                } else {
                    rank_string += (light_mode ? "\u2658 " : "\u265E ");
                }
            } else if (piece == W_PAWN || piece == B_PAWN) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265F " : "\u2659 ");
                } else {
                    rank_string += (light_mode ? "\u2659 " : "\u265F ");
                }
            } else {
                rank_string += "  ";
            }
        }
        std::cout << rank_string << '\n';
    }
    std::cout << "  a b c d e f g h\n";

    // Print turn
    if (this->turn) {
        std::cout << "Turn: white\n";
    } else {
        std::cout << "Turn: black\n";
    }

    // Print castling
    std::string castling = "Castling: ";
    if (this->castling & (1 << WKSC)) castling += "K";
    if (this->castling & (1 << WQSC)) castling += "Q";
    if (this->castling & (1 << BKSC)) castling += "k";
    if (this->castling & (1 << BQSC)) castling += "q";
    if (!(this->castling & (1 << WKSC)) && !(this->castling & (1 << WQSC)) && 
            !(this->castling & (1 << BKSC)) && 
            !(this->castling & (1 << BQSC))) {
        castling += "-";
    }
    std::cout << castling << '\n';

    // Print en-passant
    std::string ep = "En-passant: ";
    if (this->en_passant == NONE) {
        ep += "-\n";
    } else {
        ep += squareName[this->en_passant] + "\n";
    }
    std::cout << ep;

    // Clock cycles
    std::cout << "Halfmove: " << this->halfmove << '\n';
    std::cout << "Fullmove: " << this->fullmove << '\n';
}

/**
 * Display all individual game position information.
 * @param game: Pointer to game struct.
 */
void displayAll(Game* game) {
    std::cout << "White";
    displayBB(game->sides[WHITE]);

    std::cout << "Black";
    displayBB(game->sides[BLACK]);

    std::cout << "Kings";
    displayBB(game->kings);

    std::cout << "Queens";
    displayBB(game->queens);

    std::cout << "Rooks";
    displayBB(game->rooks);

    std::cout << "Bishops";
    displayBB(game->bishops);

    std::cout << "Knights";
    displayBB(game->knights);

    std::cout << "pawns";
    displayBB(game->pawns);

    std::cout << (game->turn ? "White" : "Black") << '\n';

    std::cout << "Castling: " << std::bitset<4>(game->castling) << '\n';

    std::cout << "En-passant: " << squareName[game->en_passant] << '\n';

    std::cout << "Halfmove: " << game->halfmove << '\n';
    
    std::cout << "Fullmove: " << game->fullmove << "\n\n";

    for (int i = 0; i < 12; i++) {
        std::cout << piece_type_string[i] << " " << game->piece_index[i] << 
                " :";
        for (int j = 0; j < game->piece_index[i]; j++) {
            std::cout << squareName[game->piece_list[i][j]] << " ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    for (int i = 0; i < 64; i++) {
        std::cout << squareName[i] << ":" << piece_type_string[game->pieces[i]] 
                << " ";
        if (i % 8 == 7) std::cout << '\n';
    }
}

/**
 * Finds and returns the position of the most significant set bit. If not bits
 * are set, returns 0.
 * 
 * @param n: The integer to find the MSB.
 * @return: The most significant bit position.
 */
int MSB(int n) {
    if (n == 0) return 0;

    int msb = 0;
    while (n != 0) {
        n /= 2;
        msb++;
    }

    return msb;
}

/**
 * Returns true if the nths bit of pos is set, else false.
 * 
 * @param pos: A bitboard.
 * @param n: The wanted bit.
 * @return: True if bit is set, else false.
 */
bool bitAt(uint64_t pos, int n) {
    return ((pos >> n) & 1ULL) == 1ULL;
}

/**
 * Calculates the index into the move_set of a particular move family.
 * 
 * @param masked_reach: A bitboard of the reach of the piece with own pieces
 *      masked out.
 * @param move_family: A pointer to Moves struct holding the move family. The
 *      block_bits vector MUST hold the squares in order from smallest to 
 *      largest.
 * @return: Index into the move_set.
 */
int moveSetIndex(uint64_t masked_reach, Moves* move_family) {
    int index = 0, result = 0;
    for (auto i = move_family->block_bits.begin(); i != move_family->
            block_bits.end(); i++) {
        result |= ((masked_reach >> *i) & 1ULL) << index;
        index++;
    }
    return result;
}

/**
 * Compute the castling moves.
 * @param CASTLING_MOVES: Array of move struct pointers.
 */
void Game::computeCastling(Moves* CASTLING_MOVES) {
    CASTLING_MOVES[WQSC].move_set.resize(1);
    std::vector<uint16_t>* moves_set = &(CASTLING_MOVES[WQSC].move_set[0]);
    moves_set->push_back(E1 | C1 << 6 | CASTLING | pKNIGHT);
    
    CASTLING_MOVES[WKSC].move_set.resize(1);
    moves_set = &CASTLING_MOVES[WKSC].move_set[0];
    moves_set->push_back(E1 | G1 << 6 | CASTLING | pKNIGHT);

    CASTLING_MOVES[BQSC].move_set.resize(1);
    moves_set = &CASTLING_MOVES[BQSC].move_set[0];
    moves_set->push_back(E8 | C8 << 6 | CASTLING | pKNIGHT);

    CASTLING_MOVES[BKSC].move_set.resize(1);
    moves_set = &CASTLING_MOVES[BKSC].move_set[0];
    moves_set->push_back(E8 | G8 << 6 | CASTLING | pKNIGHT);
}

/**
 * Calls all functions to make the precomputations for a game.
 * @param moves: Pointer to precomputed moves structs.
 */
void precompute(Computed* moves) {
    Game::computeRookMoves(moves->ROOK_INDEX, moves->ROOK_MOVES);
    Game::computeRookBlocks(moves->ROOK_BLOCKS);
    Game::computeBishopMoves(moves->BISHOP_INDEX, moves->BISHOP_MOVES);
    Game::computeBishopBlocks(moves->BISHOP_BLOCKS);
    Game::computeKnightMoves(moves->KNIGHT_MOVES);
    Game::computeKingMoves(moves->KING_MOVES);
    Game::computePawnMoves(moves->PAWN_MOVES, moves->EN_PASSANT_MOVES, moves->
            DOUBLE_PUSH);
    Game::computeCastling(moves->CASTLING_MOVES);
}

/**
 * Calls the function to perform pre-game move computations.
 */
void Compute::init(Computed* moves) {
    precompute(moves);
}

/**
 * Sets all the legal moves a king can make in a given position.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getKingMoves(Computed* computed_moves, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, 
        uint64_t kEnemy_attacks) {
    Moves* king_family = &computed_moves->KING_MOVES[this->piece_list[this->turn][0]];
    std::vector<uint16_t>* move_set = &king_family->move_set[
            moveSetIndex((king_family->reach ^ this->sides[this->turn]) & 
            ~kEnemy_attacks, king_family)];
    if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
}

/**
 * Get rook or queen moves if piece is pinned horizontally or vertically.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the piece pinned.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getRookPinMoves(Computed* moves, int square, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index) {
    int king = this->piece_list[this->turn][0];
    uint64_t pos = this->sides[BLACK] | this->sides[WHITE];
    uint64_t friendly = 0;
    if (square == A1 || square == A8 || square == H1 || square == H8) {
        return;
    } else if (square / 8 == 0) {
        pos |= files[square % 8];
        friendly |= 1ULL << (square + 8);
    } else if (square / 8 == 7) {
        pos |= files[square % 8];
        friendly |= 1ULL << (square - 8);
    } else if (square % 8 == 0) {
        pos |= ranks[square / 8];
        friendly |= 1ULL << (square + 1);
    } else if (square % 8 == 7) {
        pos |= ranks[square / 8];
        friendly |= 1ULL << (square - 1);
    } else { // Middle
        if (king % 8 == square % 8) { // Vertical pin
            pos |= ranks[square / 8];
            friendly |= 1ULL << (square + 1);
            friendly |= 1ULL << (square - 1);
        } else { // Horizontal pin
            pos |= files[square % 8];
            friendly |= 1ULL << (square + 8);
            friendly |= 1ULL << (square - 8);
        }
    }
    Moves* rook_moves = &moves->ROOK_MOVES[moves->ROOK_INDEX[square][this->rookIndex(
            pos, (Square) square)]];
    std::vector<uint16_t>* move_set = &rook_moves->move_set[moveSetIndex(
            rook_moves->reach ^ (this->sides[this->turn] | friendly), 
            rook_moves)];
    if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
}

/**
 * Get bishop or queen moves if piece is pinned diagonally.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the piece pinned.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getBishopPinMoves (Computed* moves, int square, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index) {
    int king = this->piece_list[this->turn][0];
    uint64_t pos = this->sides[BLACK] | this->sides[WHITE];
    uint64_t friendly = 0;
    if (square / 8 == 0 || square / 8 == 7 || square % 8 == 0 || 
            square % 8 == 7) {
        return;
    } else {
        if ((king > square && (king % 8) > (square % 8)) || (king < square && 
                (king % 8) < (square % 8))) { // [/] pin
            pos |= 1ULL << (square + 7) | 1ULL << (square - 7);
            friendly |= 1ULL << (square + 7);
            friendly |= 1ULL << (square - 7);
        } else { // [\] pin
            pos |= 1ULL << (square + 9) | 1ULL << (square - 9);
            friendly |= 1ULL << (square + 9);
            friendly |= 1ULL << (square - 9);
        }
    }
    Moves* bishop_moves = &moves->BISHOP_MOVES[moves->BISHOP_INDEX[square]
            [this->bishopIndex(pos, (Square) square)]];
    std::vector<uint16_t>* move_set = &bishop_moves->move_set[moveSetIndex(
            bishop_moves->reach ^ (this->sides[this->turn] | friendly), 
            bishop_moves)];
    if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
}

/**
 * Get queen moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getQueenMoves(Computed* computed_moves, uint64_t rook_pins, 
        uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], 
        int* moves_index) {
    PieceType piece = this->turn ? W_QUEEN : B_QUEEN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int queen = this->piece_list[piece][i];
        if (rook_pins & (1ULL << queen)) { // Pinned.
            this->getRookPinMoves(computed_moves, queen, pos_moves, 
                    moves_index);
        } else if (bishop_pins & (1ULL << queen)) { // Pinned.
            this->getBishopPinMoves(computed_moves, queen, pos_moves, 
                    moves_index);
        } else {
            Moves* rook_like_moves = this->getRookFamily(computed_moves, 
                    (Square) queen);
            std::vector<uint16_t>* move_set = &rook_like_moves->move_set[
                    moveSetIndex(rook_like_moves->reach ^ this->sides[
                    this->turn], rook_like_moves)];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;

            Moves* bishop_like_moves = this->getBishopFamily(computed_moves, 
                    (Square) queen);
            move_set = &bishop_like_moves->move_set[
                    moveSetIndex(bishop_like_moves->reach ^ this->sides[
                    this->turn], bishop_like_moves)];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }
    }
}

/**
 * Get rook moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getRookMoves(Computed* computed_moves, uint64_t rook_pins, 
        uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], 
        int* moves_index) {
    PieceType piece = this->turn ? W_ROOK : B_ROOK;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int rook = this->piece_list[piece][i];
        if (rook_pins & (1ULL << rook)) { // Pinned.
            this->getRookPinMoves(computed_moves, rook, pos_moves, moves_index);
        } else if (bishop_pins & (1ULL << rook)) {
            continue;
        } else {
            Moves* rook_like_moves = this->getRookFamily(computed_moves, 
                    (Square) rook);
            std::vector<uint16_t>* move_set = &rook_like_moves->move_set[
                    moveSetIndex(rook_like_moves->reach ^ this->
                    sides[this->turn], rook_like_moves)];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }
    }
}

/**
 * Get bishop moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getBishopMoves(Computed* computed_moves, uint64_t rook_pins, 
        uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], 
        int* moves_index) {
    PieceType piece = this->turn ? W_BISHOP : B_BISHOP;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int bishop = this->piece_list[piece][i];
        if (rook_pins & (1ULL << bishop)) { // Pinned.
            continue;
        } else if (bishop_pins & (1ULL << bishop)) { // Pinned.
            this->getBishopPinMoves(computed_moves, bishop, pos_moves, 
                    moves_index);
        } else {
            Moves* bishop_like_moves = this->getBishopFamily(computed_moves, 
                    (Square) bishop);
            std::vector<uint16_t>* move_set = &bishop_like_moves->move_set[
                    moveSetIndex(bishop_like_moves->reach ^ this->sides[
                    this->turn], bishop_like_moves)];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }
    }
}

/**
 * Get knight moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getKnightMoves(Computed* computed_moves, uint64_t rook_pins, 
        uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], 
        int* moves_index) {
    PieceType piece = this->turn ? W_KNIGHT : B_KNIGHT;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int knight = this->piece_list[piece][i];
        if (rook_pins & (1ULL << knight) || bishop_pins & (1ULL << knight)) { // Pinned.
            continue;
        } else {
            Moves* knight_moves = &computed_moves->KNIGHT_MOVES[knight];
            std::vector<uint16_t>* move_set = &knight_moves->move_set[
                    moveSetIndex(knight_moves->reach ^ this->sides[
                    this->turn], knight_moves)];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }
    }
}

/**
 * Get pawn moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getPawnMoves(Computed* computed_moves, uint64_t rook_pins, 
        uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], 
        int* moves_index) {
    Square king = this->piece_list[this->turn][0];
    PieceType piece = this->turn ? W_PAWN : B_PAWN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int pawn = this->piece_list[piece][i];
        if (rook_pins & (1ULL << pawn)) { // Pinned
            if (pawn % 8 == king % 8) {
                Moves* pawn_moves = &computed_moves->PAWN_MOVES[this->turn][
                        pawn - 8];
                std::vector<uint16_t>* move_set = &pawn_moves->move_set[
                        moveSetIndex((this->sides[WHITE] | 
                        this->sides[BLACK]) & files[pawn % 8], pawn_moves)];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            } else continue;
        } else if (bishop_pins & (1ULL << pawn)) { // Pinned
            int ray = pawn > king ? (pawn % 8 < king % 8 ? 7 : 9) : (pawn % 8 < 
                    king % 8 ? -9 : -7) ;
            if ((this->turn == BLACK && ray > 0) || (this->turn == WHITE && 
                    ray < 0)) {
                continue;
            }
            int square = (Square) pawn + ray;
            uint64_t enemies = this->sides[1 - this->turn];

            if (enemies & (1ULL << square)) {
                int advance = this->turn ? 8 : -8;
                uint64_t pos = (1ULL << (pawn + advance)) | (1ULL << (pawn + 
                        ray));
                Moves* pawn_moves = &computed_moves->PAWN_MOVES[this->turn][
                        pawn - 8];
                std::vector<uint16_t>* move_set = &pawn_moves->move_set[
                        moveSetIndex(pos, pawn_moves)];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        } else {
            Moves* pawn_moves = &computed_moves->PAWN_MOVES[this->turn][pawn - 
                    8];
            std::vector<uint16_t>* move_set = &pawn_moves->move_set[
                    moveSetIndex(this->pawnMoveArgs((Square) pawn), 
                    pawn_moves)];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }
    }
}

/**
 * Gets the squares attacked by an enemy piece.
 * 
 * @param game: A game struct pointer.
 * @param moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param kEnemy_attacks: 64 bit unsigned int pointer with value 0.
 */
void Game::getEnemyAttacks(Computed* moves, uint64_t* enemy_attacks, 
        uint64_t* rook_pins, uint64_t* bishop_pins, uint64_t* kEnemy_attacks) {
    bool turn = this->turn;
    Square king_sq = this->piece_list[turn][0];
    uint64_t pieces = this->sides[WHITE] | this->sides[BLACK];
    uint64_t masked_king_bb = (this->sides[WHITE] | this->sides[BLACK]) ^ 
            (1ULL << king_sq);

    // Pawn attacks.
    PieceType piece = turn ? B_PAWN : W_PAWN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        *enemy_attacks |= moves->PAWN_MOVES[!turn][this->piece_list[piece][i] -
                8].reach;
        *kEnemy_attacks |= *enemy_attacks;
    }

    // Knight attacks.
    piece = turn ? B_KNIGHT : W_KNIGHT;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        *enemy_attacks |= moves->KNIGHT_MOVES[this->piece_list[piece][i]].reach;
        *kEnemy_attacks |= *enemy_attacks;
    }

    // Bishop attacks.
    uint64_t king_bishop_rays = moves->BISHOP_MOVES[moves->BISHOP_INDEX[
            king_sq][this->bishopIndex(pieces ^ (1ULL << king_sq), king_sq)]].reach;
    piece = turn ? B_BISHOP : W_BISHOP;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        *enemy_attacks |= moves->BISHOP_MOVES[moves->BISHOP_INDEX[square]
                [this->bishopIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        *kEnemy_attacks |= moves->BISHOP_MOVES[moves->BISHOP_INDEX[square][
                this->bishopIndex(masked_king_bb, (Square)square)]].reach;
        
        if (std::abs(square % 8 - king_sq % 8) == std::abs(((square / 8) % 
                8) - ((king_sq / 8) % 8))) {
            // Attacks.
            uint64_t attacks = moves->BISHOP_MOVES[moves->BISHOP_INDEX[square][
                    this->bishopIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            *bishop_pins |= king_bishop_rays & attacks;
        }
    }

    // Rook attacks.
    uint64_t king_rook_rays = moves->ROOK_MOVES[moves->ROOK_INDEX[king_sq][
            this->rookIndex(pieces ^ (1ULL << king_sq), king_sq)]].reach;
    piece = turn ? B_ROOK : W_ROOK;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        *enemy_attacks |= moves->ROOK_MOVES[moves->ROOK_INDEX[square]
                [this->rookIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        *kEnemy_attacks |= moves->ROOK_MOVES[moves->ROOK_INDEX[square][
                this->rookIndex(masked_king_bb, (Square)square)]].reach;
        if (king_sq / 8 == square / 8 || king_sq % 8 == square % 8) {
            // Attacks.
            uint64_t attacks = moves->ROOK_MOVES[moves->ROOK_INDEX[square][
                    this->rookIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            *rook_pins |= king_rook_rays & attacks;
        }
    }

    // Queen attacks.
    piece = turn ? B_QUEEN : W_QUEEN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        *enemy_attacks |= moves->BISHOP_MOVES[moves->BISHOP_INDEX[square]
                [this->bishopIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        *enemy_attacks |= moves->ROOK_MOVES[moves->ROOK_INDEX[square]
                [this->rookIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        *kEnemy_attacks |= moves->BISHOP_MOVES[moves->BISHOP_INDEX[square][
                this->bishopIndex(masked_king_bb, (Square)square)]].reach;
        *kEnemy_attacks |= moves->ROOK_MOVES[moves->ROOK_INDEX[square][
                this->rookIndex(masked_king_bb, (Square)square)]].reach;
        
        if (std::abs(square % 8 - king_sq % 8) == std::abs(((square / 8) % 
                8) - ((king_sq / 8) % 8))) {
            // Attacks.
            uint64_t attacks = moves->BISHOP_MOVES[moves->BISHOP_INDEX[square][
                    this->bishopIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            *bishop_pins |= king_bishop_rays & attacks;
        }

        if (king_sq / 8 == square / 8 || king_sq % 8 == square % 8) {
            // Attacks.
            uint64_t attacks = moves->ROOK_MOVES[moves->ROOK_INDEX[square][
                    this->rookIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            *rook_pins |= king_rook_rays & attacks;
        }
    }

    // King attacks.
    *enemy_attacks |= moves->KING_MOVES[this->piece_list[!turn][0]].reach;
    *kEnemy_attacks |= *enemy_attacks;
}

/**
 * Gets the castling moves.
 * 
 * @param game: A game struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param moves: Precomputed moves struct pointer.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getCastlingMoves(uint64_t enemy_attacks, Computed* moves, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index) {
    uint64_t sides = this->sides[WHITE] | this->sides[BLACK];
    if (this->turn) {
        if (this->castling & (1 << WKSC)) {
            if (!((1ULL << F1) & enemy_attacks) && !((1ULL << G1) & 
                enemy_attacks) && !((1ULL << F1) & sides) && !((1ULL << G1) & 
                sides)) {
                std::vector<uint16_t>* move_set = &moves->CASTLING_MOVES[WKSC].
                        move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        }

        if (this->castling & (1 << WQSC)) {
            // CmdLineArgs args;
            if (!((1ULL << D1) & enemy_attacks) && !((1ULL << C1) & 
                enemy_attacks) && !((1ULL << D1) & sides) && !((1ULL << C1) & 
                sides) && !((1ULL << B1) & sides)) {
                std::vector<uint16_t>* move_set = &moves->CASTLING_MOVES[WQSC].
                        move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        }
    } else {
        if (this->castling & (1 << BKSC)) {
            if (!((1ULL << F8) & enemy_attacks) && !((1ULL << G8) & 
                enemy_attacks) && !((1ULL << F8) & sides) && !((1ULL << G8) & 
                sides)) {
                std::vector<uint16_t>* move_set = &moves->CASTLING_MOVES[BKSC].
                        move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        }

        if (this->castling & (1 << BQSC)) {
            if (!((1ULL << D8) & enemy_attacks) && !((1ULL << C8) & 
                enemy_attacks) && !((1ULL << D8) & sides) && !((1ULL << C8) & 
                sides) && !((1ULL << B8) & sides)) {
                std::vector<uint16_t>* move_set = &moves->CASTLING_MOVES[BQSC].
                        move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        }
    }
}

/**
 * Gets en-passant moves in special cases. Such a case example is 
 * k7/8/8/1K1pP2q/8/8/8/8 w - d6 0 1
 * 
 * @param game: A game struct pointer.
 * @param king: The square that the king is on.
 * @param turn: The turn of the player.
 * @param attacker_sq: Square of the pawn that can potentially capture.
 * @param captured_pawn: Square of the pawn that can potentially be captured.
 * @param moves: Precomputed moves struct pointer.
 * @param ep: En-passant square.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::horizontalPinEp(int king, bool turn, int attacker_sq, 
        int captured_pawn, Computed* moves, int ep, std::vector<uint16_t>* 
        pos_moves[MAX_MOVE_SETS], int* moves_index) {
    int rank = (king / 8) * 8, rank_end = rank + 7;
    PieceType e_rook = turn ? B_ROOK : W_ROOK;
    PieceType e_queen = turn ? B_QUEEN : W_QUEEN;
    int e_rook_sq = -1, e_queen_sq = -1;
    uint64_t pawns = 1ULL << attacker_sq | 1ULL << captured_pawn;
    uint64_t pieces = this->sides[WHITE] | this->sides[BLACK];

    // Find potential enemy rook and queen squares on the same rank.
    for (int i = rank; i <= rank_end; i++) {
        if (this->pieces[i] == e_rook) e_rook_sq = i;
        else if (this->pieces[i] == e_queen) e_queen_sq = i;
    }

    uint64_t king_reach = moves->ROOK_MOVES[moves->ROOK_INDEX[king][this->rookIndex(
            pieces ^ pawns, (Square) king)]].reach;
    uint64_t pin_ray = 0;

    // Ray between rook and king without pawns of interest.
    uint64_t rook_reach = 0;
    if (e_rook_sq != -1) {
        rook_reach = moves->ROOK_MOVES[moves->ROOK_INDEX[e_rook_sq][this->rookIndex(
                pieces ^ pawns, (Square) e_rook_sq)]].reach;
        pin_ray |= (king_reach & rook_reach);
    }

    // Ray between queen and king without pawns of interest.
    uint64_t queen_reach = 0;
    if (e_queen_sq != -1) {
        queen_reach = moves->ROOK_MOVES[moves->ROOK_INDEX[e_queen_sq][this->rookIndex(
                pieces ^ pawns, (Square) e_queen_sq)]].reach;
        pin_ray |= (king_reach & queen_reach);
    }

    if (!(pawns & pin_ray)) {
        if (attacker_sq % 8 < ep % 8) {
            std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                    attacker_sq - 24].move_set[0];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        } else {
            std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                    attacker_sq - 24].move_set[1];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }
    }
}

/**
 * Gets en-passant moves involving diagonal pins. Such a case example is
 * k7/2q5/8/3pP3/8/6K1/8/8 w - d6 0 1 or
 * k7/6q1/8/3pP3/8/2K5/8/8 w - d6 0 1
 * 
 * @param game: A game struct pointer.
 * @param king: The square that the king is on.
 * @param turn: The turn of the player.
 * @param attacker_sq: Square of the pawn that can potentially capture.
 * @param captured_pawn: Square of the pawn that can potentially be captured.
 * @param moves: Precomputed moves struct pointer.
 * @param ep: En-passant square.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::diagonalPinEp(int king, bool turn, int attacker_sq, 
        int captured_pawn, Computed* moves, int ep, std::vector<uint16_t>* 
        pos_moves[MAX_MOVE_SETS], int* moves_index) {
    if (ep > attacker_sq && ep % 8 < attacker_sq % 8) { // Upper left.
        if ((king > attacker_sq && king % 8 < attacker_sq % 8) ||
                (king < attacker_sq && king % 8 > attacker_sq % 8)) {
            if (attacker_sq % 8 < ep % 8) {
                std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                        attacker_sq - 24].move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            } else {
                std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                        attacker_sq - 24].move_set[1];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        }
    } else if (ep > attacker_sq && ep % 8 > attacker_sq % 8) { // Upper right.
        if ((king > attacker_sq && king % 8 > attacker_sq % 8) ||
                (king < attacker_sq && king % 8 < attacker_sq % 8)) {
            if (attacker_sq % 8 < ep % 8) {
                std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                        attacker_sq - 24].move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            } else {
                std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                        attacker_sq - 24].move_set[1];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        }
    } else if (ep < attacker_sq && ep % 8 < attacker_sq % 8) { // Lower left.
        if ((king < attacker_sq && king % 8 < attacker_sq % 8) ||
                (king > attacker_sq && king % 8 > attacker_sq % 8)) {
            if (attacker_sq % 8 < ep % 8) {
                std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                        attacker_sq - 24].move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            } else {
                std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                        attacker_sq - 24].move_set[1];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        }
    } else { // Lower right.
        if ((king < attacker_sq && king % 8 > attacker_sq % 8) ||
                (king > attacker_sq && king % 8 < attacker_sq % 8)) {
            if (attacker_sq % 8 < ep % 8) {
                std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                        attacker_sq - 24].move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            } else {
                std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                        attacker_sq - 24].move_set[1];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        }
    }
}

/**
 * Gets the en-passant moves for a normal move of the game.
 * @param game: A game struct pointer.
 * @param rook_pins: Bitboard of pieces pinned horiztonally or vertically.
 * @param bishop_pins: Bitboard of pieces pinned diagonally.
 * @param moves: Precomputed moves struct pointer.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getEpMoves(uint64_t rook_pins, uint64_t bishop_pins, 
        Computed* moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS],
        int* moves_index) {
    if (this->en_passant != NONE) {
        bool turn = this->turn;
        int rank_offset = turn ? -8 : 8;
        int ep = this->en_passant;
        uint64_t own_pawns = this->sides[turn] & this->pawns;
        int king_sq = this->piece_list[turn][0];
        int captured_pawn = ep + rank_offset;

        if (ep % 8 != 0) { // Capture to right.
            int attacker_sq = ep + rank_offset - 1;
            if (own_pawns & (1ULL << attacker_sq) && 
                    !((1ULL << attacker_sq) & rook_pins)) {
                if (captured_pawn / 8 == king_sq / 8) {
                    this->horizontalPinEp(king_sq, turn, attacker_sq, 
                            captured_pawn, moves, ep, pos_moves, moves_index);
                } else if ((1ULL << attacker_sq) & bishop_pins) {
                    this->diagonalPinEp(king_sq, turn, attacker_sq, 
                            captured_pawn, moves, ep, pos_moves, moves_index);
                } else {
                    if (attacker_sq % 8 < ep % 8) {
                        std::vector<uint16_t>* move_set = &moves->
                                EN_PASSANT_MOVES[attacker_sq - 24].move_set[0];
                        if (move_set->size() != 0) {
                            pos_moves[(*moves_index)++] = move_set;
                        }
                    } else {
                        std::vector<uint16_t>* move_set = &moves->
                                EN_PASSANT_MOVES[attacker_sq - 24].move_set[1];
                        if (move_set->size() != 0) {
                            pos_moves[(*moves_index)++] = move_set;
                        }
                    }
                }
            }
        }
        
        if (ep % 8 != 7) { // Capture to left.
            int attacker_sq = ep + rank_offset + 1;
            if (own_pawns & (1ULL << attacker_sq) && 
                    !((1ULL << attacker_sq) & rook_pins)) {
                if (captured_pawn / 8 == king_sq / 8) {
                    this->horizontalPinEp(king_sq, turn, attacker_sq, 
                            captured_pawn, moves, ep, pos_moves, moves_index);
                } else if ((1ULL << attacker_sq) & bishop_pins) {
                    this->diagonalPinEp(king_sq, turn, attacker_sq, 
                            captured_pawn, moves, ep, pos_moves, moves_index);
                } else {
                    if (attacker_sq % 8 < ep % 8) {
                        std::vector<uint16_t>* move_set = &moves->
                                EN_PASSANT_MOVES[attacker_sq - 24].move_set[0];
                        if (move_set->size() != 0) {
                            pos_moves[(*moves_index)++] = move_set;
                        }
                    } else {
                        std::vector<uint16_t>* move_set = &moves->
                                EN_PASSANT_MOVES[attacker_sq - 24].move_set[1];
                        if (move_set->size() != 0) {
                            pos_moves[(*moves_index)++] = move_set;
                        }
                    }
                }
            }
        }
    }
}

/**
 * Gets all legal moves for the player whose turn it is.
 * @param game: A game struct pointer.
 * @param computed_moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getNormalMoves(Computed* moves, uint64_t* enemy_attacks, 
        uint64_t* rook_pins, uint64_t* bishop_pins, std::vector<uint16_t>*
        pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks) {
    this->getKingMoves(moves, pos_moves, moves_index, kEnemy_attacks);
    this->getQueenMoves(moves, *rook_pins, *bishop_pins, pos_moves, moves_index);
    this->getRookMoves(moves, *rook_pins, *bishop_pins, pos_moves, moves_index);
    this->getBishopMoves(moves, *rook_pins, *bishop_pins, pos_moves, moves_index);
    this->getKnightMoves(moves, *rook_pins, *bishop_pins, pos_moves, moves_index);
    this->getPawnMoves(moves, *rook_pins, *bishop_pins, pos_moves, moves_index);
    this->getCastlingMoves(*enemy_attacks, moves, pos_moves, moves_index);
    this->getEpMoves(*rook_pins, *bishop_pins, moves, pos_moves, moves_index);
}

/**
 * Gets the en-passant moves for a normal move of the game.
 * @param game: A game struct pointer.
 * @param rook_pins: Bitboard of pieces pinned horiztonally or vertically.
 * @param bishop_pins: Bitboard of pieces pinned diagonally.
 * @param checkers: The bitboard of checkers.
 * @param moves: Precomputed moves struct pointer.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getCheckedEp(uint64_t* rook_pins, uint64_t* bishop_pins, 
        uint64_t checkers, Computed* moves, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index) {
    if (this->en_passant != NONE) {
        int rank_offset = this->turn ? -8 : 8;
        int ep = this->en_passant;
        uint64_t own_pawns = this->sides[this->turn] & this->pawns;

        if (ep % 8 != 0) { // Capture from left to block.
            int attack_sq = ep + rank_offset - 1;
            if (((1ULL << ep) & checkers) && (own_pawns & 
                    (1ULL << attack_sq)) && !((1ULL << attack_sq) & 
                    *rook_pins) && !((1ULL << attack_sq) & *bishop_pins)) {
                if (attack_sq % 8 < ep % 8) {
                    std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                            attack_sq - 24].move_set[0];
                    if (move_set->size() != 0) {
                        pos_moves[(*moves_index)++] = move_set;
                    }
                } else {
                    std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                            attack_sq - 24].move_set[1];
                    if (move_set->size() != 0) {
                        pos_moves[(*moves_index)++] = move_set;
                    }
                }
            }
        }

        if (ep % 8 != 7) { // Capture from right to block.
            int attack_sq = ep + rank_offset + 1;
            if (((1ULL << ep) & checkers) && (own_pawns & 
                    (1ULL << attack_sq)) && !((1ULL << attack_sq) & 
                    *rook_pins) && !((1ULL << attack_sq) & *bishop_pins)) {
                if (attack_sq % 8 < ep % 8) {
                    std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                            attack_sq - 24].move_set[0];
                    if (move_set->size() != 0) {
                        pos_moves[(*moves_index)++] = move_set;
                    }
                } else {
                    std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                            attack_sq - 24].move_set[1];
                    if (move_set->size() != 0) {
                        pos_moves[(*moves_index)++] = move_set;
                    }
                }
            }
        }

        // Capture checker
        if (ep % 8 != 0) { // Capture checker from left
            int pawn = ep + rank_offset;
            int attack_sq = ep + rank_offset - 1;
            if (((1ULL << pawn) & checkers) && (own_pawns & (1ULL << 
                    attack_sq)) && !((1ULL << attack_sq) & *rook_pins) && 
                    !((1ULL << attack_sq) & *bishop_pins)) {
                if (attack_sq % 8 < ep % 8) {
                    std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                            attack_sq - 24].move_set[0];
                    if (move_set->size() != 0) {
                        pos_moves[(*moves_index)++] = move_set;
                    }
                } else {
                    std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                            attack_sq - 24].move_set[1];
                    if (move_set->size() != 0) {
                        pos_moves[(*moves_index)++] = move_set;
                    }
                }
            }
        }

        if (ep % 8 != 7) {
            int pawn = ep + rank_offset;
            int attack_sq = ep + rank_offset + 1;
            if (((1ULL << pawn) & checkers) && (own_pawns & (1ULL << 
                    attack_sq)) && !((1ULL << attack_sq) & *rook_pins) && 
                    !((1ULL << attack_sq) & *bishop_pins)) {
                if (attack_sq % 8 < ep % 8) {
                    std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                            attack_sq - 24].move_set[0];
                    if (move_set->size() != 0) {
                        pos_moves[(*moves_index)++] = move_set;
                    }
                } else {
                    std::vector<uint16_t>* move_set = &moves->EN_PASSANT_MOVES[
                            attack_sq - 24].move_set[1];
                    if (move_set->size() != 0) {
                        pos_moves[(*moves_index)++] = move_set;
                    }
                }
            }
        }
    }
}

/**
 * Retrieves the legal moves for when the king is in check.
 * @param game: A game struct pointer.
 * @param moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Game::getCheckedMoves(Computed* moves, uint64_t* enemy_attacks, 
        uint64_t* rook_pins, uint64_t* bishop_pins,
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, 
        uint64_t kEnemy_attacks) {    
    // MIGHT BE ABLE TO RETRIEVE WHEN CALCULATING THE enemy_attacks
    Square king_sq = this->piece_list[this->turn][0];
    uint64_t checkers_only = 0ULL;
    uint64_t check_rays_only = 
            this->getBishopCheckRays(moves, king_sq, &checkers_only) | 
            this->getRookCheckRays(moves, king_sq, &checkers_only) | 
            this->getPawnCheckers(king_sq, &checkers_only) | 
            this->getKnightCheckers(moves, king_sq, &checkers_only);
    
    // King checked moves.
    this->getKingMoves(moves, pos_moves, moves_index, kEnemy_attacks);

    // Queen checked moves.
    PieceType piece = this->turn ? W_QUEEN : B_QUEEN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int queen = this->piece_list[piece][i];
        if ((1ULL << queen) & *rook_pins || (1ULL << queen) & *bishop_pins) {
            continue;
        }
        Moves* bishop_move = &moves->BISHOP_MOVES[moves->BISHOP_INDEX[queen][
                this->bishopIndex(this->sides[BLACK] | this->sides[WHITE], 
                        (Square) queen)]];
        uint64_t bishop_index = bishop_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<uint16_t>* move_set = &moves->BISHOP_BLOCKS[queen].
                checked_moves[bishop_index];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;

        Moves* rook_move = &moves->ROOK_MOVES[moves->ROOK_INDEX[queen][
                this->rookIndex(this->sides[BLACK] | this->sides[WHITE], 
                (Square) queen)]];
        uint64_t rook_index = rook_move->reach & (check_rays_only | 
                checkers_only);
        move_set = &moves->ROOK_BLOCKS[queen].
                checked_moves[rook_index];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
    }

    // Rook checked moves.
    piece = this->turn ? W_ROOK : B_ROOK;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int rook = this->piece_list[piece][i];
        if ((1ULL << rook) & *rook_pins || (1ULL << rook) & *bishop_pins) {
            continue;
        }
        Moves* rook_move = &moves->ROOK_MOVES[moves->ROOK_INDEX[rook][
                this->rookIndex(this->sides[BLACK] | this->sides[WHITE], 
                (Square) rook)]];
        uint64_t rook_index = rook_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<uint16_t>* move_set = &moves->ROOK_BLOCKS[rook].
                checked_moves[rook_index];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
    }

    // Bishop checked moves.
    piece = this->turn ? W_BISHOP : B_BISHOP;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int bishop = this->piece_list[piece][i];
        if ((1ULL << bishop) & *rook_pins || (1ULL << bishop) & *bishop_pins) {
            continue;
        }
        // displayBB(checkers_and_rays);
        Moves* bishop_move = &moves->BISHOP_MOVES[moves->BISHOP_INDEX[bishop][
                this->bishopIndex(this->sides[BLACK] | this->sides[WHITE], 
                (Square) bishop)]];
        uint64_t bishop_index = bishop_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<uint16_t>* move_set = &moves->BISHOP_BLOCKS[bishop].
                checked_moves[bishop_index];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
    }

    // Knight checked moves.
    piece = this->turn ? W_KNIGHT : B_KNIGHT;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int knight = this->piece_list[piece][i];
        if ((1ULL << knight) & *rook_pins || (1ULL << knight) & *bishop_pins) {
            continue;
        }
        Moves * knight_move = &moves->KNIGHT_MOVES[knight];
        uint64_t masked_reach = knight_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<uint16_t>* move_set = &knight_move->move_set[moveSetIndex(
                masked_reach, knight_move)];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
    }

    // Pawn checked moves.
    piece = this->turn ? W_PAWN : B_PAWN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int pawn = this->piece_list[piece][i];
        if ((1ULL << pawn) & *rook_pins || (1ULL << pawn) & *bishop_pins) {
            continue;
        }
        
        Moves* pawn_move = &moves->PAWN_MOVES[this->turn][pawn - 8];
        int advance = this->turn ? 8 : -8;
        uint64_t masked_reach = (pawn_move->reach & checkers_only) | (1ULL << 
                (pawn + advance));
        std::vector<uint16_t>* move_set = &pawn_move->move_set[moveSetIndex(
                masked_reach, pawn_move)];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;

        // Pawn single advance to block check.
        if ((1ULL << (pawn + advance)) & check_rays_only) {
            uint64_t pos = (1ULL << (pawn + advance + advance));
            Moves* pawn_moves = &moves->PAWN_MOVES[this->turn][pawn - 8];
            move_set = &pawn_moves->move_set[
                    moveSetIndex(pos, pawn_moves)];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }

        // Pawn double advance to block check.
        advance = this->turn ? 16 : -16;
        int between = this->turn ? 8 : -8;
        if (((pawn / 8 == 1 && this->turn) || (pawn / 8 == 6 && !this->turn)) && 
                (1ULL << (pawn + advance)) & check_rays_only && !((1ULL << (
                pawn + between)) & (this->sides[BLACK] | this->sides[WHITE]))) {
            Moves* pawn_moves = &moves->DOUBLE_PUSH[pawn - (this->turn ? 8 : 
                    40)];
            move_set = &pawn_moves->move_set[0];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }
    }

    // En-passant moves.
    this->getCheckedEp(rook_pins, bishop_pins, checkers_only,
            moves, pos_moves, moves_index);
}
