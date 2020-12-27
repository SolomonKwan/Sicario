
#include <iostream>
#include <unordered_map>
#include <bitset>

#include "fen.hpp"
#include "game.hpp"
#include "test.hpp"
#include "search.hpp"
#include "movegen.hpp"

namespace Moves_ {
    std::vector<MovesStruct> ROOK = computeRookMoves(); // 4900

    /////////////////////////////////////////

    std::vector<MovesStruct> BISHOP_MOVES; // 1428
    std::vector<MovesStruct> BISHOP_BLOCKS; // 64

    /////////////////////////////////////////

    std::vector<MovesStruct> KNIGHT_MOVES; // 64

    /////////////////////////////////////////

    std::vector<MovesStruct> KING_MOVES; // 64

    /////////////////////////////////////////

    std::vector<MovesStruct> PAWN_MOVES[2] = {}; // each 48

    /////////////////////////////////////////

    std::vector<MovesStruct> CASTLING_MOVES; // 4

    /////////////////////////////////////////

    std::vector<MovesStruct> EN_PASSANT_MOVES; // 16

    /////////////////////////////////////////

    std::vector<MovesStruct> DOUBLE_PUSH; // 16

    namespace Blocks {
        std::vector<MovesStruct> ROOK = computeRookBlocks(); // 64
    }
}

namespace Indices {
    const std::vector<std::vector<int>> ROOK = computeRookIndices();
    const std::vector<std::vector<int>> BISHOP = computeBishopIndices();
}

/**
 * Display all individual game position information.
 * @param game: Pointer to game struct.
 */
void Pos::displayAll(Pos* game) {
    std::cout << "White";
    displayBB(this->sides[WHITE]);

    std::cout << "Black";
    displayBB(this->sides[BLACK]);

    std::cout << "Kings";
    displayBB(this->kings);

    std::cout << "Queens";
    displayBB(this->queens);

    std::cout << "Rooks";
    displayBB(this->rooks);

    std::cout << "Bishops";
    displayBB(this->bishops);

    std::cout << "Knights";
    displayBB(this->knights);

    std::cout << "pawns";
    displayBB(this->pawns);

    std::cout << (this->turn ? "White" : "Black") << '\n';

    std::cout << "Castling: " << std::bitset<4>(this->castling) << '\n';

    std::cout << "En-passant: " << squareName[this->en_passant] << '\n';

    std::cout << "Halfmove: " << this->halfmove << '\n';
    
    std::cout << "Fullmove: " << this->fullmove << "\n\n";

    for (int i = 0; i < 12; i++) {
        std::cout << piece_type_string[i] << " " << this->piece_index[i] << 
                " :";
        for (int j = 0; j < this->piece_index[i]; j++) {
            std::cout << squareName[this->piece_list[i][j]] << " ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    for (int i = 0; i < 64; i++) {
        std::cout << squareName[i] << ":" << piece_type_string[this->pieces[i]] 
                << " ";
        if (i % 8 == 7) std::cout << '\n';
    }
}

/**
 * Returns whether or not a fen is valid. STILL TO BE UTILISED...   
 * 
 * @param fen: The fen string to check.
 * 
 * @return: True if valid, else false.
 */
bool goodFen(std::string fen) {
    return true;
}

/**
 * Parses the fen string into a game struct.
 * 
 * @param game: Pointer to game struct.
 * @param fen: The fen string.
 * 
 * @return: INVALID_FEN if fen is invalid, else NORMAL_PLY.
 */
ExitCode Pos::parseFen(std::string fen) {
    if (!goodFen(fen)) return INVALID_FEN;

    // Zero out variables.
    this->zero();

    // Record the original FEN string.
    this->original_fen = fen;

    // Split the string into parts and ranks
    std::vector<std::string> parts = split(fen, " ");
    std::vector<std::string> ranks = split(parts[0], "/");

    // Set the pieces
    int rank = 7;
    for (std::string rank_string : ranks) {
        int file = 0;

        for (char c : rank_string) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                for (int i = 0; i < (c - '0'); i++) {
                    this->pieces[8 * rank + file] = NO_PIECE;
                    file++;
                }
            } else {
                if (c == 'K') {
                    this->kings |= 1ULL << (8 * rank + file);
                    this->piece_list[WHITE][0] = (Square) (8 * rank + file);
                    this->piece_index[W_KING]++;
                    this->pieces[8 * rank + file] = W_KING;
                } else if (c == 'Q') {
                    this->queens |= 1ULL << (8 * rank + file);
                    this->piece_list[W_QUEEN][this->piece_index[W_QUEEN]] = 
                            (Square)(8 * rank + file);
                    this->piece_index[W_QUEEN]++;
                    this->pieces[8 * rank + file] = W_QUEEN;
                } else if (c == 'R') {
                    this->rooks |= 1ULL << (8 * rank + file);
                    this->piece_list[W_ROOK][this->piece_index[W_ROOK]] =
                            (Square)(8 * rank + file);
                    this->piece_index[W_ROOK]++;
                    this->pieces[8 * rank + file] = W_ROOK;
                } else if (c == 'B') {
                    this->bishops |= 1ULL << (8 * rank + file);
                    this->piece_list[W_BISHOP][this->piece_index[W_BISHOP]] =
                            (Square)(8 * rank + file);
                    this->piece_index[W_BISHOP]++;
                    this->pieces[8 * rank + file] = W_BISHOP;
                    if (isDark(8 * rank + file)) {
                        this->wdsb_cnt++;
                    } else {
                        this->wlsb_cnt++;
                    }
                } else if (c == 'N') {
                    this->knights |= 1ULL << (8 * rank + file);
                    this->piece_list[W_KNIGHT][this->piece_index[W_KNIGHT]] =
                            (Square)(8 * rank + file);
                    this->piece_index[W_KNIGHT]++;
                    this->pieces[8 * rank + file] = W_KNIGHT;
                    this->knight_cnt++;
                } else if (c == 'P') {
                    this->pawns |= 1ULL << (8 * rank + file);
                    this->piece_list[W_PAWN][this->piece_index[W_PAWN]] =
                            (Square)(8 * rank + file);
                    this->piece_index[W_PAWN]++;
                    this->pieces[8 * rank + file] = W_PAWN;
                } else if (c == 'k') {
                    this->kings |= 1ULL << (8 * rank + file);
                    this->piece_list[BLACK][0] = (Square) (8 * rank + file);
                    this->piece_index[B_KING]++;
                    this->pieces[8 * rank + file] = B_KING;
                } else if (c == 'q') {
                    this->queens |= 1ULL << (8 * rank + file);
                    this->piece_list[B_QUEEN][this->piece_index[B_QUEEN]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_QUEEN]++;
                    this->pieces[8 * rank + file] = B_QUEEN;
                } else if (c == 'r') {
                    this->rooks |= 1ULL << (8 * rank + file);
                    this->piece_list[B_ROOK][this->piece_index[B_ROOK]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_ROOK]++;
                    this->pieces[8 * rank + file] = B_ROOK;
                } else if (c == 'b') {
                    this->bishops |= 1ULL << (8 * rank + file);
                    this->piece_list[B_BISHOP][this->piece_index[B_BISHOP]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_BISHOP]++;
                    this->pieces[8 * rank + file] = B_BISHOP;
                    if (isDark(8 * rank + file)) {
                        this->bdsb_cnt++;
                    } else {
                        this->blsb_cnt++;
                    }
                } else if (c == 'n') {
                    this->knights |= 1ULL << (8 * rank + file);
                    this->piece_list[B_KNIGHT][this->piece_index[B_KNIGHT]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_KNIGHT]++;
                    this->pieces[8 * rank + file] = B_KNIGHT;
                    this->knight_cnt++;
                } else {
                    this->pawns |= 1ULL << (8 * rank + file);
                    this->piece_list[B_PAWN][this->piece_index[B_PAWN]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_PAWN]++;
                    this->pieces[8 * rank + file] = B_PAWN;
                }

                // Set sides and piece count
                if (std::isupper(static_cast<unsigned char>(c))) {
                    this->sides[WHITE] |= 1ULL << (8 * rank + file);
                } else {
                    this->sides[BLACK] |= 1ULL << (8 * rank + file);
                }
                this->piece_cnt++;

                file++;
            }
        }

        rank--;
    }

    // Set turn
    if (parts[1] == "w") {
        this->turn = WHITE;
    } else {
        this->turn = BLACK;
    }

    // Set castling
    this->castling = 0;
    if (parts[2] != "-") {
        for (char c : parts[2]) {
            if (c == 'K') {
                this->castling |= 1 << WKSC;
            } else if (c == 'Q') {
                this->castling |= 1 << WQSC;
            } else if (c == 'k') {
                this->castling |= 1 << BKSC;
            } else {
                this->castling |= 1 << BQSC;
            }
        }
    }

    // Set en-passant
    if (parts[3] != "-") {
        int value = -1;
        for (char c : parts[3]) {
            if (value == -1) {
                value = (c - 'a');
            } else {
                value += 8 * (c - '1');
            }
        }
        this->en_passant = (Square) value;
    } else {
        this->en_passant = NONE;
    }

    // Set fullmoves and halfmoves.
    this->halfmove = std::stoi(parts[4]);
    this->fullmove = std::stoi(parts[5]);

    // Set ply and position hash to zero.
    this->ply = 0;
    this->hash = 0ULL;

    return NORMAL_PLY;
}

