
#include <iostream>
#include "sicario.hpp"

int main(int argc, char* argv[]) {
    showInfo(argc, argv);
    showLogo();
    showStartUp();

    Sicario sicario;
    ExitCode code = sicario.run();

    return code;
}
