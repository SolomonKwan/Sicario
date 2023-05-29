#include "sicario.hpp"

int main(int argc, char* argv[]) {
	showInfo(argc, argv);
	showLogo();
	showStartUp();

	Sicario sicario;
	sicario.run();

	return EXIT_SUCCESS;
}
