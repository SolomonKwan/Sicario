
#ifndef UCI_HPP
#define UCI_HPP

#include "game.hpp"

/**
 * Input commands to the engine. Includes the UCI protocol and custom commands.
 */
enum UciInput {
    // UCI Protocol GUI to engine commands
    UCI,
    DEBUG,
    ISREADY,
    SETOPTION,
    REGISTER,
    UCINEWGAME,
    POSITION,
    GO,
    STOP,
    PONDERHIT,
    QUIT,

    // Custom commands to engine
    PERFT,

    // Sentinel for invalid command
    INVALID_COMMAND
};

enum SicarioOption {
    HASH,
    NALIMOV_PATH,
    NALIMOV_CACHE,
    PONDER,
    OWN_BOOK,
    MULTI_PV,
    UCI_SHOW_CURR_LINE,
    UCI_SHOW_REFUTATIONS,
    UCI_LIMIT_STRENGTH,
    UCI_ELO,
    UCI_ANALYSE_MODE,
    UCI_OPPONENT,

    // Sentinels for errors.
    INVALID_OPTION_SYNTAX,
    NO_OPTION_GIVEN,
    UNKNOWN_OPTION,
};

struct SicarioConfigs {
    bool DEBUG_MODE = false; // This is set with the "debug" command, not "setoption".
    int Hash = 16; // Megabytes
    std::string NalimovPath = "";
    int NalimovCache = 0; // Megabytes
    bool Ponder = false;
    bool OwnBook = false;
    int MultiPV = 1;
    bool UCI_ShowCurrLine = false;
    bool UCI_ShowRefutations = false;
    bool UCI_LimitStrength = false;
    int UCI_Elo = 3000;
    bool UCI_AnalyseMode = true;
    std::string UCI_Opponent = "";
};

struct OptionInfo {
    std::string name = "";
    std::string type = "";
    std::string def = "";
    std::string min = "";
    std::string max = "";
    std::vector<std::string> vars = {};
};

struct GoParams {
    std::vector<Move> moves;
    int wtime = 0;
    int btime = 0;
    int winc = 0;
    int binc = 0;
    int moves_to_go = 0;
    int depth = 0;
    int nodes = 0;
    int mate = 0;
    int movetime = 0;
    bool infinite = true;
};

#endif
