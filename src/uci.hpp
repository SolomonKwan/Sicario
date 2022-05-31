
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

enum ConfigOption {
    THREAD,
    HASH,
    CLEAR_HASH,
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
    UNKNOWN_OPTION,
};

struct SicarioConfigs {
    bool debugMode = false; // This is set with the "debug" command, not "setoption".
    int threads = 1;
    int hash = 16; // Megabytes
    bool ponder = false;
    bool ownBook = false;
    int multiPv = 1;
    bool uciShowCurrLine = false;
    bool uciShowRefutations = false;
    bool uciLimitStrength = false;
    int uciElo = 3000;
    bool uciAnalyseMode = true;
    std::string uciOpponent = "";
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
