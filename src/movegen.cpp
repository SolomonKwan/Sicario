
#include <bitset>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <set>

#include "constants.hpp"
#include "movegen.hpp"

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
int moveSetIndex(uint64_t masked_reach, MovesStruct* move_family) {
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
void Game::computeCastling(MovesStruct* CASTLING_MOVES) {
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
    MovesStruct* king_family = &computed_moves->KING_MOVES[this->piece_list[this->turn][0]];
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
    MovesStruct* rook_moves = &moves->ROOK_MOVES[moves->ROOK_INDEX[square][this->rookIndex(
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
    MovesStruct* bishop_moves = &moves->BISHOP_MOVES[moves->BISHOP_INDEX[square]
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
            MovesStruct* rook_like_moves = this->getRookFamily(computed_moves, 
                    (Square) queen);
            std::vector<uint16_t>* move_set = &rook_like_moves->move_set[
                    moveSetIndex(rook_like_moves->reach ^ this->sides[
                    this->turn], rook_like_moves)];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;

            MovesStruct* bishop_like_moves = this->getBishopFamily(computed_moves, 
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
            MovesStruct* rook_like_moves = this->getRookFamily(computed_moves, 
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
            MovesStruct* bishop_like_moves = this->getBishopFamily(computed_moves, 
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
            MovesStruct* knight_moves = &computed_moves->KNIGHT_MOVES[knight];
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
                MovesStruct* pawn_moves = &computed_moves->PAWN_MOVES[this->turn][
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
                MovesStruct* pawn_moves = &computed_moves->PAWN_MOVES[this->turn][
                        pawn - 8];
                std::vector<uint16_t>* move_set = &pawn_moves->move_set[
                        moveSetIndex(pos, pawn_moves)];
                if (move_set->size() != 0) {
                    pos_moves[(*moves_index)++] = move_set;
                }
            }
        } else {
            MovesStruct* pawn_moves = &computed_moves->PAWN_MOVES[this->turn][pawn - 
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
        MovesStruct* bishop_move = &moves->BISHOP_MOVES[moves->BISHOP_INDEX[queen][
                this->bishopIndex(this->sides[BLACK] | this->sides[WHITE], 
                        (Square) queen)]];
        uint64_t bishop_index = bishop_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<uint16_t>* move_set = &moves->BISHOP_BLOCKS[queen].
                checked_moves[bishop_index];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;

        MovesStruct* rook_move = &moves->ROOK_MOVES[moves->ROOK_INDEX[queen][
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
        MovesStruct* rook_move = &moves->ROOK_MOVES[moves->ROOK_INDEX[rook][
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
        MovesStruct* bishop_move = &moves->BISHOP_MOVES[moves->BISHOP_INDEX[bishop][
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
        MovesStruct * knight_move = &moves->KNIGHT_MOVES[knight];
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
        
        MovesStruct* pawn_move = &moves->PAWN_MOVES[this->turn][pawn - 8];
        int advance = this->turn ? 8 : -8;
        uint64_t masked_reach = (pawn_move->reach & checkers_only) | (1ULL << 
                (pawn + advance));
        std::vector<uint16_t>* move_set = &pawn_move->move_set[moveSetIndex(
                masked_reach, pawn_move)];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;

        // Pawn single advance to block check.
        if ((1ULL << (pawn + advance)) & check_rays_only) {
            uint64_t pos = (1ULL << (pawn + advance + advance));
            MovesStruct* pawn_moves = &moves->PAWN_MOVES[this->turn][pawn - 8];
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
            MovesStruct* pawn_moves = &moves->DOUBLE_PUSH[pawn - (this->turn ? 8 : 
                    40)];
            move_set = &pawn_moves->move_set[0];
            if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
        }
    }

    // En-passant moves.
    this->getCheckedEp(rook_pins, bishop_pins, checkers_only,
            moves, pos_moves, moves_index);
}


/**
 * Sets the reach of the bishop for a particular square and occupancy (according
 * to MSBs) and creates the moves. This only happens if the family of moves has
 * not been set yet.
 * 
 * @param square: The square that the bishop is on.
 * @param move_family: Pointer to struct holding the moves family for bishop.
 */
void Game::setBishopMoves(int square, MovesStruct* move_family) {
    move_family->reach = 0;

    // Set the reach.
    for (int bit : move_family->block_bits) {
        if (bit > square && bit % 8 > square % 8) { // Upper right.
            int index = square + 9;
            while (index <= bit) {
                move_family->reach |= 1ULL << index;
                index += 9;
            }
        } else if (bit > square && bit % 8 < square % 8) { // Upper left.
            int index = square + 7;
            while (index <= bit) {
                move_family->reach |= 1ULL << index;
                index += 7;
            }
        } else if (bit < square && bit % 8 > square % 8) { // Lower right.
            int index = square - 7;
            while (index >= bit) {
                move_family->reach |= 1ULL << index;
                index -= 7;
            }
        } else { // Lower left.
            int index = square - 9;
            while (index >= bit) {
                move_family->reach |= 1ULL << index;
                index -= 9;
            }
        }
    }

    // Set the moves.
    std::vector<int>* blockers = &move_family->block_bits;
    move_family->move_set.resize(std::pow(2, blockers->size()));

    // Iterate over end occupancies.
    for (int i = 0; i < std::pow(2, blockers->size()); i++) {
        uint64_t pos = move_family->reach;
        
        // Create the position with masked ends.
        int index = 0;
        for (auto j = blockers->begin(); j != blockers->end(); j++) {
            pos = (pos & ~(1ULL << *j)) | ((1ULL & i >> index) << *j);
            index++;
        }

        std::vector<uint16_t>* moves_set = &(move_family->
                move_set[moveSetIndex(pos, move_family)]);

        // Set upper right moves.
        index = square + 9;
        while (((pos >> index) & 1ULL) != 0) {
            if (index > 63 || index % 8 <= square % 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index += 9;
        }

        // Set upper left moves.
        index = square + 7;
        while (((pos >> index) & 1ULL) != 0) {
            if (index > 63 || index % 8 >= square % 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index += 7;
        }

        // Set lower right moves.
        index = square - 7;
        while (((pos >> index) & 1ULL) != 0) {
            if (index < 0 || index % 8 <= square % 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index -= 7;
        }

        // Set lower left moves.
        index = square - 9;
        while (((pos >> index) & 1ULL) != 0) {
            if (index < 0 || index % 8 >= square % 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index -= 9;
        }
    }
}

/**
 * Computes the bishop move indices for the corner squares and calls the 
 * function to set the moves and reach.
 * 
 * @param square: The corner square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the 
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of move structs for each bishop move family.
 */
void Game::computeBCornerMoves(int square, int* offset, 
        std::vector<int>* BISHOP_INDEX, MovesStruct* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start = square + (square / 8 <= 3 ? 8 : -8) + 
            (square % 8 <= 3 ? 1 : -1);
    int end = square + (square / 8 <= 3 ? 48 : -48) + 
            (square % 8 <= 3 ? 6 : -6);

    BISHOP_INDEX[square].resize(64);
    std::set<int> indices;
    for (int occ = 0; occ < 64; occ++) {
        // Build position.
        int index, change;
        uint64_t pos = 0;
            
        // Retrieve and set bits.
        if (square == A1) change = 9;
        if (square == A8) change = -7;
        if (square == H1) change = 7;
        if (square == H8) change = -9;
        index = 5;
        for (int k = start; k != end; k += change) {
            pos |= ((occ >> index) & 1ULL) << k;
            index--;
        }
        pos |= ((occ >> index) & 1ULL) << end;

        // Index creation.
        int bishopIndex = (
            ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> 
                    bishopShifts[square]
        );

        int move_index = MSB(occ) + *offset;
        BISHOP_INDEX[square][bishopIndex] = move_index;

        indices.insert(move_index);
        if (BISHOP_MOVES[move_index].reach == UNSET) {
            BISHOP_MOVES[move_index].block_bits.push_back(
                square + change * (7 - MSB(occ))
            );
            std::sort(BISHOP_MOVES[move_index].block_bits.begin(),
                    BISHOP_MOVES[move_index].block_bits.end());
            Game::setBishopMoves(square, &(BISHOP_MOVES[move_index]));
        }
    }

    *offset += 7;
}

/**
 * Computes the bishop move indices for the left and right squares and calls the 
 * function to set the moves and reach.
 * 
 * @param square: The corner square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the 
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of move structs for each bishop move family.
 */
void Game::computeBLRSideMoves(int square, int* offset, 
        std::vector<int>* BISHOP_INDEX, MovesStruct* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start1 = square % 8 == 0 ? square + 9 : square - 9;
    int end1 = square + (square % 8 == 0 ? 9 : -9) * (square % 8 == 0 ? 
            6 - square / 8 : square / 8 - 1);
    int start2 = square % 8 == 0 ? square - 7 : square + 7;
    int end2 = square + (square % 8 == 0 ? -7 : 7) * (square % 8 == 0 ? 
            square / 8 - 1 : 6 - square / 8);

    BISHOP_INDEX[square].resize(32);

    int bits1 = square % 8 == 0 ? (end1 % 8 - start1 % 8 + 1) : 
            (start1 % 8 - end1 % 8 + 1);
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = square % 8 == 0 ? (end2 % 8 - start2 % 8 + 1) : 
            (start2 % 8 - end2 % 8 + 1);
    int size2 = std::pow(2, bits2); // Left diagonal.

    std::set<int> indices;
    int offset_add;
    for (int occ1 = 0; occ1 < size1; occ1++) {
        for (int occ2 = 0; occ2 < size2; occ2++) {
            // Build position.
            int index, change;
            uint64_t pos = 0;
            
            // Retrieve and set right diagonal bits.
            change = square % 8 == 0 ? 9 : -9;
            index = bits1 - 1;
            if (size1 != 1) {
                for (int i = start1; i != end1; i += change) {
                    pos |= ((occ1 >> index) & 1ULL) << i;
                    index--;
                }
                pos |= ((occ1 >> index) & 1ULL) << end1;
            }

            // Retrieve and set left diagonal bits.
            change = square % 8 == 0 ? -7 : 7;
            index = bits2 - 1;
            if (size2 != 1) {
                for (int i = start2; i != end2; i += change) {
                    pos |= ((occ2 >> index) & 1ULL) << i;
                    index--;
                }
                pos |= ((occ2 >> index) & 1ULL) << end2;
            }

            // Index creation.
            int bishopIndex = (
                ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> 
                        bishopShifts[square]
            );

            int move_index;
            if (square == A2 || square == A7 || square == H2 || square == H7) {
                move_index = MSB(size1 == 32 ? occ1 : occ2) + *offset;
                offset_add = 6;
            } else if (square == A3 || square == A6 || square == H3 || 
                    square == H6) {
                move_index = MSB(size1 == 16 ? occ1 : occ2) + 
                        5 * MSB(size1 == 2 ? occ1 : occ2) + *offset;
                offset_add = 10;
            } else {
                move_index = MSB(size1 == 8 ? occ1 : occ2) + 
                        4 * MSB(size1 == 4 ? occ1 : occ2) + *offset;
                offset_add = 12;
            }

            BISHOP_INDEX[square][bishopIndex] = move_index;
            indices.insert(move_index);
            if (BISHOP_MOVES[move_index].reach == UNSET) {
                BISHOP_MOVES[move_index].block_bits.push_back( // Right diag.
                    square + (square % 8 == 0 ? 9 : -9) * 
                            (square % 8 == 0 ? 7 - square / 8 - MSB(occ1) : 
                            square / 8 - MSB(occ1))
                );
                BISHOP_MOVES[move_index].block_bits.push_back( // Left diag.
                    square + (square % 8 == 0 ? -7 : 7) * 
                            (square % 8 == 0 ? square / 8 - MSB(occ2) : 
                            7 - square / 8 - MSB(occ2))
                );

                std::sort(BISHOP_MOVES[move_index].block_bits.begin(),
                        BISHOP_MOVES[move_index].block_bits.end());
                Game::setBishopMoves(square, &(BISHOP_MOVES[move_index]));
            }
        }
    }

    *offset += offset_add;
}

/**
 * Computes the bishop move indices for the upper and lower squares and calls 
 * the function to set the moves and reach.
 * 
 * @param square: The corner square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the 
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of move structs for each bishop move family.
 */
void Game::computeBULSideMoves(int square, int* offset, 
        std::vector<int>* BISHOP_INDEX, MovesStruct* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start1 = square / 8 == 0 ? square + 9 : square - 9;
    int end1 = square + (square / 8 == 0 ? 9 : -9) * (square / 8 == 0 ? 
            6 - square % 8 : square % 8 - 1); // Right diagonal.
    int start2 = square / 8 == 0 ? square + 7 : square - 7;
    int end2 = square + (square / 8 == 0 ? 7 : -7) * (square / 8 == 0 ? 
            square % 8 - 1 : 6 - square % 8); // Left diagonal.

    BISHOP_INDEX[square].resize(32);
    int bits1 = square / 8 == 0 ? (end1 / 8 - start1 / 8 + 1) : 
            (start1 / 8 - end1 / 8 + 1);
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = square / 8 == 0 ? (end2 / 8 - start2 / 8 + 1) : 
            (start2 / 8 - end2 / 8 + 1);
    int size2 = std::pow(2, bits2); // Left diagonal.

    std::set<int> indices;
    int offset_add;
    for (int occ1 = 0; occ1 < size1; occ1++) {
        for (int occ2 = 0; occ2 < size2; occ2++) {
            // Build position.
            int index, change;
            uint64_t pos = 0;
            
            // Retrieve and set right diagonal bits.
            change = square / 8 == 0 ? 9 : -9;
            index = bits1 - 1;
            if (size1 != 1) {
                for (int i = start1; i != end1; i += change) {
                    pos |= ((occ1 >> index) & 1ULL) << i;
                    index--;
                }
                pos |= ((occ1 >> index) & 1ULL) << end1;
            }

            // Retrieve and set left diagonal bits.
            change = square / 8 == 0 ? 7 : -7;
            index = bits2 - 1;
            if (size2 != 1) {
                for (int i = start2; i != end2; i += change) {
                    pos |= ((occ2 >> index) & 1ULL) << i;
                    index--;
                }
                pos |= ((occ2 >> index) & 1ULL) << end2;
            }

            // Index creation.
            int bishopIndex = (
                ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> 
                        bishopShifts[square]
            );

            int move_index;
            if (square == B1 || square == B8 || square == G1 || square == G8) {
                move_index = MSB(size1 == 32 ? occ1 : occ2) + *offset;
                offset_add = 6;
            } else if (square == C1 || square == C8 || square == F1 || 
                    square == F8) {
                move_index = MSB(size1 == 16 ? occ1 : occ2) + 
                        5 * MSB(size1 == 2 ? occ1 : occ2) + *offset;
                offset_add = 10;
            } else {
                move_index = MSB(size1 == 8 ? occ1 : occ2) + 
                        4 * MSB(size1 == 4 ? occ1 : occ2) + *offset;
                offset_add = 12;
            }
            BISHOP_INDEX[square][bishopIndex] = move_index;

            indices.insert(move_index);
            if (BISHOP_MOVES[move_index].reach == UNSET) {
                BISHOP_MOVES[move_index].block_bits.push_back( // Right diag.
                    square + (square / 8 == 0 ? 9 : -9) * 
                            (square / 8 == 0 ? 7 - square % 8 - MSB(occ1) : 
                            square % 8 - MSB(occ1))
                );
                BISHOP_MOVES[move_index].block_bits.push_back( // Left diag.
                    square + (square / 8 == 0 ? 7 : -7) * 
                            (square / 8 == 0 ? square % 8 - MSB(occ2) : 
                            7 - square % 8 - MSB(occ2))
                );

                std::sort(BISHOP_MOVES[move_index].block_bits.begin(),
                        BISHOP_MOVES[move_index].block_bits.end());
                Game::setBishopMoves(square, &(BISHOP_MOVES[move_index]));
            }
        }
    }
    *offset += offset_add;
}

/**
 * Computes the bishop move indices for the centre squares and calls the 
 * function to set the moves and reach.
 * 
 * @param square: The centre square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the 
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of move structs for each bishop move family.
 */
void Game::computeBCentreMoves(int sq, int* offset, std::vector<int>* BISHOP_INDEX, 
        MovesStruct* BISHOP_MOVES) {
    int start1 = sq + 9; // Upper right.
    int start2 = sq - 7; // Lower right.
    int start3 = sq - 9; // Lower left.
    int start4 = sq + 7; // Upper left.

    int end1, end2, end3, end4;

    int ind = start1;
    while (ind / 8 != 0 && ind / 8 != 7 && ind % 8 != 0 && ind % 8 != 7) {
        ind += 9;
    }
    ind -= 9;
    end1 = ind;

    ind = start2;
    while (ind / 8 != 0 && ind / 8 != 7 && ind % 8 != 0 && ind % 8 != 7) {
        ind -= 7;
    }
    ind += 7;
    end2 = ind;

    ind = start3;
    while (ind / 8 != 0 && ind / 8 != 7 && ind % 8 != 0 && ind % 8 != 7) {
        ind -= 9;
    }
    ind += 9;
    end3 = ind;

    ind = start4;
    while (ind / 8 != 0 && ind / 8 != 7 && ind % 8 != 0 && ind % 8 != 7) {
        ind += 7;
    }
    ind -= 7;
    end4 = ind;


    if (sq / 8 == 1 || sq / 8 == 6 || sq % 8 == 1 || sq % 8 == 6) {
        BISHOP_INDEX[sq].resize(32);
    } else if (sq / 8 == 2 || sq / 8 == 5 || sq % 8 == 2 || sq % 8 == 5) {
        BISHOP_INDEX[sq].resize(128);
    } else {
        BISHOP_INDEX[sq].resize(512);
    }

    std::set<int> indices;
    
    int bits1 = end1 % 8 - start1 % 8 + 1;
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = end2 % 8 - start2 % 8 + 1;
    int size2 = std::pow(2, bits2); // Left diagonal.
    int bits3 = start3 % 8 - end3 % 8 + 1;
    int size3 = std::pow(2, bits3); // Right diagonal.
    int bits4 = start4 % 8 - end4 % 8 + 1;
    int size4 = std::pow(2, bits4); // Left diagonal.

    int offset_add;

    for (int occ1 = 0; occ1 < size1; occ1++) {
        for (int occ2 = 0; occ2 < size2; occ2++) {
            for (int occ3 = 0; occ3 < size3; occ3++) {
                for (int occ4 = 0; occ4 < size4; occ4++) {
                     // Build position.
                    int ind;
                    uint64_t pos = 0;
                    int msb1 = -1, msb2 = -1, msb3 = -1, msb4 = -1;

                    // Retrieve and set upper right bits.
                    ind = bits1 - 1;
                    if (size1 != 1) {
                        for (int i = start1; i <= end1; i += 9) {
                            pos |= ((occ1 >> ind) & 1ULL) << i;
                            if (((occ1 >> ind) & 1ULL) == 1ULL && msb1 == -1) {
                                msb1 = i;
                            }
                            ind--;
                        }
                    }
                    if (msb1 == -1) msb1 = end1 + 9;

                    // Retrieve and set lower right bits.
                    ind = bits2 - 1;
                    if (size2 != 1) {
                        for (int i = start2; i >= end2; i -= 7) {
                            pos |= ((occ2 >> ind) & 1ULL) << i;
                            if (((occ2 >> ind) & 1ULL) == 1ULL && msb2 == -1) {
                                msb2 = i;
                            }
                            ind--;
                        }
                    }
                    if (msb2 == -1) msb2 = end2 - 7;

                    // Retrieve and set lower left bits.
                    ind = bits3 - 1;
                    if (size3 != 1) {
                        for (int i = start3; i >= end3; i -= 9) {
                            pos |= ((occ3 >> ind) & 1ULL) << i;
                            if (((occ3 >> ind) & 1ULL) == 1ULL && msb3 == -1) {
                                msb3 = i;
                            }
                            ind--;
                        }
                    }
                    if (msb3 == -1) msb3 = end3 - 9;

                    // Retrieve and set upper left bits.
                    ind = bits4 - 1;
                    if (size4 != 1) {
                        for (int i = start4; i <= end4; i += 7) {
                            pos |= ((occ4 >> ind) & 1ULL) << i;
                            if (((occ4 >> ind) & 1ULL) == 1ULL && msb4 == -1) {
                                msb4 = i;
                            }
                            ind--;
                        }
                    }
                    if (msb4 == -1) msb4 = end4 + 7;

                    // Index creation.
                    int bishopIndex = (
                        ((pos & bishopMasks[sq]) * bishopMagicNums[sq]) >> 
                                bishopShifts[sq]
                    );

                    int move_index;
                    if (sq == B2 || sq == B7 || sq == G2 || 
                            sq == G7) {
                        move_index = MSB(
                            size1 == 32 ? occ1 : (
                            size2 == 32 ? occ2 : (
                            size3 == 32 ? occ3 : occ4))
                        ) + *offset;
                        offset_add = 6;
                    } else if (sq == C2 || sq == F2 || sq == C7 || 
                            sq == F7 || sq == B3 || sq == G3 || 
                            sq == B6 || sq == G6) {
                        move_index = MSB(
                            size1 == 16 ? occ1 : (
                            size2 == 16 ? occ2 : (
                            size3 ==  16 ? occ3 : occ4))) + 
                        5 * MSB(size1 == 2 ? occ1 : (
                            size2 == 2 ? occ2 : (
                            size3 ==  2 ? occ3 : occ4))) + *offset;
                        offset_add = 10;
                    } else if (sq == D2 || sq == E2 || sq == B4 || 
                            sq == G4 || sq == B5 || sq == G5 || 
                            sq == D7 || sq == E7) {
                        move_index = MSB(
                            size1 == 8 ? occ1 : (
                            size2 == 8 ? occ2 : (
                            size3 ==  8 ? occ3 : occ4))) + 
                        4 * MSB(size1 == 4 ? occ1 : (
                            size2 == 4 ? occ2 : (
                            size3 ==  4 ? occ3 : occ4))) + *offset;
                        offset_add = 12;
                    } else if (sq == C3 || sq == F3 || sq == C6 || 
                            sq == F6) {
                        if (sq == C3) {
                            move_index = MSB(occ1) + 5 * MSB(occ2) + 10 * 
                                    MSB(occ3) + 20 * MSB(occ4) + *offset;
                        } else if (sq == F3) {
                            move_index = MSB(occ4) + 5 * MSB(occ2) + 10 * 
                                    MSB(occ3) + 20 * MSB(occ1) + *offset;
                        } else if (sq == C6) {
                            move_index = MSB(occ2) + 5 * MSB(occ1) + 10 * 
                                    MSB(occ3) + 20 * MSB(occ4) + *offset;
                        } else {
                            move_index = MSB(occ3) + 5 * MSB(occ2) + 10 * 
                                    MSB(occ1) + 20 * MSB(occ4) + *offset;
                        }
                        offset_add = 40;
                    } else if (sq == D3 || sq == E3 || sq == C4 || 
                            sq == F4 || sq == C5 || sq == F5 || 
                            sq == D6 || sq == E6) {
                        if (sq % 8 == 2) {
                            move_index = MSB(size1 == 8 ? occ1 : occ2) + 
                                4 * MSB(size1 == 4 ? occ1 : occ2) +
                                12 * MSB(occ3) +
                                24 * MSB(occ4) + *offset; 
                        } else if (sq % 8 == 5) {
                            move_index = MSB(size3 == 8 ? occ3 : occ4) + 
                                4 * MSB(size3 == 4 ? occ3 : occ4) +
                                12 * MSB(occ1) +
                                24 * MSB(occ2) + *offset;
                        } else if (sq / 8 == 2) {
                            move_index = MSB(size1 == 8 ? occ1 : occ4) + 
                                4 * MSB(size1 == 4 ? occ1 : occ4) +
                                12 * MSB(occ2) +
                                24 * MSB(occ3) + *offset;
                        } else {
                            move_index = MSB(size2 == 8 ? occ2 : occ3) + 
                                4 * MSB(size2 == 4 ? occ2 : occ3) +
                                12 * MSB(occ1) +
                                24 * MSB(occ4) + *offset;
                        }
                        offset_add = 48;
                    } else {
                        if (sq == D4) {
                            move_index = MSB(occ1) + 4 * MSB(occ2) + 12 * 
                                    MSB(occ3) + 36 * MSB(occ4) + *offset;
                        } else if (sq == D5) {
                            move_index = MSB(occ2) + 4 * MSB(occ1) + 12 * 
                                    MSB(occ3) + 36 * MSB(occ4) + *offset;
                        } else if (sq == E4) {
                            move_index = MSB(occ4) + 4 * MSB(occ2) + 12 * 
                                    MSB(occ3) + 36 * MSB(occ1) + *offset;
                        } else {
                            move_index = MSB(occ3) + 4 * MSB(occ2) + 12 * 
                                    MSB(occ1) + 36 * MSB(occ4) + *offset;
                        }
                        offset_add = 108;
                    }
                    indices.insert(move_index);
                    BISHOP_INDEX[sq][bishopIndex] = move_index;

                    if (BISHOP_MOVES[move_index].reach == UNSET) {
                        BISHOP_MOVES[move_index].block_bits.push_back(msb1);                        
                        BISHOP_MOVES[move_index].block_bits.push_back(msb2);
                        BISHOP_MOVES[move_index].block_bits.push_back(msb3);
                        BISHOP_MOVES[move_index].block_bits.push_back(msb4);

                        std::sort(BISHOP_MOVES[move_index].block_bits.begin(),
                                BISHOP_MOVES[move_index].block_bits.end());
                        Game::setBishopMoves(sq, &(BISHOP_MOVES[move_index]));
                    }
                }
            }
        }
    }

    *offset += offset_add;
}

/**
 * Returns the index from BISHOP_INDEX into BISHOP_MOVES based on the square and 
 * occupancy.
 * 
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 */
const int Game::bishopIndex(const uint64_t pos, Square square) {
    return ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> 
            bishopShifts[square];
}

/**
 * Gets a bitboard of the rays between the bishop (queen) checkers and the king
 * being checked.
 * 
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t Game::getBishopCheckRays(Computed* moves, Square square, uint64_t* checkers_only) {
    uint64_t result = 0;
    bool turn = this->turn;
    Square king_sq = this->piece_list[turn][0];
    uint64_t king_rays = this->getBishopFamily(moves, king_sq)->reach & 
            ~this->sides[turn];
    *checkers_only |= (this->queens | this->bishops) & this->sides[!turn] & 
            king_rays;
    PieceType queen = turn ? B_QUEEN : W_QUEEN;
    PieceType bishop = turn ? B_BISHOP : W_BISHOP;

    for (int i = 0; i < this->piece_index[queen]; i++) {
        Square piece = this->piece_list[queen][i];
        if (std::abs(piece % 8 - king_sq % 8) == std::abs(piece / 8 - king_sq / 
                8)) {
            result |= (this->getBishopFamily(moves, (Square) piece)->reach & 
                    ~this->sides[!turn] & king_rays);
        }
    }

    for (int i = 0; i < this->piece_index[bishop]; i++) {
        Square piece = this->piece_list[bishop][i];
        if (std::abs(piece % 8 - king_sq % 8) == std::abs(piece / 8 - king_sq / 
                8)) {
            result |= (this->getBishopFamily(moves, (Square) piece)->reach & 
                    ~this->sides[!turn] & king_rays);
        }
    }

    return result;
}

/**
 * Iterates through the squares and call functions to compute bishop moves and
 * calculate indices.
 * 
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of bishop move sets to store precomputed moves.
 */
void Game::computeBishopMoves(std::vector<int>* BISHOP_INDEX, MovesStruct* BISHOP_MOVES) {
    // Set initial reach to UNSET.
    for (int i = 0; i < 1428; i++) {
        BISHOP_MOVES[i].reach = UNSET;
    }

    // Set the moves and reach.
    int offset = 0;
    for (int square = A1; square <= H8; square++) {
        if (square == A1 || square == A8 || square == H1 || square == H8) {
            Game::computeBCornerMoves(square, &offset, BISHOP_INDEX, BISHOP_MOVES);
        } else if (square % 8 == 0 || square % 8 == 7) {
            Game::computeBLRSideMoves(square, &offset, BISHOP_INDEX, BISHOP_MOVES);
        } else if (square / 8 == 0 || square / 8 == 7) {
            Game::computeBULSideMoves(square, &offset, BISHOP_INDEX, BISHOP_MOVES);
        } else {
            Game::computeBCentreMoves(square, &offset, BISHOP_INDEX, BISHOP_MOVES);
        }
    }
}

/**
 * Computes the blocking moves (and captures in case of check) for bishops on 
 * each square.
 * 
 * @param BISHOP_BLOCKS: A vector of move structs.
 */
void Game::computeBishopBlocks(MovesStruct* BISHOP_BLOCKS) {
    std::tuple<int, int> pairs[4] = {
        std::make_tuple(9, -7), std::make_tuple(-7, -9), std::make_tuple(-9, 7),
        std::make_tuple(7, 9)
    };
    int directions[4] = {9, -7, -9, 7};

    for (int square = A1; square <= H8; square++) {
        
        // Single ray traversals.
        for (int direction : directions) {
            if ((direction == 9 && (square % 8 == 7 || square / 8 == 7)) || 
                    (direction == -7 && (square % 8 == 7 || square / 8 == 0)) ||
                    (direction == -9 && (square % 8 == 0 || square / 8 == 0)) ||
                    (direction == 7 && (square % 8 == 0 || square / 8 == 7))) {
                continue;
            }

            int start = square + direction;
            int end_pt = start;
            while (end_pt / 8 != 0 && end_pt / 8 != 7 && end_pt % 8 != 0 && 
                    end_pt % 8 != 7) {
                end_pt += direction;
            }
            end_pt += direction;

            std::unordered_map<uint64_t, std::vector<uint16_t>>* moves = 
                    &BISHOP_BLOCKS[square].checked_moves;
            while (start != end_pt) {
                uint16_t move = square | start << 6 | NORMAL | pKNIGHT;
                std::vector<uint16_t> moveset = {move};
                moves->insert(std::make_pair(1ULL << start, moveset));
                start += direction;
            }
        }

        // Pair ray traversal.
        for (std::tuple<int, int> tuple : pairs) {
            int ray1 = std::get<0>(tuple);
            int ray2 = std::get<1>(tuple);

            if ((square / 8 == 0 && (ray1 < 0 || ray2 < 0)) || 
                    (square / 8 == 7 && (ray1 > 0 || ray2 > 0)) || 
                    (square % 8 == 0 && (ray1 == -9 || ray1 == 7 || 
                    ray2 == -9 || ray2 == 7)) || (square % 8 == 7 && 
                    (ray1 == 9 || ray1 == -7 || ray2 == 9 || ray2 == -7))) {
                continue;
            }

            int start1 = square + ray1, start2 = square + ray2; 
            int end1 = start1, end2 = start2;
            
            while (end1 / 8 != 0 && end1 / 8 != 7 && end1 % 8 != 0 && 
                    end1 % 8 != 7) {
                end1 += ray1;
            }
            end1 += ray1;

            while (end2 / 8 != 0 && end2 / 8 != 7 && end2 % 8 != 0 && 
                    end2 % 8 != 7) {
                end2 += ray2;
            }
            end2 += ray2;
            
            std::unordered_map<uint64_t, std::vector<uint16_t>>* moves = 
                    &BISHOP_BLOCKS[square].checked_moves;

            while (start1 != end1) {
                start2 = square + ray2;
                while (start2 != end2) {
                    uint16_t move1 = square | start1 << 6 | NORMAL | pKNIGHT;
                    uint16_t move2 = square | start2 << 6 | NORMAL | pKNIGHT;
                    std::vector<uint16_t> moveset = {move1, move2};
                    moves->insert(std::make_pair(1ULL << start1 | 1ULL << 
                            start2, moveset));
                    start2 += ray2;
                }
                start1 += ray1;
            };
        };
    }
}

/**
 * Finds and returns a pointer to a bishop move family.
 * 
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the bishop is on.
 */
MovesStruct* Game::getBishopFamily(Computed* moves, Square square) {
    return &moves->BISHOP_MOVES[moves->BISHOP_INDEX[square][
            this->bishopIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
}

/**
 * Compute the king moves.
 * @param KING_MOVES: The array of king moves to be computed.
 */
void Game::computeKingMoves(MovesStruct* KING_MOVES) {
    // Compute the moves and reach.
    for (int square = A1; square <= H8; square++) {
        KING_MOVES[square].reach = 0;
        
        if (square / 8 != 7) {
            KING_MOVES[square].reach |= 1ULL << (square + 8);
            KING_MOVES[square].block_bits.push_back(square + 8);
        }
        if (square / 8 != 7 && square % 8 != 7) {
            KING_MOVES[square].reach |= 1ULL << (square + 9);
            KING_MOVES[square].block_bits.push_back(square + 9);
        }
        if (square % 8 != 7) {
            KING_MOVES[square].reach |= 1ULL << (square + 1);
            KING_MOVES[square].block_bits.push_back(square + 1);
        }
        if (square / 8 != 0 && square % 8 != 7) {
            KING_MOVES[square].reach |= 1ULL << (square - 7);
            KING_MOVES[square].block_bits.push_back(square - 7);
        }
        if (square / 8 != 0) {
            KING_MOVES[square].reach |= 1ULL << (square - 8);
            KING_MOVES[square].block_bits.push_back(square - 8);
        }
        if (square / 8 != 0 && square % 8 != 0) {
            KING_MOVES[square].reach |= 1ULL << (square - 9);
            KING_MOVES[square].block_bits.push_back(square - 9);
        }
        if (square % 8 != 0) {
            KING_MOVES[square].reach |= 1ULL << (square - 1);
            KING_MOVES[square].block_bits.push_back(square - 1);
        }
        if (square / 8 != 7 && square % 8 != 0) {
            KING_MOVES[square].reach |= 1ULL << (square + 7);
            KING_MOVES[square].block_bits.push_back(square + 7);
        }

        // Sort the block bits.
        std::sort(KING_MOVES[square].block_bits.begin(), 
                KING_MOVES[square].block_bits.end());

        // Set the moves.
        std::vector<int>* blockers = &(KING_MOVES[square].block_bits);
        KING_MOVES[square].move_set.resize(std::pow(2, blockers->size()));

        // Iterate over end occupancies.
        for (int i = 0; i < std::pow(2, blockers->size()); i++) {
            uint64_t pos = KING_MOVES[square].reach;
        
            // Create the position with masked ends.
            int index = 0;
            for (auto j = blockers->begin(); j != blockers->end(); j++) {
                pos = (pos & ~(1ULL << *j)) | ((1ULL & (i >> index)) << *j);
                index++;
            }

            std::vector<uint16_t>* moves_set = &(KING_MOVES[square].
                move_set[moveSetIndex(pos, &KING_MOVES[square])]);
            
            // Set the moves.
            for (uint64_t shift = 0; shift < 64; shift++) {
                if (((pos >> shift) & 1ULL) == 1) {
                    moves_set->push_back(square | shift << 6 | NORMAL | pKNIGHT);
                }
            }
        }
    }
}

/**
 * Compute the knight moves.
 * @param KNIGHT_MOVES: Array of knight moves to be computed.
 */
void Game::computeKnightMoves(MovesStruct* KNIGHT_MOVES) {
    // Compute the moves and reach.
    for (int square = A1; square <= H8; square++) {
        // Compute the reach.
        KNIGHT_MOVES[square].reach = 0;
        
        if (square / 8 < 6 && square % 8 < 7) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square + 17); // NNE.
            KNIGHT_MOVES[square].block_bits.push_back(square + 17);
        }
        if (square / 8 < 7 && square % 8 < 6) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square + 10); // ENE.
            KNIGHT_MOVES[square].block_bits.push_back(square + 10);
        }
        if (square / 8 > 0 && square % 8 < 6) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square - 6); // ESE.
            KNIGHT_MOVES[square].block_bits.push_back(square - 6);
        }
        if (square / 8 > 1 && square % 8 < 7) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square - 15); // SES.
            KNIGHT_MOVES[square].block_bits.push_back(square - 15);
        }
        if (square / 8 > 1 && square % 8 > 0) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square - 17); // SWS.
            KNIGHT_MOVES[square].block_bits.push_back(square - 17);
        }
        if (square / 8 > 0 && square % 8 > 1) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square - 10); // ESE.
            KNIGHT_MOVES[square].block_bits.push_back(square - 10);
        }
        if (square / 8 < 7 && square % 8 > 1) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square + 6); // WNW.
            KNIGHT_MOVES[square].block_bits.push_back(square + 6);
        }
        if (square / 8 < 6 && square % 8 > 0) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square + 15); // NWN.
            KNIGHT_MOVES[square].block_bits.push_back(square + 15);
        }

        // Sort the block bits.
        std::sort(KNIGHT_MOVES[square].block_bits.begin(), 
                KNIGHT_MOVES[square].block_bits.end());

        // Set the moves.
        std::vector<int>* blockers = &(KNIGHT_MOVES[square].block_bits);
        KNIGHT_MOVES[square].move_set.resize(std::pow(2, blockers->size()));

        // Iterate over end occupancies.
        for (int i = 0; i < std::pow(2, blockers->size()); i++) {
            uint64_t pos = KNIGHT_MOVES[square].reach;
        
            // Create the position with masked ends.
            int index = 0;
            for (auto j = blockers->begin(); j != blockers->end(); j++) {
                pos = (pos & ~(1ULL << *j)) | ((1ULL & i >> index) << *j);
                index++;
            }

            std::vector<uint16_t>* moves_set = &(KNIGHT_MOVES[square].
                move_set[moveSetIndex(pos, &KNIGHT_MOVES[square])]);
            
            // Set the moves.
            for (uint64_t shift = 0; shift < 64; shift++) {
                if (((pos >> shift) & 1ULL) == 1) {
                    moves_set->push_back(square | shift << 6 | NORMAL | pKNIGHT);
                }
            }
        }
    }
}

/**
 * Gets a bitboard of knight checkers.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the king in check.
 * @param checkers_only: A pointer to a bit board of checkers only.
 * @return: Bitboard of knight checkers.
 */
uint64_t Game::getKnightCheckers(Computed* moves, Square square, uint64_t* checkers_only) {
    uint64_t result = 0;
    *checkers_only |= moves->KNIGHT_MOVES[square].reach & this->sides[1 - this->turn] & this->knights;
    return result;
}

/**
 * Computes pawn moves.
 * @param PAWN_MOVES: The array of pawn moves structs.
 * @param EN_PASSANT_MOVES: Array of en-passant moves structs. 32 is the offset
 *  to index a moves struct. move_set index of 1 is for left ep, 0 is for right
 *  ep.
 * @param DOUBLE_PUSH: An array of moves structs for double pawn push blocks.
 */
void Game::computePawnMoves(MovesStruct PAWN_MOVES[][48], MovesStruct EN_PASSANT_MOVES[16],
        MovesStruct DOUBLE_PUSH[16]) {
    for (int player = BLACK; player <= WHITE; player++) {
        int left = player == BLACK ? -9 : 7;
        int right = player == BLACK ? -7 : 9;
        int forward_push = player == BLACK ?  -8 : 8;
        int double_push = player == BLACK ? - 16 : 16;
        
        for (int square = 8; square <= 55; square++) {
            uint64_t pos = 0; // The position for move generation.
            PAWN_MOVES[player][square - 8].reach = 0;

            // Create the attack reach.
            if (square % 8 != 0) {
                PAWN_MOVES[player][square - 8].reach |= 1ULL << (square + 
                        left);
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + 
                        left);
                pos |= 1ULL << (square + left);
            }
            if (square % 8 != 7) {
                PAWN_MOVES[player][square - 8].reach |= 1ULL << (square + 
                        right);
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + 
                        right);
                pos |= 1ULL << (square + right);
            }

            // Add one push block bit.
            PAWN_MOVES[player][square - 8].block_bits.push_back(square + 
                    forward_push);
            pos |= 1ULL << (square + forward_push);

            // Add two push block bit.
            if ((player == WHITE && square / 8 == 1) || (player == BLACK && 
                    square / 8 == 6)) {
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + 
                        double_push);
                pos |= 1ULL << (square + double_push);
            }
            
            // Sort the block bits.
            std::sort(PAWN_MOVES[player][square - 8].block_bits.begin(), 
                    PAWN_MOVES[player][square - 8].block_bits.end());

            // Set the moves.
            std::vector<int>* blockers = &(PAWN_MOVES[player][square - 8].
                    block_bits);
            PAWN_MOVES[player][square - 8].move_set.resize(std::pow(2, 
                    blockers->size()));
            
            // En-passant captures
            if (player == WHITE && square / 8 == 4) { // En-passant.
                EN_PASSANT_MOVES[square - 24].move_set.resize(2);
                std::vector<uint16_t>* r_ep_set = &(EN_PASSANT_MOVES[square - 
                        24].move_set[0]);
                std::vector<uint16_t>* l_ep_set = &(EN_PASSANT_MOVES[square - 
                        24].move_set[1]);
                r_ep_set->push_back(square | (square + right) << 6 | 
                        EN_PASSANT | pKNIGHT);
                l_ep_set->push_back(square | (square + left) << 6 | EN_PASSANT |
                        pKNIGHT);
            } else if (player == BLACK && square / 8 == 3) { // En-passant.
                EN_PASSANT_MOVES[square - 24].move_set.resize(2);
                std::vector<uint16_t>* r_ep_set = &(EN_PASSANT_MOVES[square - 
                        24].move_set[0]);
                std::vector<uint16_t>* l_ep_set = &(EN_PASSANT_MOVES[square - 
                        24].move_set[1]);
                r_ep_set->push_back(square | (square + right) << 6 | 
                        EN_PASSANT | pKNIGHT);
                l_ep_set->push_back(square | (square + left) << 6 | EN_PASSANT |
                        pKNIGHT);
            }

            // Double push blocks
            if (player == WHITE && square / 8 == 1) {
                DOUBLE_PUSH[square - 8].move_set.resize(1);
                std::vector<uint16_t>* move = &(DOUBLE_PUSH[square - 8].
                        move_set[0]);
                move->push_back(square | (square + 16) << 6 | NORMAL | pKNIGHT);
            } else if (player == BLACK && square / 8 == 6) {
                DOUBLE_PUSH[square - 40].move_set.resize(1);
                std::vector<uint16_t>* move = &(DOUBLE_PUSH[square - 40].
                        move_set[0]);
                move->push_back(square | (square - 16) << 6 | NORMAL | pKNIGHT);
            }

            // Iterate over occupancies.
            for (int i = 0; i < std::pow(2, blockers->size()); i++) {            
                // Create the position with masked ends.
                int index = 0;
                for (auto j = blockers->begin(); j != blockers->end(); j++) {
                    pos = (pos & ~(1ULL << *j)) | ((1ULL & (i >> index)) << *j);
                    index++;
                }

                std::vector<uint16_t>* moves_set = &(PAWN_MOVES[player][square -
                        8].move_set[moveSetIndex(pos, &PAWN_MOVES[player][
                        square - 8])]);

                if (player == WHITE) {
                    if (square % 8 != 7 && square / 8 == 6) { // Right promo.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pROOK);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 7) { // Normal right capture.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 
                                    6 | NORMAL | pKNIGHT);
                        }
                    }

                    if (square % 8 != 0 && square / 8 == 6) { // Left promo
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pROOK);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 0) { // Normal left capture.
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 |
                                    NORMAL | pKNIGHT);
                        }
                    }

                    // Move up promo.
                    if (square / 8 == 6 && !bitAt(pos, square + forward_push)) {
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pKNIGHT);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pBISHOP);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pROOK);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pQUEEN);
                    } else if (!bitAt(pos, square + forward_push)) {
                        // Normal move up.
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | NORMAL | pKNIGHT);
                    }

                    if (square / 8 == 1) { // Starting square.
                        if (!bitAt(pos, square + forward_push) && !bitAt(pos,
                                square + double_push)) { // Move up.
                            moves_set->push_back(square | (square + 
                                    double_push) << 6 | NORMAL | pKNIGHT);
                        }
                    }
                } else {
                    if (square % 8 != 7 && square / 8 == 1) { // Right promo.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pROOK);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 7) { // Normal right capture.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 
                                    6 | NORMAL | pKNIGHT);
                        }
                    }

                    if (square % 8 != 0 && square / 8 == 1) { // Left promo
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pROOK);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 0) { // Normal left capture.
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 |
                                    NORMAL | pKNIGHT);
                        }
                    }

                    if (square / 8 == 1 && !bitAt(pos, square + forward_push)) {
                        // Move up promo.
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pKNIGHT);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pBISHOP);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pROOK);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pQUEEN);
                    } else if (!bitAt(pos, square + forward_push)) {
                        // Normal move up.
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | NORMAL | pKNIGHT);
                    }

                    if (square / 8 == 6) { // Starting square.
                        if (!bitAt(pos, square + forward_push) && !bitAt(pos, 
                                square + double_push)) { // Move up.
                            moves_set->push_back(square | (square + 
                                    double_push) << 6 | NORMAL | pKNIGHT);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Get the arguments for the pawn index function for pawns.
 * @param game: Pointer to game struct.
 * @param square: Square of pawn.
 */
uint64_t Game::pawnMoveArgs(Square square) {
    bool turn = this->turn;
    return (this->sides[!turn] | (this->sides[turn] & files[square % 8]));
}

/**
 * Returns a bitboard of all pawn checkers.
 * @param game: The game struct pointer of the current position.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t Game::getPawnCheckers(Square square, uint64_t* checkers_only) {
    uint64_t enemy_pawns = this->sides[1 - this->turn] & this->pawns;
    uint64_t result = 0;
    int rank_offset = this->turn ? 8 : -8;
    if (square % 8 != 0) {
        int pawn_sq = square + rank_offset - 1;
        if ((1ULL << pawn_sq) & enemy_pawns) {
            *checkers_only |= 1ULL << pawn_sq;
        }
    }

    if (square % 8 != 7) {
        int pawn_sq = square + rank_offset + 1;
        if ((1ULL << pawn_sq) & enemy_pawns) {
            *checkers_only |= 1ULL << pawn_sq;
        }
    }

    return result;
}

/**
 * Sets the moves of the rook for a particular square and occupancy (according
 * to MSBs) and creates the moves. This only happens if the family of moves has
 * not been set yet.
 * @param square: The square that the rook is on.
 * @param move_family: The struct holding the moves family for rooks.
 */
void Game::setRookMoves(int square, MovesStruct* move_family) {
    move_family->reach = 0;

    // Set the reach.
    for (int bit : move_family->block_bits) {
        if (bit < square) { // To left or below.
            if (bit / 8 == square / 8) { // To left.
                int index = square - 1;
                while (index >= bit) {
                    move_family->reach |= 1ULL << index;
                    index--;
                }
            } else { // Below.
                int index = square - 8;
                while (index >= bit) {
                    move_family->reach |= 1ULL << index;
                    index -= 8;
                }
            }
        } else { // To right or above.
            if (bit / 8 == square / 8) { // To right.
                int index = square + 1;
                while (index <= bit) {
                    move_family->reach |= 1ULL << index;
                    index++;
                }
            } else { // Above.
                int index = square + 8;
                while (index <= bit) {
                    move_family->reach |= 1ULL << index;
                    index += 8;
                }
            }
        }
    }

    // Set the moves.
    std::vector<int>* blockers = &move_family->block_bits;
    move_family->move_set.resize(std::pow(2, blockers->size()));

    // Iterate over end occupancies.
    for (int i = 0; i < std::pow(2, blockers->size()); i++) {
        uint64_t pos = move_family->reach;
        
        // Create the position with masked ends.
        int index = 0;
        for (auto j = blockers->begin(); j != blockers->end(); j++) {
            pos = (pos & ~(1ULL << *j)) | ((1ULL & i >> index) << *j);
            index++;
        }

        std::vector<uint16_t>* moves_set = &(move_family->
                move_set[moveSetIndex(pos, move_family)]);

        // Set upward moves.
        index = square + 8;
        while (((pos >> index) & 1ULL) != 0) {
            if (index > 63) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index += 8;
        }

        // Set right moves.
        index = square + 1;
        while (((pos >> index) & 1ULL) != 0) {
            if (index == (square / 8) * 8 + 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index++;
        }

        // Set downward moves.
        index = square - 8;
        while (((pos >> index) & 1ULL) != 0) {
            if (index < 0) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index -= 8;
        }

        // Set left moves.
        index = square - 1;
        while (((pos >> index) & 1ULL) != 0) {
            if (index == (square / 8) * 8 - 1) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index--;
        }
    }
}

/**
 * Computes the rook moves for the corner squares and calls the function
 * to set the moves and reach.
 * @param square: The corner square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Array of move structs for each rook move family.
 */
void Game::computeRCornerMoves(int square, int* offset, std::vector<int>* ROOK_INDEX, 
        MovesStruct* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int hStart = (square / 8) * 8 + 1;
    int hEnd = (square / 8) * 8 + 6;
    int vStart = (square % 8) + 8;
    int vEnd = (square % 8) + 48;

    ROOK_INDEX[square].resize(4096);

    for (int vOcc = 0; vOcc < 64; vOcc++) {
        for (int hOcc = 0; hOcc < 64; hOcc++) {
            // Build position.
            int index;
            Bitboard pos = 0;
            
            // Retrieve and set horizontal bits.
            square % 8 == 0 ? index = 5 : index = 0 ;
            for (int k = hStart; k <= hEnd; k++) {
                pos |= ((hOcc >> index) & 1ULL) << k;
                square % 8 == 0 ? index-- : index++;
            }

            // Retrieve and set vertical bits.
            square / 8 == 0 ? index = 5 : index = 0;
            for (int x = vStart; x <= vEnd; x += 8) {
                pos |= ((vOcc >> index) & 1ULL) << x;
                square / 8 == 0 ? index-- : index++;
            }

            // Index creation.
            int rookIndex = (
                ((pos & rookMasks[square]) * rookMagicNums[square]) >> rookShifts[square]
            );

            int move_index = MSB(hOcc) + 7 * MSB(vOcc) + *offset;
            ROOK_INDEX[square][rookIndex] = move_index;

            if (ROOK_MOVES[move_index].reach == UNSET) {
                ROOK_MOVES[move_index].block_bits.push_back(
                    square / 8 == 0 ? square + 8 * (7 - MSB(vOcc)) : square - 8 * (7 - MSB(vOcc))
                );
                ROOK_MOVES[move_index].block_bits.push_back(
                    square % 8 == 0 ? square + (7 - MSB(hOcc)) : square - (7 - MSB(hOcc))
                );
                std::sort(ROOK_MOVES[move_index].block_bits.begin(), ROOK_MOVES[move_index].block_bits.end());
                Game::setRookMoves(square, &(ROOK_MOVES[move_index]));
            }
        }
    }

    *offset += 49;
}

/**
 * Computes the rook moves for the left and right squares and calls the 
 * function to set the moves and reach.
 * @param square: The corner square of interest.
 * @param offset: The index offset.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Array of move structs for each rook move family.
 */
void Game::computeRLRSideMoves(int square, int* offset, std::vector<int>* ROOK_INDEX,
        MovesStruct* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int hStart = (square / 8) * 8 + 1;
    int hEnd = (square / 8) * 8 + 6;
    int v1_Start = (square % 8) + 8;
    int v1_End = square - 8;
    int v2_Start = square + 8;
    int v2_End = (square % 8) + 48;

    ROOK_INDEX[square].resize(2048);

    int v1_occs = std::pow(2, (v1_End - v1_Start) / 8 + 1);
    int v2_occs = std::pow(2, (v2_End - v2_Start) / 8 + 1);

    int offset_add;

    for (int hOcc = 0; hOcc < 64; hOcc++) {
        for (int vOcc1 = 0; vOcc1 < v1_occs; vOcc1++) {
            for (int vOcc2 = 0; vOcc2 < v2_occs; vOcc2++) {
                // Build position.
                int index;
                uint64_t pos = 0;
                
                // Retrieve and set horizontal bits.
                square % 8 == 0 ? index = 5 : index = 0;
                for (int k = hStart; k <= hEnd; k++) {
                    pos |= ((hOcc >> index) & 1ULL) << k;
                    square % 8 == 0 ? index-- : index++;
                }

                // Retrieve and set vertical1 bits.
                if (v1_occs != 1) {
                    index = 0;
                    for (int x = v1_Start; x <= v1_End; x += 8) {
                        pos |= ((vOcc1 >> index) & 1ULL) << x;
                        index++;
                    }
                }

                // Retrieve and set vertical2 bits.
                if (v2_occs != 1) {
                    index = (v2_End - v2_Start) / 8;
                    for (int x = v2_Start; x <= v2_End; x += 8) {
                        pos |= ((vOcc2 >> index) & 1ULL) << x;
                        index--;
                    }
                }

                // Index creation.
                uint64_t rookIndex = (
                    ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
                            rookShifts[square]
                );

                int move_index;
                if (square / 8 == 1) { // A2 H2
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 42;
                } else if (square / 8 == 2) { // A3 H3
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + 14 * MSB(vOcc2) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 70;
                } else if (square / 8 == 3) { // A4 H4
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + 21 * MSB(vOcc2) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 84;
                } else if (square / 8 == 4) { // A5 H5
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + 21 * MSB(vOcc1) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 84;
                } else if (square / 8 == 5) { // A6 H6
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + 14 * MSB(vOcc1) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 70;
                } else { // A7 H7
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 42;
                }
                
                if (ROOK_MOVES[move_index].reach == UNSET) {
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square + 8 * ((7 - square / 8) - MSB(vOcc2))
                    );
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square - 8 * ((square / 8) - MSB(vOcc1))
                    );
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square % 8 == 0 ? square + (7 - MSB(hOcc)) :
                                square - (7 - MSB(hOcc))
                    );
                    std::sort(ROOK_MOVES[move_index].block_bits.begin(),
                            ROOK_MOVES[move_index].block_bits.end());
                    setRookMoves(square, &(ROOK_MOVES[move_index]));
                }
            }
        }
    }

    *offset += offset_add;
}

