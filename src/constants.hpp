
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <random>

#define UNSET 0xFFFFFFFFFFFFFFFF
#define BLACK 0
#define WHITE 1
#define STANDARD_GAME "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define MAX_MOVES 2048
#define MAX_PIECE_MOVES 27
#define MAX_MOVE_SETS 100
#define MAX_THREADS 4

#define VERSION "Tardigrade (Alpha 3.0.0)"
#define AUTHOR "S. Kwan"
#define ACKNOWLEDGMENTS "Acknowledgments:"
#define CHESS_PROGRAMMING "www.chessprogramming.org"
#define STOCKFISH "https://github.com/official-stockfish/Stockfish"
#define BLUE_FEVER_SOFT "https://github.com/bluefeversoft"

typedef uint64_t U64;
typedef uint16_t U16;
typedef U16 MOVE;
typedef std::vector<MOVE> MOVES;

/**
 * The squares of the board and their associated numbers. NONE (64) used as
 * sentinel value.
 */
enum Square {
	A1, B1, C1, D1, E1, F1, G1, H1,
  	A2, B2, C2, D2, E2, F2, G2, H2,
  	A3, B3, C3, D3, E3, F3, G3, H3,
  	A4, B4, C4, D4, E4, F4, G4, H4,
  	A5, B5, C5, D5, E5, F5, G5, H5,
 	A6, B6, C6, D6, E6, F6, G6, H6,
  	A7, B7, C7, D7, E7, F7, G7, H7,
  	A8, B8, C8, D8, E8, F8, G8, H8,
	
	NONE
};

/**
 * The piece types.
 */
enum PieceType {
    B_KING, W_KING, 
    
    W_QUEEN, W_ROOK, W_BISHOP, W_KNIGHT, W_PAWN,
    B_QUEEN, B_ROOK, B_BISHOP, B_KNIGHT, B_PAWN,

    NO_PIECE
};

/**
 * The promotion piece types. In the format for move hashes (i.e. left shifted
 * by 14 bits).
 */
enum Promotion {
    pKNIGHT = 0,
    pBISHOP = 1 << 14,
    pROOK = 2 << 14,
    pQUEEN = 3 << 14
};

/**
 * The move types. In the format for move hashes (i.e. left shifted by 12 bits).
 */
enum MoveType {
	NORMAL = 0,
	PROMOTION = 1 << 12,
	EN_PASSANT = 2 << 12,
	CASTLING = 3 << 12
};

enum ExitCode {
    NORMAL_PLY,
    WHITE_WINS,
    BLACK_WINS,
    STALEMATE,
    THREE_FOLD_REPETITION,
    FIFTY_MOVES_RULE,
    INSUFFICIENT_MATERIAL,
    DRAW_BY_AGREEMENT,
    INVALID_FEN,
    INVALID_ARGS
};

enum Player {
    HUMAN, COMPUTER
};

enum Castling {
    WKSC, WQSC, BKSC, BQSC
};

/**
 * A struct holding the move families. 
 * 
 * Contains:
 *  reach:      A bit board of the squares a piece can reach. Enemy and friendly
 *              pieces included. Goes all the way to the edge of the board.
 *  block_bits: A vector of the end squares on each ray of the reach bitboard.
 *  move_set:   A vector of vectors of 16 bit unsigned integers. Each integer
 *              encodes a move as Promotion (4) | MoveType (4) | Destination (6)
 *              | Origin (6).
 *  en_passant: Vector of vectors of en-passant moves. Used only for pawns.
 *  checked_moves: Unnorderd map whose keys are uint64_t ints with the possible
 *      destination squares (captures and blocks) set. The value is a vector of
 *      moves to those squares.
 */
struct MovesStruct {
    uint64_t reach;
    std::vector<int> block_bits;
    std::vector<std::vector<uint16_t>> move_set;
    std::vector<std::vector<uint16_t>> en_passant;
    std::vector<std::vector<uint16_t>> double_push;

    std::unordered_map<uint64_t, std::vector<uint16_t>> checked_moves;
};

/**
 * Information before current move is made.
 */
struct History {
    int castling;
    Square en_passant;
    int halfmove;
    uint16_t move;
    PieceType captured;
    uint64_t hash;
};

struct Computed {
    std::vector<int> ROOK_INDEX[64];
    MovesStruct ROOK_MOVES[4900];
    MovesStruct ROOK_BLOCKS[64];

