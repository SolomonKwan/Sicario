
#include <iostream>
#include <unordered_map>

#include "fen.hpp"
#include "game.hpp"
#include "test.hpp"
#include "search.hpp"

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
void runNormal(Pos* game, Computed* moves, CmdLine* args, char *argv[], 
        std::string input) {
    goto start;
    while (std::getline(std::cin, input)) {
        start:
        std::vector<std::string> commands = split(input, " ");
        if (commands[0] == "perft") perft(commands.size() == 2 ? std::stoi(commands[1]) : 0, game);
        if (commands[0] == "play") game->handleGame(moves, args, argv);
        if (commands[0] == "fen") game->fen();
        if (commands[0] == "exit") break;

        // Reset to the position.
        game->parseFen(game->original_fen);
    }
}

void Play::init(Pos* game, Computed* moves, CmdLine* args, char *argv[], 
        std::string input) {
    runNormal(game, moves, args, argv, input);
}