/**
 * Computes the rook moves for the upper and lower squares and calls the
 * function to set the moves and reach.
 * @param square: The corner square of interest.
 * @param offset: The index offset.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Array of move structs for each rook move family.
 */
void Game::computeRULSideMoves(int square, int* offset, std::vector<int>* ROOK_INDEX,
        MovesStruct* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int h1_Start = (square / 8) * 8 + 1;
    int h1_End = square - 1;
    int h2_Start = square + 1;
    int h2_End = (square / 8) * 8 + 6;
    int vStart = (square % 8) + 8;
    int vEnd = (square % 8) + 48;

    ROOK_INDEX[square].resize(2048);

    int h1_occs = std::pow(2, h1_End - h1_Start + 1);
    int h2_occs = std::pow(2, h2_End - h2_Start + 1);

    int offset_add;

    for (int vOcc = 0; vOcc < 64; vOcc++) {
        for (int hOcc1 = 0; hOcc1 < h1_occs; hOcc1++) {
            for (int hOcc2 = 0; hOcc2 < h2_occs; hOcc2++) {
                // Build position.
                int index;
                uint64_t pos = 0;
                
                // Retrieve and set vertical bits.
                square / 8 == 0 ? index = 5 : index = 0;
                for (int k = vStart; k <= vEnd; k += 8) {
                    pos |= ((vOcc >> index) & 1ULL) << k;
                    square / 8 == 0 ? index-- : index++;
                }

                // Retrieve and set horizontal1 bits.
                if (h1_occs != 1) {
                    index = 0;
                    for (int x = h1_Start; x <= h1_End; x++) {
                        pos |= ((hOcc1 >> index) & 1ULL) << x;
                        index++;
                    }
                }

                // Retrieve and set horizontal2 bits.
                if (h2_occs != 1) {
                    index = h2_End - h2_Start;
                    for (int x = h2_Start; x <= h2_End; x++) {
                        pos |= ((hOcc2 >> index) & 1ULL) << x;
                        index--;
                    }
                }

                // Index creation.
                uint64_t rookIndex = (
                    ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
                            rookShifts[square]
                );

                int move_index;
                if (square % 8 == 1) { // B1 B8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 42;
                } else if (square % 8 == 2) { // C1 C8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + 14 * MSB(hOcc2) + 
                            *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 70;
                } else if (square % 8 == 3) { // D1 D8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + 21 * MSB(hOcc2) + 
                            *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 84;
                } else if (square % 8 == 4) { // E1 E8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + 21 * MSB(hOcc1) + 
                            *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 84;
                } else if (square % 8 == 5) { // F1 F8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + 14 * MSB(hOcc1) + 
                            *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 70;
                } else { // G1 G8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 42;
                }

                if (ROOK_MOVES[move_index].reach == UNSET) {
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square / 8 == 0 ? square + 8 * (7 - MSB(vOcc)) :
                            square - 8 * (7 - MSB(vOcc))
                    );
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square - ((square % 8) - MSB(hOcc1))
                    );
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square + (7 - square % 8) - MSB(hOcc2)
                    );
                    std::sort(ROOK_MOVES[move_index].block_bits.begin(),
                            ROOK_MOVES[move_index].block_bits.end());
                    setRookMoves(square, &(ROOK_MOVES[move_index]));
                }
            }
        }
    }

    *offset += offset_add;
}

