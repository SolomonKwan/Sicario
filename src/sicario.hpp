
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
        SicarioConfigs sicarioConfigs;

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
        void sendInvalidArgument(std::vector<std::string>);
        void sendUnknownOption(std::vector<std::string>);

        // Option setters
        void setOptionHash(std::vector<std::string>);
        void setOptionClearHash();
        void setOptionNalimovPath(std::vector<std::string>);
        void setOptionNalimovCache(std::vector<std::string>);
        void setOptionPonder(std::vector<std::string>);
        void setOptionOwnBook(std::vector<std::string>);
        void setOptionMultiPV(std::vector<std::string>);
        void setOptionUciShowCurrLine(std::vector<std::string>);
        void setOptionUciShowRefutations(std::vector<std::string>);
        void setOptionUciLimitStrength(std::vector<std::string>);
        void setOptionUciElo(std::vector<std::string>);
        void setOptionUciAnalyseMode(std::vector<std::string>);
        void setOptionUciOpponent(std::vector<std::string>);

        // SetOption command processing
        std::string getOptionName(std::vector<std::string>);
        std::string getOptionValue(std::vector<std::string>);
};

#endif
