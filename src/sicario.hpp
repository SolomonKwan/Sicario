
#ifndef SICARIO_HPP
#define SICARIO_HPP

#include <atomic>
#include "constants.hpp"
#include "game.hpp"
#include "uci.hpp"

void showInfo(int argc, char* argv[]);

void showLogo();

void showStartUp();

class Sicario {
    public:
        ExitCode run();
        Position getPosition() const;

    private:
        Position position;
        SicarioConfigs sicarioConfigs;
        std::atomic_bool searchTree = false;

        // UCI command processing
        void processInput(std::string&);
        UciInput hashCommandInput(std::string&);
        ConfigOption hashOptionsInput(std::vector<std::string>&);

        // Handlers for UCI protocol commands
        void handleUci();
        void handleIsReady();
        void handleSetOption();
        void handleDebug(std::vector<std::string>&);
        void handleSetOption(std::vector<std::string>&);
        void handleUciNewGame();
        void handlePosition(std::vector<std::string>&);
        void handleGo(std::vector<std::string>&);
        void handleStop();
        void handlePonderHit();

        // Handlers for custom commands
        void handlePerft(std::vector<std::string>&);
        void handleMove(std::vector<std::string>&);
        void handleUndo();
        void handleDisplay();
        void handleMoves();
        void handleBitboards();
        void handleRandom();
        void handleState();
        void handleOptions();

        // GUI messaging
        void communicate(std::string);
        void sendOption(OptionInfo&);
        void sendReadyOk();
        void sendBestMove();
        void sendCopyProtection();
        void sendRegistration();
        void sendInfo();
        void sendInvalidCommand(std::vector<std::string>&);
        void sendMissingArgument(std::vector<std::string>&);
        void sendInvalidArgument(std::vector<std::string>&);
        void sendUnknownOption(std::vector<std::string>&);
        void sendInvalidValue(std::vector<std::string>&);
        void sendArgumentOutOfRange(std::vector<std::string>&);

        // Option setters
        void setOptionThread(std::vector<std::string>&);
        void setOptionHash(std::vector<std::string>&);
        void setOptionClearHash();
        void setOptionPonder(std::vector<std::string>&);
        void setOptionOwnBook(std::vector<std::string>&);
        void setOptionMultiPV(std::vector<std::string>&);
        void setOptionUciShowCurrLine(std::vector<std::string>&);
        void setOptionUciShowRefutations(std::vector<std::string>&);
        void setOptionUciLimitStrength(std::vector<std::string>&);
        void setOptionUciElo(std::vector<std::string>&);
        void setOptionUciAnalyseMode(std::vector<std::string>&);
        void setOptionUciOpponent(std::vector<std::string>&);

        // SetOption command processing
        std::string getOptionName(std::vector<std::string>&);
        std::string getOptionValue(std::vector<std::string>&);

        // Configurable options of the engine
        // NOTE Currently, the default, min, max and var are all arbitrary
        OptionInfo thread { "Thread", "spin", "1", "1", "512" };
        OptionInfo hash { "Hash", "spin", "16", "0", "5000" };
        OptionInfo ponder { "Ponder", "check", "false" };
        OptionInfo ownBook { "OwnBook", "check", "false" };
        OptionInfo multiPv { "MultiPV", "spin", "1", "1", "5" };
        OptionInfo uciShowCurrLine { "UCI_ShowCurrLine", "check", "false" };
        OptionInfo uciShowRefutations { "UCI_ShowRefutations", "check", "false" };
        OptionInfo uciLimitStrength { "UCI_LimitStrength", "check", "false" };
        OptionInfo uciElo { "UCI_Elo", "spin", "3000", "1000", "3500" };
        OptionInfo uciAnalyseMode { "UCI_AnalyseMode", "check", "true" };
        OptionInfo uciOpponent { "UCI_Opponent", "string", "" };

        // Tree search
        void mcts();

        // Perft
        uint64_t perft(int depth, bool root = false);
};

#endif