/**
 * Computes the rook moves for the center squares and calls the function to set
 * the moves and reach.
 * @param square: The corner square of interest.
 * @param offset: The index offset.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Array of move structs for each rook move family.
 */
void Game::computeRCentreMoves(int square, int* offset, std::vector<int>* ROOK_INDEX,
        MovesStruct* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int h1_Start = (square / 8) * 8 + 1;
    int h1_End = square - 1;
    int h2_Start = square + 1;
    int h2_End = (square / 8) * 8 + 6;
    int v1_Start = (square % 8) + 8;
    int v1_End = square - 8;
    int v2_Start = square + 8;
    int v2_End = (square % 8) + 48;

    ROOK_INDEX[square].resize(1024);

    int h1_occs = std::pow(2, h1_End - h1_Start + 1);
    int h2_occs = std::pow(2, h2_End - h2_Start + 1);
    int v1_occs = std::pow(2, (v1_End - v1_Start) / 8 + 1);
    int v2_occs = std::pow(2, (v2_End - v2_Start) / 8 + 1);

    int offset_add;
    for (int hOcc1 = 0; hOcc1 < h1_occs; hOcc1++) {
        for (int hOcc2 = 0; hOcc2 < h2_occs; hOcc2++) {
            for (int vOcc1 = 0; vOcc1 < v1_occs; vOcc1++) {
                for (int vOcc2 = 0; vOcc2 < v2_occs; vOcc2++) {
                    // Build position.
                    int index;
                    uint64_t pos = 0;

                    // Retrieve and set horizontal1 bits.
                    if (h1_occs != 1) {
                        index = 0;
                        for (int x = h1_Start; x <= h1_End; x++) {
                            pos |= ((hOcc1 >> index) & 1ULL) << x;
                            index++;
                        }
                    }

                    // Retrieve and set horizontal2 bits.
                    if (h2_occs != 1) {
                        index = h2_End - h2_Start;
                        for (int x = h2_Start; x <= h2_End; x++) {
                            pos |= ((hOcc2 >> index) & 1ULL) << x;
                            index--;
                        }
                    }

                    // Retrieve and set vertical1 bits.
                    if (v1_occs != 1) {
                        index = 0;
                        for (int x = v1_Start; x <= v1_End; x += 8) {
                            pos |= ((vOcc1 >> index) & 1ULL) << x;
                            index++;
                        }
                    }

                    // Retrieve and set vertical2 bits.
                    if (v2_occs != 1) {
                        index = (v2_End - v2_Start) / 8;
                        for (int x = v2_Start; x <= v2_End; x += 8) {
                            pos |= ((vOcc2 >> index) & 1ULL) << x;
                            index--;
                        }
                    }

                    // Index creation.
                    uint64_t rookIndex = (
                        ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
                                rookShifts[square]
                    );

                    int move_index;
                    if (square == B2 || square == B7 || square == G2 || 
                            square == G7) {
                        move_index = MSB((h1_occs != 1 ? hOcc1 : hOcc2)) + 
                                6 * MSB((v1_occs != 1 ? vOcc1 : vOcc2)) + 
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 36;
                    } else if (square == C2 || square == F2 || square == C7 || 
                            square == F7 || square == B3 || square == G3 || 
                            square == B6 || square == G6) {
                        move_index = MSB(h1_occs == 32 ? hOcc1 : 
                                    (h2_occs == 32 ? hOcc2 : 
                                    (v1_occs == 32 ? vOcc1 : vOcc2))) +
                                6 * MSB(h1_occs == 2 ? hOcc1 : 
                                    (h2_occs == 2 ? hOcc2 : 
                                    (v1_occs == 2 ? vOcc1 : vOcc2))) +
                                12 * MSB(h1_occs == 16 ? hOcc1 : 
                                    (h2_occs == 16 ? hOcc2 : 
                                    (v1_occs == 16 ? vOcc1 : vOcc2))) +
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 60;
                    } else if (square == D2 || square == E2 || square == B4 || 
                            square == G4 || square == B5 || square == G5 || 
                            square == D7 || square == E7) {
                        move_index = MSB(h1_occs == 32 ? hOcc1 : 
                                    (h2_occs == 32 ? hOcc2 : 
                                    (v1_occs == 32 ? vOcc1 : vOcc2))) +
                                6 * MSB(h1_occs == 8 ? hOcc1 : 
                                    (h2_occs == 8 ? hOcc2 : 
                                    (v1_occs == 8 ? vOcc1 : vOcc2))) +
                                24 * MSB(h1_occs == 4 ? hOcc1 : 
                                    (h2_occs == 4 ? hOcc2 : 
                                    (v1_occs == 4 ? vOcc1 : vOcc2))) +
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 72;
                    } else if (square == C3 || square == F3 || square == C6 || 
                            square == F6) {
                        move_index = MSB(h1_occs == 16 ? hOcc1 : hOcc2) +
                                25 * MSB(h1_occs == 2 ? hOcc1 : hOcc2) +
                                5 * MSB(v1_occs == 16 ? vOcc1 : vOcc2) +
                                50 * MSB(v1_occs == 2 ? vOcc1 : vOcc2) +
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 100;
                    } else if (square == D3 || square == E3 || square == C4 || 
                            square == F4 || square == C5 || square == F5 || 
                            square == D6 || square == E6) {
                        move_index = MSB(h1_occs == 16 ? hOcc1 :
                                    (h2_occs == 16 ? hOcc2 :
                                    (v1_occs == 16 ? vOcc1 :vOcc2))) +
                                5 * MSB(h1_occs == 8 ? hOcc1 :
                                    (h2_occs == 8 ? hOcc2 :
                                    (v1_occs == 8 ? vOcc1 : vOcc2))) +
                                20 * MSB(h1_occs == 4 ? hOcc1 :
                                    (h2_occs == 4 ? hOcc2 :
                                    (v1_occs == 4 ? vOcc1 : vOcc2))) +
                                60 * MSB(h1_occs == 2 ? hOcc1 :
                                    (h2_occs == 2 ? hOcc2 :
                                    (v1_occs == 2 ? vOcc1 : vOcc2))) +
                                    *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 120;
                    } else {
                        move_index = MSB(h1_occs == 8 ? hOcc1 : hOcc2) +
                                4 * MSB(v1_occs == 8 ? vOcc1 : vOcc2) +
                                16 * MSB(v1_occs == 4 ? vOcc1 : vOcc2) +
                                48 * MSB(h1_occs == 4 ? hOcc1 : hOcc2) +
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 144;
                    }

                    if (ROOK_MOVES[move_index].reach == UNSET) {
                        ROOK_MOVES[move_index].block_bits.push_back(
                            square - 8 * ((square / 8) - MSB(vOcc1))
                        );
                        ROOK_MOVES[move_index].block_bits.push_back(
                            square - ((square % 8) - MSB(hOcc1))
                        );
                        ROOK_MOVES[move_index].block_bits.push_back(
                            square + (7 - square % 8) - MSB(hOcc2)
                        );
                        ROOK_MOVES[move_index].block_bits.push_back(
                            square + 8 * ((7 - square / 8) - MSB(vOcc2))
                        );

                        std::sort(ROOK_MOVES[move_index].block_bits.begin(),
                                ROOK_MOVES[move_index].block_bits.end());
                        setRookMoves(square, &(ROOK_MOVES[move_index]));
                    }
                }
            }
        }
    }

    *offset += offset_add;
}

