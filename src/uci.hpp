
#ifndef UCI_HPP
#define UCI_HPP

#include "game.hpp"
class UCI_Instance {
    public:
        Pos pos;

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
};
namespace UCI {
    void init(std::string input);
}

#endif
