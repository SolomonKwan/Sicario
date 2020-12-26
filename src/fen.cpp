
#include <iostream>
#include "fen.hpp"

/**
 * Splits a string by a delimeter string and returns vector of results.
 * 
 * @param input: String to split.
 * @param delim: String delimiter.
 * 
 * @return: Vector of input split by delim.
 */
std::vector<std::string> split(std::string input, std::string delim) {
    std::vector<std::string> result;
    std::size_t pos;

    while ((pos = input.find(delim)) != std::string::npos) {
        result.push_back(input.substr(0, pos));
        input = input.substr(pos + delim.length(), input.length() - 1);
    }
    result.push_back(input.substr(0, pos));

    return result;
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