/**
 * Returns the index from ROOK_INDEX into ROOK_MOVES based on the square and 
 * occupancy.
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 * @return: Index into ROOK_MOVES.
 */
const int Game::rookIndex(const uint64_t pos, Square square) {
    return ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
            rookShifts[square];
}

/**
 * Computes the index (in this case the key) into the ROOK_BLOCK moves map.
 * @param pos: The uint64_t with possible destination squares from the param
 *      square set.
 * @param moves: The precomputed moves.
 * @param square: The square from which the piece moves.
 * @param game: A Game struct pointer.
 * @return: Index into the ROOK_BLOCK array.
 */
const int Game::rookBlockIndex(uint64_t pos, Computed* moves, Square square) {
    return pos & this->getRookFamily(moves, square)->reach;
}

/**
 * Gets a bitboard of the rays between the rook (queen) checkers and the king
 * being checked.
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers: Bitboard of checkers.
 * @return: Check rays of rooks and queens.
 */
uint64_t Game::getRookCheckRays(Computed* moves, Square square, uint64_t* checkers) {
    uint64_t result = 0;
    Square king_sq = this->piece_list[this->turn][0];
    uint64_t king_rays = this->getRookFamily(moves, king_sq)->reach & ~this->sides[this->turn];
    *checkers |= (this->queens | this->rooks) & this->sides[1 - this->turn] & king_rays;
    PieceType queen = this->turn ? B_QUEEN : W_QUEEN;
    PieceType rook = this->turn ? B_ROOK : W_ROOK;

    for (int i = 0; i < this->piece_index[queen]; i++) {
        Square queen_sq = this->piece_list[queen][i];
        if (queen_sq % 8 == king_sq % 8 || queen_sq / 8 == king_sq / 8) {
            result |= (this->getRookFamily(moves, (Square) queen_sq)->reach & 
                    ~this->sides[1 - this->turn] & king_rays);
        }
    }

    for (int i = 0; i < this->piece_index[rook]; i++) {
        Square rook_sq = this->piece_list[rook][i];
        if (rook_sq % 8 == king_sq % 8 || rook_sq / 8 == king_sq / 8) {
            result |= (this->getRookFamily(moves, (Square) rook_sq)->reach & 
                    ~this->sides[1 - this->turn] & king_rays);
        }
    }

    return result;
}

