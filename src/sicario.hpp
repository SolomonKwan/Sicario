
#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include "constants.hpp"
#include "game.hpp"
#include "uci.hpp"


class Sicario {
    public:
        ExitCode run();

    private:
        Position position;
        SicarioConfigs sicaroConfigs;

        // UCI command processing
        void processInput(std::string);
        UciInput hashCommandInput(std::string);
        SicarioOption hashOptionsInput(std::vector<std::string>);

        // Handlers for UCI protocol commands
        void handleUci();
        void handleIsReady();
        void handleSetOption();
        void handleDebug(std::vector<std::string>);
        void handleSetOption(std::vector<std::string>);
        void handleRegister(std::vector<std::string>);
        void handleUciNewGame();
        void handlePosition(std::vector<std::string>);
        void handleGo(std::vector<std::string>);
        void handleStop();
        void handlePonderHit();

        // Handlers for custom commands
        void handlePerft(std::string);

        // GUI messaging
        void communicate(std::string);
        void sendOption(OptionInfo);
        void sendReadyOk();
        void sendBestMove();
        void sendCopyProtection();
        void sendRegistration();
        void sendInfo();
        void sendInvalidCommand(std::string);
        void sendMissingArgument(std::string);
        void sendInvalidArgument(std::string);
        void sendInvalidOptionSyntax(std::string);
        void sendNoOptionGiven(std::string);
        void sendUnknownOption(std::string);
};

#endif