    std::vector<int> BISHOP_INDEX[64];
    MovesStruct BISHOP_MOVES[1428];
    MovesStruct BISHOP_BLOCKS[64];

    MovesStruct KNIGHT_MOVES[64];
    MovesStruct KING_MOVES[64];
    MovesStruct PAWN_MOVES[2][48];
    MovesStruct CASTLING_MOVES[4];
    MovesStruct EN_PASSANT_MOVES[16];
    MovesStruct DOUBLE_PUSH[16];
};

/**
 * Some of the command line arguments.
 */
struct CmdLine {
    Player white;
    Player black;
    std::string fen = STANDARD_GAME;
    bool dark_mode, quiet;
};

/**
 * A struct representing the current board position.
 */
class Pos {
    public:
        std::string original_fen;
        int halfmove, fullmove;
        int piece_index[12];

        void displayAll(Pos* game);
        ExitCode parseFen(std::string fen);
        void zero();
        void handleGame(Computed* moves, CmdLine* args, char *argv[]);
        int getMoves(Computed* moves, uint64_t* enemy_attacks, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS]);
        void getEnemyAttacks(Computed* moves, uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, uint64_t* kEnemy_attacks);
        const int bishopIndex(const uint64_t pos, Square square);
        const int rookIndex(const uint64_t pos, Square square);
        bool isDoubleChecked(Computed* moves);
        MovesStruct* getRookFamily(Computed* moves, Square square);
        MovesStruct* getBishopFamily(Computed* moves, Square square);
        void display(CmdLine* args);
        ExitCode isEOG(Computed* moves, uint64_t enemy_attacks, int move_index);
        bool isThreeFoldRep();
        bool insufficientMaterial();
        bool isChecked(uint64_t enemy_attacks);
        void getKingMoves(Computed* computed_moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        void getCheckedMoves(Computed* moves, uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        uint64_t getBishopCheckRays(Computed* moves, Square square, uint64_t* checkers_only);
        uint64_t getRookCheckRays(Computed* moves, Square square, uint64_t* checkers);
        uint64_t getPawnCheckers(Square square, uint64_t* checkers_only);
        uint64_t getKnightCheckers(Computed* moves, Square square, uint64_t* checkers_only);
        void getCheckedEp(uint64_t* rook_pins, uint64_t* bishop_pins, uint64_t checkers, Computed* moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getNormalMoves(Computed* moves, uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, uint64_t kEnemy_attacks);
        void getQueenMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getRookPinMoves(Computed* moves, int square, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getBishopPinMoves (Computed* moves, int square, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getRookMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getBishopMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getKnightMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getPawnMoves(Computed* computed_moves, uint64_t rook_pins, uint64_t bishop_pins, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        uint64_t pawnMoveArgs(Square square);
        void getCastlingMoves(uint64_t enemy_attacks, Computed* moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void getEpMoves(uint64_t rook_pins, uint64_t bishop_pins, Computed* moves, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void horizontalPinEp(int king, bool turn, int attacker_sq, int captured_pawn, Computed* moves, int ep, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void diagonalPinEp(int king, bool turn, int attacker_sq, int captured_pawn, Computed* moves, int ep, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        uint16_t chooseMove(int white, int black, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, Computed* moves);
        void fen();
        std::string getFEN();
        void getSquares(std::string move_string, uint16_t* move, uint* start, uint* end);
        void checkCastlingEnPassantMoves(uint start, uint end, uint16_t* move);
        bool validMove(uint16_t move, std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index);
        void makeMove(uint16_t move);
        void undoMove();
        void undoNormal();
        void findAndRemovePiece(PieceType piece, Square square);
        bool isDark(int square);
        void undoPromotion();
        void addPiece(PieceType piece, Square square);
        void undoEnPassant();
        void undoCastling();
        void saveHistory(uint16_t move);
        void makeKingMoves(uint16_t move);
        void removePiece();
        void handleCastle();
        void makeQueenMoves(uint16_t move);
        void makeRookMoves(uint16_t move);
        void makeBishopMoves(uint16_t move);
        void makeKnightMoves(uint16_t move);
        void makePawnMoves(uint16_t move);
        double alphaBeta(int depth, double alpha, double beta, bool max, Computed* moves);
        double evaluate();
        void printMove(uint16_t move, bool extraInfo);
        void showEOG(ExitCode code, char *argv[]);
        const int rookBlockIndex(uint64_t pos, Computed* moves, Square square);

        static void computeRookBlocks(MovesStruct* ROOK_BLOCKS);
        static void computeBishopBlocks(MovesStruct* BISHOP_BLOCKS);
        static void computeCastling(MovesStruct* CASTLING_MOVES);

        static void computeKnightMoves(MovesStruct* KNIGHT_MOVES);
        static void computeKingMoves(MovesStruct* KING_MOVES);
        static void computePawnMoves(MovesStruct PAWN_MOVES[][48], MovesStruct EN_PASSANT_MOVES[16], MovesStruct DOUBLE_PUSH[16]);

    private:
        uint64_t sides[2];
        uint64_t kings, queens, rooks, bishops, knights, pawns;
        bool turn;
        int castling;
        Square en_passant;

        // The piece positions.
        Square piece_list[12][10];
        PieceType pieces[64];

        // Piece counts for insufficient material checks.
        int piece_cnt = 0;
        int knight_cnt = 0;
        int wdsb_cnt = 0, wlsb_cnt = 0;
        int bdsb_cnt = 0, blsb_cnt = 0;

        uint16_t last_move;
        PieceType piece_moved;
        PieceType piece_captured;
        MoveType last_move_type;

        // Pos history
        History history[MAX_MOVES];
        int ply;
        uint64_t hash;
};

/**
 * Bitboard of files with bits set.
 */
const uint64_t files[64] = {
    72340172838076673ULL, 144680345676153346ULL, 289360691352306692ULL,
    578721382704613384ULL, 1157442765409226768ULL, 2314885530818453536ULL,
    4629771061636907072ULL, 9259542123273814144ULL
};

/**
 * Bitboards of ranks with bits set.
 */
const uint64_t ranks[64] = {
    255ULL, 65280ULL, 16711680ULL, 4278190080ULL, 1095216660480ULL, 
    280375465082880ULL, 71776119061217280ULL, 18374686479671623680ULL
};

/**
 * The squares in string format. Used for building and debugging. 
 */
const std::string squareName[65] = {
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
 	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",

	"None"
};

/**
 * Piece type strings.
 */
const std::string piece_type_string[13] = {
    "B_KING", "W_KING", 
    
    "W_QUEEN", "W_ROOK", "W_BISHOP", "W_KNIGHT", "W_PAWN",
    "B_QUEEN", "B_ROOK", "B_BISHOP", "B_KNIGHT", "B_PAWN",

    "NO_PIECE"
};

/**
 * Promotion piece strings.
 */
const std::string promoName[4] = {
	"n",
    "b",
    "r",
    "q"
};

/**
 * Move type strings.
 */
const std::string moveName[4] = {
	"Normal",
    "Promotion",
    "En-passant",
    "Castling"
};

/**
 * Magic numbers for vertical and horizontal rook attacks.
 */
const uint64_t rookMagicNums[64] = {
	0x80001020400080ULL, 0x40004020001000ULL, 0x80300008802000ULL, 
  	0x80100080040800ULL, 0x80040102800800ULL, 0x80012400800200ULL, 
  	0x806A0000800900ULL, 0x80002040800100ULL, 0x0A002081410200ULL, 
	0x4A002201054081ULL, 0x43002001001040ULL, 0x01005000B90120ULL, 
	0x80800C00080080ULL, 0x07808004000A00ULL, 0x04001842100184ULL, 
	0x01000081000042ULL, 0x800240042000C0ULL, 0x20008080204000ULL, 
	0x02808020001000ULL, 0x000A0020120040ULL, 0x08010009100500ULL, 
	0x84808002000400ULL, 0x01C40010021108ULL, 0x01020004008041ULL, 
	0x00400080002388ULL, 0x40032100408300ULL, 0x00200100430010ULL, 
	0x08090100100024ULL, 0x02110100040800ULL, 0x02000600081004ULL, 
	0x46000A00040849ULL, 0x00800080084100ULL, 0x8000200040004AULL, 
	0x22200440401004ULL, 0x00200080801000ULL, 0x00282103001000ULL, 
	0x80880080805400ULL, 0x00800201801400ULL, 0x04800200802100ULL, 
	0x00004082000401ULL, 0x00802040008002ULL, 0x10102000484000ULL, 
	0x80500020008080ULL, 0x081200200A0040ULL, 0x04008040080800ULL, 
	0x0440900C080120ULL, 0x01084110140012ULL, 0x00284100820004ULL,
	0x41401020800480ULL, 0x00400900298500ULL, 0x00102004410100ULL, 
	0x00402092002A00ULL, 0x05001298000500ULL, 0x44010040420040ULL, 
	0x00028308100400ULL, 0x00800441001080ULL, 0x00442205008012ULL, 
	0x00210110400081ULL, 0x10104009022001ULL, 0x40045000090021ULL, 
	0x02000804A0100AULL, 0x91005802040001ULL, 0x01000400820001ULL, 
	0x090000804A0A21ULL
};

/** 
 * Masks for vertical and horizontal rook attacks. 
 */
const uint64_t rookMasks[64] = {
	0x000101010101017EULL, 0x000202020202027CULL, 0x000404040404047AULL, 
	0x0008080808080876ULL, 0x001010101010106EULL, 0x002020202020205EULL,
	0x004040404040403EULL, 0x008080808080807EULL, 0x0001010101017E00ULL,
	0x0002020202027C00ULL, 0x0004040404047A00ULL, 0x0008080808087600ULL,
	0x0010101010106E00ULL, 0x0020202020205E00ULL, 0x0040404040403E00ULL,
	0x0080808080807E00ULL, 0x00010101017E0100ULL, 0x00020202027C0200ULL,
	0x00040404047A0400ULL, 0x0008080808760800ULL, 0x00101010106E1000ULL,
	0x00202020205E2000ULL, 0x00404040403e4000ULL, 0x00808080807E8000ULL,
	0x000101017E010100ULL, 0x000202027C020200ULL, 0x000404047A040400ULL,
	0x0008080876080800ULL, 0x001010106E101000ULL, 0x002020205E202000ULL,
	0x004040403E404000ULL, 0x008080807E808000ULL, 0x0001017E01010100ULL,
	0x0002027C02020200ULL, 0x0004047A04040400ULL, 0x0008087608080800ULL,
	0x0010106E10101000ULL, 0x0020205E20202000ULL, 0x0040403E40404000ULL,
	0x0080807E80808000ULL, 0x00017E0101010100ULL, 0x00027C0202020200ULL,
	0x00047A0404040400ULL, 0x0008760808080800ULL, 0x00106E1010101000ULL,
	0x00205E2020202000ULL, 0x00403E4040404000ULL, 0x00807E8080808000ULL,
	0x007E010101010100ULL, 0x007C020202020200ULL, 0x007A040404040400ULL,
	0x0076080808080800ULL, 0x006E101010101000ULL, 0x005E202020202000ULL,
	0x003E404040404000ULL, 0x007E808080808000ULL, 0x7E01010101010100ULL,
	0x7C02020202020200ULL, 0x7A04040404040400ULL, 0x7608080808080800ULL,
	0x6E10101010101000ULL, 0x5E20202020202000ULL, 0x3E40404040404000ULL,
	0x7E80808080808000ULL
};

/** 
 * Shifts for finding rook indices. 
 */
const int rookShifts[64] = {
	52, 53, 53, 53, 53, 53, 53, 52,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	52, 53, 53, 53, 53, 53, 53, 52,
};

/** 
 * Magic numbers for diagonal attacks. 
 */
const uint64_t bishopMagicNums[64] = {
	0x04081001020051, 0x10010842808000, 0x10010045020008, 0x18204040080000,
	0x21104008000008, 0x6088200A00B954, 0x04010410040000, 0x30220802080200,
	0x00400808008280, 0x02200803010030, 0x00084283020080, 0x00040408808000,
	0x40011040040000, 0x00160904200040, 0xC9510082202100, 0x03004208A46000,
	0x41022008068080, 0x08003102008400, 0x50008802254050, 0x04000814101002,
	0x00804400A04000, 0x01008200820103, 0x00400121101000, 0x0A020100848C00,
	0x2410000C200842, 0x410400100CC802, 0x88080050882140, 0x11080004004090,
	0x40840002020200, 0x00D08014100400, 0x01040A22108408, 0x90810102014200,
	0x06104000100200, 0x08420800428828, 0xC0A80100080200, 0x00860080080080,
	0x0C010200040084, 0x21004100020100, 0x0210A100020801, 0x00820081005400,
	0x91100310002010, 0x00C21004001004, 0x0A002208040104, 0x080020510A4800,
	0x14080104000241, 0x06008102000100, 0x02108409020489, 0x02080200B00080,
	0x44010110108100, 0x01010082200005, 0x48010401040000, 0x0100002088000D,
	0x10104008288200, 0xDCC4100A020000, 0x040410144104C0, 0x48500102006008,
	0x08440048080410, 0x08020184841000, 0x00000201466800, 0x42100024420200,
	0x00000040050102, 0x00800410420200, 0x002004C1080100, 0x20204101010010
};

/** 
 * Masks for diagonal attacks. 
 */
const uint64_t bishopMasks[64] = {
	0x40201008040200, 0x00402010080400, 0x00004020100A00, 0x00000040221400,
	0x00000002442800, 0x00000204085000, 0x00020408102000, 0x02040810204000,
	0x20100804020000, 0x40201008040000, 0x004020100A0000, 0x00004022140000,
	0x00000244280000, 0x00020408500000, 0x02040810200000, 0x04081020400000,
	0x10080402000200, 0x20100804000400, 0x4020100A000A00, 0x00402214001400,
	0x00024428002800, 0x02040850005000, 0x04081020002000, 0x08102040004000,
	0x08040200020400, 0x10080400040800, 0x20100A000A1000, 0x40221400142200,
	0x02442800284400, 0x04085000500800, 0x08102000201000, 0x10204000402000,
	0x04020002040800, 0x08040004081000, 0x100A000A102000, 0x22140014224000,
	0x44280028440200, 0x08500050080400, 0x10200020100800, 0x20400040201000,
	0x02000204081000, 0x04000408102000, 0x0A000A10204000, 0x14001422400000,
	0x28002844020000, 0x50005008040200, 0x20002010080400, 0x40004020100800,
	0x00020408102000, 0x00040810204000, 0x000A1020400000, 0x00142240000000,
	0x00284402000000, 0x00500804020000, 0x00201008040200, 0x00402010080400,
	0x02040810204000, 0x04081020400000, 0x0A102040000000, 0x14224000000000,
	0x28440200000000, 0x50080402000000, 0x20100804020000, 0x40201008040200
};

/** 
 * Shifts for finding bishop indices.
 */
const int bishopShifts[64] = {
	58, 59, 59, 59, 59, 59, 59, 58,
	59, 59, 59, 59, 59, 59, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 59, 59, 59, 59, 59, 59,
	58, 59, 59, 59, 59, 59, 59, 58
};

/******************************************************************************/
/********************** Zobrist hashing used in gameplay **********************/
template <typename T>
T generateZobristArray(int size) {
    std::mersenne_twister_engine<std::uint_fast64_t, 64, 312, 156, 31, 
            0xb5026f5aa96619e9, 29, 0x5555555555555555, 17, 
            0x71d67fffeda60000, 37, 0xfff7eee000000000, 43, 
            6364136223846793005> rng;
    T array;
    for (int i = 0; i < size; i++) array[i] = rng();
    return array;
}

typedef std::array<uint64_t, 64> ZobristArray64;
typedef std::array<uint64_t, 48> ZobristArray48;
typedef std::array<uint64_t, 16> ZobristArray16;
typedef std::array<uint64_t, 8> ZobristArray8;
typedef std::array<uint64_t, 1> ZobristArray1;

const ZobristArray64 ZOBRIST_WKING = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_BKING = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_WQUEEN = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_BQUEEN = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_WROOK = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_BROOK = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_WBISHOP = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_BBISHOP = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_WKNIGHT = generateZobristArray<ZobristArray64>(64);

const ZobristArray64 ZOBRIST_BKNIGHT = generateZobristArray<ZobristArray64>(64);

const ZobristArray48 ZOBRIST_WPAWN = generateZobristArray<ZobristArray48>(48);

const ZobristArray48 ZOBRIST_BPAWN = generateZobristArray<ZobristArray48>(48);

const ZobristArray16 ZOBRIST_CASTLING = generateZobristArray<ZobristArray16>(16);

const ZobristArray8 ZOBRIST_EP = generateZobristArray<ZobristArray8>(8);

const ZobristArray1 ZOBRIST_BLACK = generateZobristArray<ZobristArray1>(1);
/******************** End Zobrist hashing used in gameplay ********************/
/******************************************************************************/

#endif