/**
 * Iterates through the squares and call functions to compute rook moves and
 * calculate indices.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Move struct to store the moves.
 */
void Game::computeRookMoves(std::vector<int>* ROOK_INDEX, MovesStruct* ROOK_MOVES) {
    // Set initial reach to UNSET.
    for (int i = 0; i < 4900; i++) {
        ROOK_MOVES[i].reach = UNSET;
    }

    // Set the moves and reach.
    int offset = 0;
    for (int square = A1; square <= H8; square++) {
        if (square == A1 || square == A8 || square == H1 || square == H8) {
            Game::computeRCornerMoves(square, &offset, ROOK_INDEX, ROOK_MOVES);
        } else if (square % 8 == 0 || square % 8 == 7) {
            Game::computeRLRSideMoves(square, &offset, ROOK_INDEX, ROOK_MOVES);
        } else if (square / 8 == 0 || square / 8 == 7) {
            Game::computeRULSideMoves(square, &offset, ROOK_INDEX, ROOK_MOVES);
        } else {
            Game::computeRCentreMoves(square, &offset, ROOK_INDEX, ROOK_MOVES);
        }
    }
}

/**
 * Computes the blocking moves (and captures in case of check) for rooks on each
 * square.
 * @param ROOK_BLOCKS: A vector of move structs.
 */