void Pos::zero() {
    this->sides[WHITE] = 0ULL;
    this->sides[BLACK] = 0ULL;
    this->kings = 0ULL;
    this->queens = 0ULL;
    this->rooks = 0ULL;
    this->bishops = 0ULL;
    this->knights = 0ULL;
    this->pawns = 0ULL;

    std::fill(this->piece_list[0] + 0, this->piece_list[12] + 10, NONE);
    std::fill(std::begin(this->piece_index), std::end(this->piece_index), 0);
    std::fill(std::begin(this->pieces), std::end(this->pieces), NO_PIECE);

    this->piece_cnt = 0;
    this->knight_cnt = 0;
    this->wdsb_cnt = 0;
    this->wlsb_cnt = 0;
    this->bdsb_cnt = 0;
    this->blsb_cnt = 0;
}

/**
 * Show the usage of the program.
 * 
 * @param argv: Array of command line arguments.
 */
void showUsage(char *argv[]) {
    std::cerr << "\nUsage: " << argv[0] << " white black [options]\n";
    std::cerr << "    white - Type of player for white (c or h)\n";
    std::cerr << "    black - Type of player for white (c or h)\n\n";
    std::cerr << "Options:\n";
    std::cerr << "    -f FEN            FEN input string in quotes (' or \").";
    std::cerr << "\n    -h --help         Print usage information.\n";
    std::cerr << "    -i                Invert piece colours.\n";
    std::cerr << "    -p                Print pgn.\n";
    std::cerr << "    -q                Supress output.\n";
    std::cerr << "    -u                Show unicode chess chars.\n";
    std::cerr << "    --version         Show version.\n\n";
    std::cerr << "Compiled by running 'make'.\n";
    std::cerr << "Made in development using GNU Make 4.1.\n\n";
}

/**
 * Show the end of game message.
 * @param code: The exitcode of the game.
 * @param argv: Array of command line arguments.
 */
