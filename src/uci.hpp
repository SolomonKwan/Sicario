
#ifndef UCI_HPP
#define UCI_HPP

#include "game.hpp"
#include "search.hpp"
class UCI {
    public:
        static void init(std::string input);
        void runUCI(std::string input);

        Pos pos;
        SearchParams params;

        bool debug_mode = false;

        void handleIsReady();
        void handleSetOption();
        void handleDebug(std::vector<std::string>);
        void handleSetOption(std::vector<std::string>);
        void handleRegister(std::vector<std::string>);
        void handleUCI_NewGame();
        void handlePosition(std::vector<std::string>);
        void handleGo(std::vector<std::string>, std::atomic_bool&);
        void handleStop(std::atomic_bool&);
        void handlePonderHit();

        void sendInitialResponse();
        void sendIds();
        void options();
        void sendReadyOk();
        void sendBestMove();
        void sendCopyProtection();
        void sendRegistration();
        void sendInfo();
};

#endif