void Game::computeRookBlocks(MovesStruct* ROOK_BLOCKS) {
    std::tuple<int, int> pairs[4] = {
        std::make_tuple(8, 1), std::make_tuple(1, -8), std::make_tuple(-8, -1),
        std::make_tuple(-1, 8)
    };
    int directions[4] = {8, 1, -8, -1};

    for (int square = A1; square <= H8; square++) {
        
        // Single ray traversals.
        for (int direction : directions) {
            if ((direction == 8 && square / 8 == 7) || (direction == 1 && 
                    square % 8 == 7) || (direction == -8 && square / 8 == 0) || 
                    (direction == -1 && square % 8 == 0)) {
                continue;
            }

            int start = square + direction;
            int end_pt = std::abs(direction) == 8 ? (start % 8 + 
                    (direction == 8 ? 64 : -8)) : (8 * (start / 8) + 
                    (direction == 1 ? 8 : -1));

            std::unordered_map<uint64_t, std::vector<uint16_t>>* moves = 
                    &ROOK_BLOCKS[square].checked_moves;
            while (start != end_pt) {
                // moves[pos] = std::vector<uint16_t>(); WHY DOESN'T WORK????
                uint16_t move = square | start << 6 | NORMAL | pKNIGHT;
                std::vector<uint16_t> moveset = {move};
                moves->insert(std::make_pair(1ULL << start, moveset));
                start += direction;
            }
        }

        // Pair ray traversal.
        for (std::tuple<int, int> tuple : pairs) {
            int ray1 = std::get<0>(tuple);
            int ray2 = std::get<1>(tuple);

            if ((square / 8 == 0 && (ray1 == -8 || ray2 == -8)) || 
                    (square / 8 == 7 && (ray1 == 8 || ray2 == 8)) || 
                    (square % 8 == 0 && (ray1 == -1 || ray2 == -1)) || 
                    (square % 8 == 7 && (ray1 == 1 || ray2 == 1)) ) {
                continue;
            }

            int start1 = square + ray1, start2 = square + ray2; 
            int end1, end2;

            end1 = std::abs(ray1) == 8 ? (start1 % 8 + (ray1 == 8 ? 64 : -8)) :
                    (8 * (start1 / 8) + (ray1 == 1 ? 8 : -1));
            end2 = std::abs(ray2) == 8 ? (start2 % 8 + (ray2 == 8 ? 64 : -8)) :
                    (8 * (start2 / 8) + (ray2 == 1 ? 8 : -1));
            
            std::unordered_map<uint64_t, std::vector<uint16_t>>* moves = 
                    &ROOK_BLOCKS[square].checked_moves;

            while (start1 != end1) {
                start2 = square + ray2;
                while (start2 != end2) {
                    uint16_t move1 = square | start1 << 6 | NORMAL | pKNIGHT;
                    uint16_t move2 = square | start2 << 6 | NORMAL | pKNIGHT;
                    std::vector<uint16_t> moveset = {move1, move2};
                    moves->insert(std::make_pair(1ULL << start1 | 1ULL << start2, 
                            moveset));
                    start2 += ray2;
                }
                start1 += ray1;
            };
        };
    }
}

/**
 * Finds and returns a pointer to a rook move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* Game::getRookFamily(Computed* moves, Square square) {
    &moves->ROOK_MOVES[moves->ROOK_INDEX[square][rookIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
    return &moves->ROOK_MOVES[moves->ROOK_INDEX[square][
            rookIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
}

/**
 * Finds and returns a pointer to a rook block move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* getRookBlockFamily(Game* game, Computed* moves, Square square) {
    return nullptr;
}