void Pos::showEOG(ExitCode code, char *argv[]) {
    switch (code) {
        case NORMAL_PLY:
            std::cout << "Debugging: normal\n";
            break;

        case WHITE_WINS:
            std::cout << "Checkmate, white wins\n";
            break;
        
        case BLACK_WINS:
            std::cout << "Checkmate, black wins\n";
            break;
        
        case STALEMATE:
            std::cout << "Draw by stalemate\n";
            break;
        
        case THREE_FOLD_REPETITION:
            std::cout << "Draw by three-fold repetition\n";
            break;
        
        case FIFTY_MOVES_RULE:
            std::cout << "Draw by fifty move rule\n";
            break;
        
        case INSUFFICIENT_MATERIAL:
            std::cout << "Draw by insufficient material\n";
            break;
        
        case DRAW_BY_AGREEMENT:
            std::cout << "Draw by agreement\n";
            break;
        
        case INVALID_FEN:
            std::cout << "Invalid FEN string\n";
            break;
        
        case INVALID_ARGS:
            showUsage(argv);
            break;

        default:
            std::cout << "Should not happen...\n";
    }
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
const int Pos::rookBlockIndex(uint64_t pos, Computed* moves, Square square) {
    return pos & this->getRookFamily(moves, square)->reach;
}

/**
 * Check if draw by insufficient material.
 * 
 * @param game: Pointer to game struct.
 * @return: True if draw, else false.
 */
bool Pos::insufficientMaterial() {
    // King vs king
    if (this->piece_cnt == 2) return true;

    // King and bishop(s) vs king (same colour bishop(s))
    if ((this->wlsb_cnt && (this->piece_cnt - this->wlsb_cnt) == 2) ||
            (this->wdsb_cnt && (this->piece_cnt - this->wdsb_cnt) == 2) ||
            (this->blsb_cnt && (this->piece_cnt - this->blsb_cnt) == 2) ||
            (this->bdsb_cnt && (this->piece_cnt - this->bdsb_cnt) == 2)) {
        return true;
    }

    // King and bishop(s) vs king and bishop(s) (same colour bishop(s))
    if ((this->wlsb_cnt && this->blsb_cnt && (this->piece_cnt - 
            this->wlsb_cnt - this->blsb_cnt) == 2) || (this->wdsb_cnt && 
            this->bdsb_cnt && (this->piece_cnt - this->wdsb_cnt - 
            this->bdsb_cnt) == 2)) {
        return true;
    }

    // King and knight vs king
    if (this->knight_cnt == 1 && this->piece_cnt == 3) return true;

    return false;
}

/**
 * Check if king of current player is in check.
 * @param game: Pointer to game struct.
 * @param enemy_attacks: Bitboard of squares attacked by enemy pieces.
 * @return: True if in check, else false.
 */
bool Pos::isChecked(uint64_t enemy_attacks) {
    return (1ULL << this->piece_list[this->turn][0]) & enemy_attacks;
}

/**
 * Sets all the legal moves a king can make in a given position.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getKingMoves(Computed* computed_moves, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, 
        uint64_t kEnemy_attacks) {
    MovesStruct* king_family = &computed_moves->KING_MOVES[this->piece_list[this->turn][0]];
    std::vector<uint16_t>* move_set = &king_family->move_set[
            moveSetIndex((king_family->reach ^ this->sides[this->turn]) & 
            ~kEnemy_attacks, king_family)];
    if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
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
void Pos::getCheckedMoves(Computed* moves, uint64_t* enemy_attacks, 
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
        MovesStruct* bishop_move = &moves->BISHOP_MOVES[Indices::BISHOP[queen][
                bishopIndex(this->sides[BLACK] | this->sides[WHITE], 
                        (Square) queen)]];
        uint64_t bishop_index = bishop_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<uint16_t>* move_set = &moves->BISHOP_BLOCKS[queen].
                checked_moves[bishop_index];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;

        MovesStruct* rook_move = &Moves_::ROOK[Indices::ROOK[queen][
                rookIndex(this->sides[BLACK] | this->sides[WHITE], 
                (Square) queen)]];
        uint64_t rook_index = rook_move->reach & (check_rays_only | 
                checkers_only);
        move_set = &Moves_::Blocks::ROOK[queen].checked_moves[rook_index];
        if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
    }

    // Rook checked moves.
    piece = this->turn ? W_ROOK : B_ROOK;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int rook = this->piece_list[piece][i];
        if ((1ULL << rook) & *rook_pins || (1ULL << rook) & *bishop_pins) {
            continue;
        }
        MovesStruct* rook_move = &Moves_::ROOK[Indices::ROOK[rook][
                rookIndex(this->sides[BLACK] | this->sides[WHITE], 
                (Square) rook)]];
        uint64_t rook_index = rook_move->reach & (check_rays_only | checkers_only);
        std::vector<uint16_t>* move_set = &Moves_::Blocks::ROOK[rook].checked_moves[rook_index];
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
        MovesStruct* bishop_move = &moves->BISHOP_MOVES[Indices::BISHOP[bishop][
                bishopIndex(this->sides[BLACK] | this->sides[WHITE], 
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
 * Gets a bitboard of the rays between the bishop (queen) checkers and the king
 * being checked.
 * 
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t Pos::getBishopCheckRays(Computed* moves, Square square, uint64_t* checkers_only) {
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
 * Gets a bitboard of the rays between the rook (queen) checkers and the king
 * being checked.
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers: Bitboard of checkers.
 * @return: Check rays of rooks and queens.
 */
uint64_t Pos::getRookCheckRays(Computed* moves, Square square, uint64_t* checkers) {
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
 * Returns a bitboard of all pawn checkers.
 * @param game: The game struct pointer of the current position.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t Pos::getPawnCheckers(Square square, uint64_t* checkers_only) {
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
 * Gets the en-passant moves for a normal move of the game.
 * @param game: A game struct pointer.
 * @param rook_pins: Bitboard of pieces pinned horiztonally or vertically.
 * @param bishop_pins: Bitboard of pieces pinned diagonally.
 * @param checkers: The bitboard of checkers.
 * @param moves: Precomputed moves struct pointer.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getCheckedEp(uint64_t* rook_pins, uint64_t* bishop_pins, 
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
 * Gets all legal moves for the player whose turn it is.
 * @param game: A game struct pointer.
 * @param computed_moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getNormalMoves(Computed* moves, uint64_t* enemy_attacks, 
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
 * Get queen moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getQueenMoves(Computed* computed_moves, uint64_t rook_pins, 
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
 * Get rook or queen moves if piece is pinned horizontally or vertically.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the piece pinned.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getRookPinMoves(Computed* moves, int square, 
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
    MovesStruct* rook_moves = &Moves_::ROOK[Indices::ROOK[square][rookIndex(
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
void Pos::getBishopPinMoves (Computed* moves, int square, 
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
    MovesStruct* bishop_moves = &moves->BISHOP_MOVES[Indices::BISHOP[square]
            [bishopIndex(pos, (Square) square)]];
    std::vector<uint16_t>* move_set = &bishop_moves->move_set[moveSetIndex(
            bishop_moves->reach ^ (this->sides[this->turn] | friendly), 
            bishop_moves)];
    if (move_set->size() != 0) pos_moves[(*moves_index)++] = move_set;
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
void Pos::getRookMoves(Computed* computed_moves, uint64_t rook_pins, 
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
void Pos::getBishopMoves(Computed* computed_moves, uint64_t rook_pins, 
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
void Pos::getKnightMoves(Computed* computed_moves, uint64_t rook_pins, 
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
void Pos::getPawnMoves(Computed* computed_moves, uint64_t rook_pins, 
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
 * Get the arguments for the pawn index function for pawns.
 * @param game: Pointer to game struct.
 * @param square: Square of pawn.
 */
uint64_t Pos::pawnMoveArgs(Square square) {
    bool turn = this->turn;
    return (this->sides[!turn] | (this->sides[turn] & files[square % 8]));
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
void Pos::getCastlingMoves(uint64_t enemy_attacks, Computed* moves, 
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
 * Gets the en-passant moves for a normal move of the game.
 * @param game: A game struct pointer.
 * @param rook_pins: Bitboard of pieces pinned horiztonally or vertically.
 * @param bishop_pins: Bitboard of pieces pinned diagonally.
 * @param moves: Precomputed moves struct pointer.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getEpMoves(uint64_t rook_pins, uint64_t bishop_pins, 
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
void Pos::horizontalPinEp(int king, bool turn, int attacker_sq, 
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

    uint64_t king_reach = Moves_::ROOK[Indices::ROOK[king][rookIndex(
            pieces ^ pawns, (Square) king)]].reach;
    uint64_t pin_ray = 0;

    // Ray between rook and king without pawns of interest.
    uint64_t rook_reach = 0;
    if (e_rook_sq != -1) {
        rook_reach = Moves_::ROOK[Indices::ROOK[e_rook_sq][rookIndex(
                pieces ^ pawns, (Square) e_rook_sq)]].reach;
        pin_ray |= (king_reach & rook_reach);
    }

    // Ray between queen and king without pawns of interest.
    uint64_t queen_reach = 0;
    if (e_queen_sq != -1) {
        queen_reach = Moves_::ROOK[Indices::ROOK[e_queen_sq][rookIndex(
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
void Pos::diagonalPinEp(int king, bool turn, int attacker_sq, 
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
 * Gets a bitboard of knight checkers.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the king in check.
 * @param checkers_only: A pointer to a bit board of checkers only.
 * @return: Bitboard of knight checkers.
 */
uint64_t Pos::getKnightCheckers(Computed* moves, Square square, uint64_t* checkers_only) {
    uint64_t result = 0;
    *checkers_only |= moves->KNIGHT_MOVES[square].reach & this->sides[1 - this->turn] & this->knights;
    return result;
}

/**
 * Checks if king of current player is in double-check.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @return: True if in double-check, else false.
 */
bool Pos::isDoubleChecked(Computed* moves) {
    bool turn = this->turn;
    Square king = this->piece_list[turn][0];
    uint64_t rook_attacks = this->getRookFamily(moves, king)->reach;
    uint64_t bishop_attacks = this->getBishopFamily(moves, king)->reach;

    uint64_t attackers = (rook_attacks & this->rooks & this->sides[!turn]) | 
            (bishop_attacks & this->bishops & this->sides[!turn]) |
            (rook_attacks & this->queens & this->sides[!turn]) |
            (bishop_attacks & this->queens & this->sides[!turn]);
    
    attackers |= moves->KNIGHT_MOVES[king].reach & this->knights & this->
            sides[!turn];

    uint64_t enemy_pawns = this->sides[!turn] & this->pawns;
    int rank_offset = turn ? 8 : -8;
    if (king % 8 != 0) {
        int pawn_sq = king + rank_offset - 1;
        attackers |= (1ULL << pawn_sq) & enemy_pawns;
    }

    if (king % 8 != 7) {
        int pawn_sq = king + rank_offset + 1;
        attackers |= (1ULL << pawn_sq) & enemy_pawns;
    }

    return (attackers & (attackers - 1)) != 0;
}

/**
 * Finds and returns a pointer to a rook move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* Pos::getRookFamily(Computed* moves, Square square) {
    return &Moves_::ROOK[Indices::ROOK[square][rookIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
}

/**
 * Finds and returns a pointer to a bishop move family.
 * 
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the bishop is on.
 */
MovesStruct* Pos::getBishopFamily(Computed* moves, Square square) {
    return &moves->BISHOP_MOVES[Indices::BISHOP[square][bishopIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
}

/**
 * Displays the bitboard.
 * @param game: A pointer to the game struct.
 * @param args: The command line arguments.
 */
void Pos::display(CmdLine* args) {
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
 * Computes and returns the hash of the current position.
 * @param game: Pointer to game struct.
 */
uint64_t getPosHash(Pos* game) {
    uint64_t hash = 0ULL;
    // if (game->ply == 0) { // Start of game, loop through all pieces.
    //     for (int sq = A1; sq <= H8; sq++) {
    //         if (game->pieces[sq] == W_KING) {
    //             hash ^= ZOBRIST_WKING[sq];
    //         } else if (game->pieces[sq] == W_QUEEN) {
    //             hash ^= ZOBRIST_WQUEEN[sq];
    //         } else if (game->pieces[sq] == W_ROOK) {
    //             hash ^= ZOBRIST_WROOK[sq];
    //         } else if (game->pieces[sq] == W_BISHOP) {
    //             hash ^= ZOBRIST_WBISHOP[sq];
    //         } else if (game->pieces[sq] == W_KNIGHT) {
    //             hash ^= ZOBRIST_WKNIGHT[sq];
    //         } else if (game->pieces[sq] == W_PAWN) {
    //             hash ^= ZOBRIST_WPAWN[sq];
    //         } else if (game->pieces[sq] == B_KING) {
    //             hash ^= ZOBRIST_BKING[sq];
    //         } else if (game->pieces[sq] == B_QUEEN) {
    //             hash ^= ZOBRIST_BQUEEN[sq];
    //         } else if (game->pieces[sq] == B_ROOK) {
    //             hash ^= ZOBRIST_BROOK[sq];
    //         } else if (game->pieces[sq] == B_BISHOP) {
    //             hash ^= ZOBRIST_BBISHOP[sq];
    //         } else if (game->pieces[sq] == B_KNIGHT) {
    //             hash ^= ZOBRIST_BKNIGHT[sq];
    //         } else if (game->pieces[sq] == B_PAWN) {
    //             hash ^= ZOBRIST_BPAWN[sq];
    //         }
    //     }

    //     hash ^= ZOBRIST_CASTLING[game->castling];
    //     // if (!game->turn) hash ^= ZOBRIST_BLACK;
    //     if (game->en_passant != NONE) {
    //         hash ^= ZOBRIST_EP[game->en_passant % 8];
    //     }
    // } else {

    // }

    return hash;
}

/**
 * Checks if a three fold repetition occured. If so, return true, else false.
 * Also performs the hash updates for the history.
 * @param game: Pointer to game struct.
 */
bool Pos::isThreeFoldRep() {
    std::unordered_map<uint64_t, int> counts;
    for (int i = this->ply - 1; i > this->ply - 15; i--) {
        if (i < 0) break;
        uint64_t hash = this->history[i].hash;
        // std::cout << counts[hash] << '\n';
        if (counts.count(hash) >= 3) return true;
    }

    return false;
}

/**
 * Checks if end of game has been reached.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves.
 * @param enemy_attacks: Bitboard of squares attacked by enemy pieces.
 * @param moves_index: Pointer to number of vectors in pos_moves.
 * @return: The appropriate ExitCode.
 */
ExitCode Pos::isEOG(Computed* moves, uint64_t enemy_attacks, int move_index) {
    // Draw by threefold repetition.
    if (this->isThreeFoldRep()) return THREE_FOLD_REPETITION;

    // Draw by fifty-move rule.
    if (this->halfmove == 100) return FIFTY_MOVES_RULE;

    // Draw by insufficient material.
    if (this->insufficientMaterial()) return INSUFFICIENT_MATERIAL;

    // Check for stalemate.
    if (move_index == 0 && !(this->isChecked(enemy_attacks))) return STALEMATE;

    // Check for checkmate.
    if (move_index == 0 && this->isChecked(enemy_attacks)) {
        if (this->turn) return BLACK_WINS;
        return WHITE_WINS;
    }

    return NORMAL_PLY;
}

/**
 * Retrives all legal moves of the current position.
 * 
 * @param game: Pointer to Pos struct of the current position.
 * @param moves: Pointer to the precomputed moves struct.
 * @param enemy_attacks: Pointer to bitboard of all squares attacked by the 
 *  enemy.
 * @param pos_moves: Array of vectors pointers of 16 bit unsigned int moves.
 * @return: The number of move sets.
 */
int Pos::getMoves(Computed* moves, uint64_t* enemy_attacks, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS]) {
    // The pinning rays
    uint64_t rook_pins = 0;
    uint64_t bishop_pins = 0;
    uint64_t kEnemy_attacks = 0;
    this->getEnemyAttacks(moves, enemy_attacks, &rook_pins, &bishop_pins, 
            &kEnemy_attacks);

    // Move retrieval for the 3 cases.
    int moves_index = 0;
    if (isDoubleChecked(moves)) {
        this->getKingMoves(moves, pos_moves, &moves_index, kEnemy_attacks);
    } else if (this->isChecked(*enemy_attacks)) {
        this->getCheckedMoves(moves, enemy_attacks, &rook_pins, &bishop_pins, 
                pos_moves, &moves_index, kEnemy_attacks);
    } else {
        this->getNormalMoves(moves, enemy_attacks, &rook_pins, &bishop_pins, 
                pos_moves, &moves_index, kEnemy_attacks);
    }
    return moves_index;
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
void Pos::getEnemyAttacks(Computed* moves, uint64_t* enemy_attacks, 
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
    uint64_t king_bishop_rays = moves->BISHOP_MOVES[Indices::BISHOP[
            king_sq][bishopIndex(pieces ^ (1ULL << king_sq), king_sq)]].reach;
    piece = turn ? B_BISHOP : W_BISHOP;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        *enemy_attacks |= moves->BISHOP_MOVES[Indices::BISHOP[square]
                [bishopIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        *kEnemy_attacks |= moves->BISHOP_MOVES[Indices::BISHOP[square][
                bishopIndex(masked_king_bb, (Square)square)]].reach;
        
        if (std::abs(square % 8 - king_sq % 8) == std::abs(((square / 8) % 
                8) - ((king_sq / 8) % 8))) {
            // Attacks.
            uint64_t attacks = moves->BISHOP_MOVES[Indices::BISHOP[square][
                    bishopIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            *bishop_pins |= king_bishop_rays & attacks;
        }
    }

    // Rook attacks.
    uint64_t king_rook_rays = Moves_::ROOK[Indices::ROOK[king_sq][
            rookIndex(pieces ^ (1ULL << king_sq), king_sq)]].reach;
    piece = turn ? B_ROOK : W_ROOK;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        *enemy_attacks |= Moves_::ROOK[Indices::ROOK[square]
                [rookIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        *kEnemy_attacks |= Moves_::ROOK[Indices::ROOK[square][
                rookIndex(masked_king_bb, (Square)square)]].reach;
        if (king_sq / 8 == square / 8 || king_sq % 8 == square % 8) {
            // Attacks.
            uint64_t attacks = Moves_::ROOK[Indices::ROOK[square][
                    rookIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            *rook_pins |= king_rook_rays & attacks;
        }
    }

    // Queen attacks.
    piece = turn ? B_QUEEN : W_QUEEN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        *enemy_attacks |= moves->BISHOP_MOVES[Indices::BISHOP[square]
                [bishopIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        *enemy_attacks |= Moves_::ROOK[Indices::ROOK[square]
                [rookIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        *kEnemy_attacks |= moves->BISHOP_MOVES[Indices::BISHOP[square][
                bishopIndex(masked_king_bb, (Square)square)]].reach;
        *kEnemy_attacks |= Moves_::ROOK[Indices::ROOK[square][
                rookIndex(masked_king_bb, (Square)square)]].reach;
        
        if (std::abs(square % 8 - king_sq % 8) == std::abs(((square / 8) % 
                8) - ((king_sq / 8) % 8))) {
            // Attacks.
            uint64_t attacks = moves->BISHOP_MOVES[Indices::BISHOP[square][
                    bishopIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            *bishop_pins |= king_bishop_rays & attacks;
        }

        if (king_sq / 8 == square / 8 || king_sq % 8 == square % 8) {
            // Attacks.
            uint64_t attacks = Moves_::ROOK[Indices::ROOK[square][
                    rookIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            *rook_pins |= king_rook_rays & attacks;
        }
    }

    // King attacks.
    *enemy_attacks |= moves->KING_MOVES[this->piece_list[!turn][0]].reach;
    *kEnemy_attacks |= *enemy_attacks;
}

/**
 * Find the piece in their associated piece list and removes it.
 * @param game: Pointer to game struct.
 * @param piece: The piece type to find and remove.
 * @param square: The square of the piece type to find and remove.
 */
void Pos::findAndRemovePiece(PieceType piece, Square square) {
    int taken_index = -1;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        if (this->piece_list[piece][i] == square) {
            taken_index = i;
            break;
        }
    }

    this->piece_index[piece]--;
    this->piece_list[piece][taken_index] = this->piece_list[piece][this->
            piece_index[piece]];
    if (piece == W_BISHOP) {
        if (this->isDark(square)) {
            this->wdsb_cnt--;
        } else {
            this->wlsb_cnt--;
        }
    } else if (piece == B_BISHOP) {
        if (this->isDark(square)) {
            this->bdsb_cnt--;
        } else {
            this->blsb_cnt--;
        }
    } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
        this->knight_cnt--;
    }
    this->piece_cnt--;
}

/**
 * Return true if a square is a dark square, else false.
 * @param square: The square to check.
 */
bool Pos::isDark(int square) {
    if ((square % 2 == 0 && ((square / 8) % 2) == 0) || 
            (square % 2 == 1 && ((square / 8) % 2) == 1)) {
        return true;
    }
    return false;
}

/**
 * Adds a piece to the end of the appropriate piece list.
 * @param game: Pointer to game struct.
 * @param piece: The piece type to add.
 * @param square: The square to add for piece.
 */
void Pos::addPiece(PieceType piece, Square square) {
    this->piece_list[piece][this->piece_index[piece]] = square;
    this->piece_index[piece]++;
    if (piece == W_BISHOP) {
        if (this->isDark(square)) {
            this->wdsb_cnt++;
        } else {
            this->wlsb_cnt++;
        }
    } else if (piece == B_BISHOP) {
        if (this->isDark(square)) {
            this->bdsb_cnt++;
        } else {
            this->blsb_cnt++;
        }
    } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
        this->knight_cnt++;
    }
    this->piece_cnt++;
}

/**
 * Process the removal of a piece just captured, if any.
 * @param game: Pointer to game struct.
 */
void Pos::removePiece() {
    PieceType captured = this->piece_captured;
    if (captured == NO_PIECE) return;
    int end = (this->last_move >> 6) & 0b111111;
    if (captured == W_QUEEN || captured == B_QUEEN) {
        this->queens &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_ROOK || captured == B_ROOK) {
        this->rooks &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_BISHOP || captured == B_BISHOP) {
        this->bishops &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_KNIGHT || captured == B_KNIGHT) {
        this->knights &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_PAWN || captured == B_PAWN) {
        this->pawns &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    }
}

/**
 * Process castling.
 * @param game: Pointer to game struct.
 */
void Pos::handleCastle() {
    int start, end;
    int last_end = (this->last_move >> 6) & 0b111111;
    PieceType rook;

    if (last_end  == G1) {
        start = H1;
        end = F1;
        rook = W_ROOK;
    } else if (last_end == C1) {
        start = A1;
        end = D1;
        rook = W_ROOK;
    } else if (last_end == G8) {
        start = H8;
        end = F8;
        rook = B_ROOK;
    } else {
        start = A8;
        end = D8;
        rook = B_ROOK;
    }

    this->sides[this->turn] &= ~(1ULL << start);
    this->sides[this->turn] |= 1ULL << end;
    this->pieces[start] = NO_PIECE;
    this->pieces[end] = rook;

    this->findAndRemovePiece(rook, (Square) start);
    this->addPiece(rook, (Square) end);
    this->rooks &= ~(1ULL << start);
    this->rooks |= 1ULL << end;
}

/**
 * Make the king move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeKingMoves(uint16_t move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    this->piece_list[this->turn][0] = (Square) end;

    // Remove castling rights
    if (this->turn) {
        this->castling &= ~(1 << WKSC | 1 << WQSC);
    } else {
        this->castling &= ~(1 << BKSC | 1 << BQSC);
    }

    if (move_type == CASTLING) {
        this->handleCastle();
    }

    // Update bitboards.
    this->kings &= ~(1ULL << start);
    this->kings |= 1ULL << end;
}

/**
 * Make the queen move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeQueenMoves(uint16_t move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
    this->findAndRemovePiece(queen, (Square) start);
    this->addPiece(queen, (Square) end);
    this->queens &= ~(1ULL << start);
    this->queens |= 1ULL << end;
}

/**
 * Make the rook move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeRookMoves(uint16_t move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType rook = this->turn ? W_ROOK : B_ROOK;
    this->findAndRemovePiece(rook, (Square) start);
    this->addPiece(rook, (Square) end);
    this->rooks &= ~(1ULL << start);
    this->rooks |= 1ULL << end;
}

/**
 * Make the bishop move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeBishopMoves(uint16_t move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
    this->findAndRemovePiece(bishop, (Square) start);
    this->addPiece(bishop, (Square) end);
    this->bishops &= ~(1ULL << start);
    this->bishops |= 1ULL << end;
}

/**
 * Make the knight move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeKnightMoves(uint16_t move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
    this->findAndRemovePiece(knight, (Square) start);
    this->addPiece(knight, (Square) end);
    this->knights &= ~(1ULL << start);
    this->knights |= 1ULL << end;
}

/**
 * Prints a readable version of a move.
 * @param move: The move to be printed.
 */
void Pos::printMove(uint16_t move, bool extraInfo) {
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
 * Make the pawn move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makePawnMoves(uint16_t move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    int promo = move & (0b11 << 14);
    bool pawn = false;
    int ep = end + (this->turn ? -8 : 8);
    PieceType piece = this->turn ? W_PAWN : B_PAWN;
    this->findAndRemovePiece(piece, (Square) start);

    if (move_type == PROMOTION) {
        if (this->turn) {
            piece = promo == pQUEEN ? W_QUEEN : promo == pROOK ? 
                W_ROOK : promo == pBISHOP ? W_BISHOP : W_KNIGHT;
        } else {
            piece = promo == pQUEEN ? B_QUEEN : promo == pROOK ? 
                B_ROOK : promo == pBISHOP ? B_BISHOP : B_KNIGHT;
        }

        if (promo == pQUEEN) {
            this->queens |= 1ULL << end;
            PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
            this->addPiece(queen, (Square) end);
        } else if (promo == pROOK) {
            this->rooks |= 1ULL << end;
            PieceType ROOK = this->turn ? W_ROOK : B_ROOK;
            this->addPiece(ROOK, (Square) end);
        } else if (promo == pBISHOP) {
            this->bishops |= 1ULL << end;
            PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
            this->addPiece(bishop, (Square) end);
        } else {
            this->knights |= 1ULL << end;
            PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
            this->addPiece(knight, (Square) end);
        }
        this->pieces[end] = piece;
    } else if (move_type == EN_PASSANT) {
        pawn = true;
        this->addPiece(piece, (Square) end);
        piece = this->turn ? B_PAWN : W_PAWN;
        this->findAndRemovePiece(piece, (Square) ep);
        this->pawns &= ~(1ULL << ep);
        this->sides[1 - this->turn] &= ~(1ULL << ep);
        this->pieces[ep] = NO_PIECE;
    } else {
        pawn = true;
        this->addPiece(piece, (Square) end);
        int rank_diff = end / 8 - start / 8;
        if (std::abs(rank_diff) != 1) {
            this->en_passant = (Square) (start + 8 * (rank_diff - (rank_diff > 0 ? 1 : -1)));
        }
    }

    this->pawns &= ~(1ULL << start);
    this->pawns |= (uint64_t)pawn << end;
}

/**
 * Save game state to history for undoing moves.
 * @param game: Pointer to game struct.
 * @param move: The move to made.
 */
void Pos::saveHistory(uint16_t move) {
    this->history[this->ply].castling = this->castling;
    this->history[this->ply].en_passant = this->en_passant;
    this->history[this->ply].halfmove = this->halfmove;
    this->history[this->ply].move = move;
    this->history[this->ply].captured = this->pieces[(move >> 6) & 0b111111];
    this->ply++;
}

/**
 * Undo normal moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoNormal() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];
    
    // Retrieve last move information
    uint16_t move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];
    PieceType captured = previous_pos.captured;
    
    // Change turn
    this->turn = 1 - this->turn;
    bool turn = this->turn;

    // Change sides bitboards
    this->sides[turn] &= ~(1ULL << end);
    this->sides[turn] |= 1ULL << start;
    if (captured != NO_PIECE) this->sides[!turn] |= 1ULL << end;

    // Change pieces bitboards, piece list and indices and piece counts
    if (moved == B_PAWN || moved == W_PAWN) {
        this->pawns &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->pawns |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_KNIGHT || moved == W_KNIGHT) {
        this->knights &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->knights |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_BISHOP || moved == W_BISHOP) {
        this->bishops &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->bishops |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_ROOK || moved == W_ROOK) {
        this->rooks &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->rooks |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_QUEEN || moved == W_QUEEN) {
        this->queens &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->queens |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else {
        this->kings &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->kings |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    }

    if (captured == NO_PIECE) {
        /* Do nothing */
    } else if (captured == B_PAWN || captured == W_PAWN) {
        this->pawns |= 1ULL << end;
        this->addPiece(captured, (Square) end);
        this->piece_cnt++;
    } else if (captured == B_KNIGHT || captured == W_KNIGHT) {
        this->knights |= 1ULL << end;
        this->addPiece(captured, (Square) end);
        this->piece_cnt++;
        this->knight_cnt++;
    } else if (captured == B_BISHOP || captured == W_BISHOP) {
        this->bishops |= 1ULL << end;
        addPiece(captured, (Square) end);
        this->piece_cnt++;
        if (this->isDark(end)) {
            turn ? this->bdsb_cnt++ : this->wdsb_cnt++;
        } else {
            turn ? this->blsb_cnt++ : this->wlsb_cnt++;
        }
    } else if (captured == B_ROOK || captured == W_ROOK) {
        this->rooks |= 1ULL << end;
        this->addPiece(captured, (Square) end);
        this->piece_cnt++;
    } else {
        this->queens |= 1ULL << end;
        this->addPiece(captured, (Square) end);
        this->piece_cnt++;
    }

    // Undo fullmove and History struct information
    if (!turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = captured;
}

/**
 * Undo promotion moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoPromotion() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = 1 - this->turn;
    bool turn = this->turn;

    // Retrieve last move information
    uint16_t move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType promoted = this->pieces[end];
    PieceType captured = previous_pos.captured;
    PieceType pawn = turn ? W_PAWN : B_PAWN;

    // Remove promoted piece
    this->findAndRemovePiece(promoted, (Square) end);
    this->sides[turn] &= ~(1ULL << end);
    this->pieces[end] = NO_PIECE;
    if (promoted == W_QUEEN || promoted == B_QUEEN) {
        this->queens &= ~(1ULL << end);
    } else if (promoted == W_ROOK || promoted == B_ROOK) {
        this->rooks &= ~(1ULL << end);
    } else if (promoted == W_BISHOP || promoted == B_BISHOP) {
        this->bishops &= ~(1ULL << end);
    } else { // Knight
        this->knights &= ~(1ULL << end);
    }

    // Replace captured piece (if any)
    if (captured != NO_PIECE) {
        this->addPiece(captured, (Square) end);
        this->sides[!turn] |= 1ULL << end;
        this->pieces[end] = captured;
        if (captured == W_QUEEN || captured == B_QUEEN) {
            this->queens |= 1ULL << end;
        } else if (captured == W_ROOK || captured == B_ROOK) {
            this->rooks |= 1ULL << end;
        } else if (captured == W_BISHOP || captured == B_BISHOP) {
            this->bishops |= 1ULL << end;
        } else { // Knight
            this->knights |= 1ULL << end;
        }
    }

    // Replace pawn
    this->addPiece(pawn, (Square) start);
    this->sides[turn] |= 1ULL << start;
    this->pawns |= 1ULL << start;
    this->pieces[start] = pawn;

    // Undo fullmove and History struct information
    if (!turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;
}

/**
 * Undo en-passant moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoEnPassant() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = 1 - this->turn;
    bool turn = this->turn;

    // Retrieve last move information
    uint16_t move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];
    PieceType captured = turn ? B_PAWN : W_PAWN;
    int captured_sq = end + (turn ? -8 : 8);

    // Change sides bitboards
    this->sides[turn] &= ~(1ULL << end);
    this->sides[turn] |= 1ULL << start;
    this->sides[!turn] |= 1ULL << captured_sq;

    // Change pawn bitboards
    this->pawns &= ~(1ULL << end);
    this->pawns |= 1ULL << start;
    this->pawns |= 1ULL << captured_sq;

    // Undo fullmove and History struct information
    if (!turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update piece list, indices and counts
    this->findAndRemovePiece(moved, (Square) end);
    this->addPiece(moved, (Square) start);
    this->addPiece(captured, (Square) captured_sq);
    this->piece_cnt++;

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = NO_PIECE;
    this->pieces[captured_sq] = captured;
}

/**
 * Undo castling moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoCastling() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = 1 - this->turn;
    bool turn = this->turn;

    // Retrieve last move information
    uint16_t move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];

    // Rook changes
    int rook_start, rook_end;
    PieceType rook = turn ? W_ROOK : B_ROOK;
    if (end == G1) {
        rook_start = H1;
        rook_end = F1;
    } else if (end == C1) {
        rook_start = A1;
        rook_end = D1;
    } else if (end == G8) {
        rook_start = H8;
        rook_end = F8;
    } else {
        rook_start = A8;
        rook_end = D8;
    }

    // Change sides bitboards
    this->sides[turn] &= ~(1ULL << end);
    this->sides[turn] |= 1ULL << start;
    this->sides[turn] &= ~(1ULL << rook_end);
    this->sides[turn] |= 1ULL << rook_start;

    // Change king bitboards
    this->kings &= ~(1ULL << end);
    this->kings |= 1ULL << start;

    // Change rook bitboards
    this->rooks &= ~(1ULL << rook_end);
    this->rooks |= 1ULL << rook_start;

    // Undo fullmove and History struct information (some is not necessary i think? Done out of principle)
    if (!turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update piece list, indices and counts
    this->findAndRemovePiece(moved, (Square) end);
    this->addPiece(moved, (Square) start);
    this->findAndRemovePiece(rook, (Square) rook_end);
    this->addPiece(rook, (Square) rook_start);

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = NO_PIECE;
    this->pieces[rook_start] = rook;
    this->pieces[rook_end] = NO_PIECE;
}

/**
 * Undo moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoMove() {
    if (this->ply == 0) {
        std::cout << "Nothing to undo...\n";
        return;
    }

    int type = this->history[this->ply - 1].move & (0b11 << 12);
    if (type == NORMAL) {
        this->undoNormal();
    } else if (type == PROMOTION) {
        this->undoPromotion();
    } else if (type == EN_PASSANT) {
        this->undoEnPassant();
    } else {
        this->undoCastling();
    }
}

/**
 * Make the moves.
 * @param move: The moves to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeMove(uint16_t move) {
    if (move == 0) {
        this->undoMove();
        return;
    }

    // Save current position and move to make to history.
    this->saveHistory(move);

    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    
    this->last_move = move;
    this->last_move_type = (MoveType) move_type;
    this->piece_captured = this->pieces[end];
    this->piece_moved = this->pieces[start];
    this->en_passant = NONE;
    this->sides[this->turn] &= ~(1ULL << start);
    this->sides[this->turn] |= 1ULL << end;
    this->sides[1 - this->turn] &= ~(1ULL << end);
    this->pieces[start] = NO_PIECE;
    this->pieces[end] = this->piece_moved;

    // Change castling privileges.
    if (start == H8 || end == H8) this->castling &= ~(1 << BKSC);
    if (start == A8 || end == A8) this->castling &= ~(1 << BQSC);
    if (start == A1 || end == A1) this->castling &= ~(1 << WQSC);
    if (start == H1 || end == H1) this->castling &= ~(1 << WKSC);

    PieceType moved = this->piece_moved;
    if (moved == W_KING || moved == B_KING) {
        this->makeKingMoves(move);
    } else if (moved == W_QUEEN || moved == B_QUEEN) {
        this->makeQueenMoves(move);
    } else if (moved == W_ROOK || moved == B_ROOK) {
        this->makeRookMoves(move);
    } else if (moved == W_BISHOP || moved == B_BISHOP) {
        this->makeBishopMoves(move);
    } else if (moved == W_KNIGHT || moved == B_KNIGHT) {
        this->makeKnightMoves(move);
    } else {
        this->makePawnMoves(move);
    }
    
    if (this->turn == BLACK) this->fullmove++;
    if (moved == W_PAWN || moved == B_PAWN || 
            this->piece_captured != NO_PIECE) {
        this->halfmove = 0;
    } else {
        this->halfmove++;
    }
    this->turn = 1 - this->turn;
}

/**
 * Count and print the moves of the current position.
 * @param game: Pointer to game struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to int of number of move vectors in pos_moves.
 * @return: The number of moves in the position.
 */
int countMoves(Pos* game, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], 
        int* moves_index) {
    int count = 0;
    for (int i = 0; i < *moves_index; i++) {
        std::vector<uint16_t>* pointer = pos_moves[i];
        for (uint16_t move : *pointer) {
            std::cout << squareName[move & 0b111111] << " " << squareName[(
                    move >> 6) & 0b111111] << " " << moveName[(move >> 12) & 
                    0b11] << " " << promoName[(move >> 14) & 0b11] << '\n';
            count++;
        }
    }
    return count;
}

/**
 * Checks if a given move is valid.
 * @param move: The move to check.
 * @param game: Pointer to game move struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Int pointer to number of vectors in pos_moves.
 * @return: True if move is valid, else false.
 */
bool Pos::validMove(uint16_t move, std::vector<uint16_t>* pos_moves[
        MAX_MOVE_SETS], int* moves_index) {
    for (int i = 0; i < *moves_index; i++) {
        for (uint16_t move_candidate : *pos_moves[i]) {
            if (move == move_candidate) return true;
        }
    }
    return false;
}

/**
 * Calls getFEN to get the FEN string of the current position and prints out.
 */
void Pos::fen() {
    std::cout << this->getFEN() << '\n';
}

/**
 * Get and return the FEN string of the current position.
 * @param game: Pointer to game struct.
 */
std::string Pos::getFEN() {
    std::string fen = "";
    int no_piece_count = 0;
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file <= 7; file++) {
            int sq = rank * 8 + file;
            
            if (sq % 8 == 0 && sq != A8) {
                if (no_piece_count != 0) {
                    fen += no_piece_count + '0';
                    no_piece_count = 0;
                }
                fen += "/";
            }
            
            PieceType piece = this->pieces[sq];
            if (piece == NO_PIECE) {
                no_piece_count++;
            } else {
                if (no_piece_count != 0) {
                    fen += no_piece_count + '0';
                    no_piece_count = 0;
                }
                if (piece == B_KING) fen += 'k';
                else if (piece == B_QUEEN) fen += 'q';
                else if (piece == B_ROOK) fen += 'r';
                else if (piece == B_BISHOP) fen += 'b';
                else if (piece == B_KNIGHT) fen += 'n';
                else if (piece == B_PAWN) fen += 'p';
                else if (piece == W_KING) fen += 'K';
                else if (piece == W_QUEEN) fen += 'Q';
                else if (piece == W_ROOK) fen += 'R';
                else if (piece == W_BISHOP) fen += 'B';
                else if (piece == W_KNIGHT) fen += 'N';
                else fen += 'P';
            }
        }
    }
    if (no_piece_count != 0) {
        fen += no_piece_count + '0';
        no_piece_count = 0;
    }

    if (this->turn) {
        fen += " w ";
    } else {
        fen += " b ";
    }

    if (this->castling == 0) {
        fen += "-";
    } else {
        if (this->castling & (1 << WKSC)) fen += "K";
        if (this->castling & (1 << WQSC)) fen += "Q";
        if (this->castling & (1 << BKSC)) fen += "k";
        if (this->castling & (1 << BQSC)) fen += "q";
    }
    fen += " ";

    if (this->en_passant == NONE) {
        fen += "- ";
    } else {
        fen += squareName[this->en_passant] + " ";
    }

    fen += this->halfmove + '0';
    fen += " ";
    fen += this->fullmove + '0';

    return fen;
}

/**
 * Gets the start and end squares of a move. Also sets the promotion moves.
 * @param move_string: The move string.
 * @param move: Unsigned 16-bit int representing the move.
 * @param start: Pointer to unsigned int for start square.
 * @param end: Pointer to unsigned int for end square.
 */
void Pos::getSquares(std::string move_string, uint16_t* move, uint* start, uint* end) {
    int start_file, start_rank, end_file, end_rank;
            start_file = move_string[0] - 'a';
            start_rank = move_string[1] - '1';
            end_file = move_string[2] - 'a';
            end_rank = move_string[3] - '1';
            if (move_string.length() == 5) {
                *move |= PROMOTION;
                if (move_string[4] == 'q') {
                    *move |= pQUEEN;
                } else if (move_string[4] == 'r') {
                    *move |= pROOK;
                } else if (move_string[4] == 'b') {
                    *move |= pBISHOP;
                }
            }

            *start = 8 * start_rank + start_file;
            *end = 8 * end_rank + end_file;
}

/**
 * Checks and sets castling and en passant moves, if any.
 * @param game: Pointer to the game struct.
 * @param start: The start square.
 * @param end: The end square.
 * @param move: Pointer to the integer representing the move.
 */
void Pos::checkCastlingEnPassantMoves(uint start, uint end, uint16_t* move) {
    // Check for castling move.
    if (this->turn && this->piece_list[WHITE][0] == E1) {
        if (start == E1 && end == G1 && (this->castling & (1 << 
                WKSC))) {
            *move |= CASTLING;
        } else if (start == E1 && end == C1 && (this->castling & (1 << 
                WQSC))) {
            *move |= CASTLING;
        }
    } else if (!this->turn && this->piece_list[BLACK][0] == E8) {
        if (start == E8 && end == G8 && (this->castling & (1 << 
                BKSC))) {
            *move |= CASTLING;
        } else if (start == E8 && end == C8 && (this->castling & (1 << 
                BQSC))) {
            *move |= CASTLING;
        }
    }

    // Check for en-passant move.
    if ((this->turn && start / 8 == 4) || (!this->turn && start / 8 == 
            3)) {
        PieceType piece = this->pieces[start];
        if ((piece == W_PAWN || piece == B_PAWN) &&
                end == this->en_passant) {
            *move |= EN_PASSANT;
        }
    }
}

/**
 * Process human move choice or generate random move choice for computer.
 * @param game: Pointer to game move struct.
 * @param white: The type of player for white.
 * @param black: The type of player for black.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Int pointer to number of vectors in pos_moves.
 * @return: The chosen move.
 */
uint16_t Pos::chooseMove(int white, int black, std::vector<uint16_t>* 
        pos_moves[MAX_MOVE_SETS], int* moves_index, Computed* moves) {
    uint16_t move = NORMAL | pKNIGHT;
    
    // Recieve and print move.
    if ((this->turn && white == HUMAN) || (!this->turn && black == HUMAN)) {
        std::cout << "Enter move: ";

        while (true) {
            move = NORMAL | pKNIGHT;
            std::string move_string;
            std::cin >> move_string;

            if (move_string == "kill") exit(-1);
            if (move_string == "fen") this->fen();
            if (move_string == "undo") return 0;

            uint start, end;
            this->getSquares(move_string, &move, &start, &end);
            this->checkCastlingEnPassantMoves(start, end, &move);

            move |= start;
            move |= (end << 6);
            if (!this->validMove(move, pos_moves, moves_index)) {
                std::cout << "Invalid move, enter again: ";
            } else break;
        }
         
    } else {
        uint64_t enemy_attacks = 0;
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS];
        int moves_index = this->getMoves(moves, &enemy_attacks, pos_moves);
        double value = this->turn ? -1000000 : 1000000;
        std::cout << "Thinking..." << std::flush;
        for (int i = 0; i < moves_index; i++) {
            std::vector<uint16_t>* move_set = pos_moves[i];
            for (uint16_t move_candidate : *move_set) {
                this->makeMove(move_candidate);
                double pos_value = this->alphaBeta(4, -100000, 100000, this->turn, moves);
                if (pos_value > value && !this->turn) {
                    move = move_candidate;
                    value = pos_value;
                } else if (pos_value < value && this->turn) {
                    move = move_candidate;
                    value = pos_value;
                }
                this->undoMove();
            }
        }
        std::cout << "\rComputer move: ";
        this->printMove(move, true);
    }
    
    return move;
}

/**
 * Handles a single game.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param args: Pointer to command line arguments struct.
 */
void Pos::handleGame(Computed* moves, CmdLine* args, char *argv[]) {
    ExitCode code = NORMAL_PLY;
    
    uint64_t enemy_attacks = 0;
    std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS];
    int moves_index = this->getMoves(moves, &enemy_attacks, pos_moves);
    
    while (!code) {
        if (!args->quiet) this->display(args);

        code = this->isEOG(moves, enemy_attacks, moves_index);
        if (code) break;
        uint16_t move = this->chooseMove(args->white, args->black, pos_moves, 
                &moves_index, moves);
        this->makeMove(move);

        enemy_attacks = 0;
        moves_index = this->getMoves(moves, &enemy_attacks, pos_moves);
    }

    this->showEOG(code, argv);
}

/**
 * The main loop that constantly reads from stdin for commands. Calls other
 * functions that create threads which handle different parts of program.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param args: Pointer to command line arguments.
 * @param argv: Command line arguments.
 * @param input: Command given by the user.
 */
void runNormal(Pos* game, CmdLine* args, char *argv[], std::string input) {
    Computed moves;
    Compute::init(&moves);
    goto start;
    while (std::getline(std::cin, input)) {
        start:
        std::vector<std::string> commands = split(input, " ");
        if (commands[0] == "perft") perft(commands.size() == 2 ? std::stoi(commands[1]) : 0, game);
        if (commands[0] == "play") game->handleGame(&moves, args, argv);
        if (commands[0] == "fen") game->fen();
        if (commands[0] == "exit") break;

        // Reset to the position.
        game->parseFen(game->original_fen);
    }
}

void Play::init(Pos* game, CmdLine* args, char *argv[], std::string input) {
    runNormal(game, args, argv, input);
}
