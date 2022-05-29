
#ifndef UCI_HPP
#define UCI_HPP

#include "game.hpp"
// #include "search.hpp"

// TODO list all the possible uci inputs, and then list custom inputs
enum UciInputs {
    QUIT,
    UCI,
    ISREADY,
    SETOPTION,
    DEBUG,
    REGISTER,
    UCINEWGAME,
    POSITION,
    GO,
    STOP,
    PONDERHIT,

    INVALID_INPUT
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

class Uci {
    public:
        Position pos;
        SearchParams params;

        void parseInput(std::string);
        UciInputs hashInput(std::string);

        bool debug_mode = false;

        void handleIsReady();
        void handleSetOption();
        void handleDebug(std::vector<std::string>);
        void handleSetOption(std::vector<std::string>);
        void handleRegister(std::vector<std::string>);
        void handleUCI_NewGame();
        void handlePosition(std::vector<std::string>);
        void handleGo(std::vector<std::string>);
        void handleStop();
        void handlePonderHit();

        void sendInitialResponse();
        void sendIds();
        void options();
        void sendReadyOk();
        void sendBestMove();
        void sendCopyProtection();
        void sendRegistration();
        void sendInfo();

        Move parseMove(std::string);
};

#endif
