// Warning for the brave:
// This code contains a combination of the standard C IO library (mostly for direct command line output) but also the C++ IO library (mostly for file reading)
#include "GameCore.h"
#undef main // SDL likes to redeclare your main as SDL_main... for no reason. Yeah.

// TODO: (10) Add a way to load various game constants from a file called constants.cfg

GameCore gc;

int main(int argv, char **argc) {
	gc.argument_interpreter(argv, argc);
	gc.init();
	gc.run();
	return 0;
}

/*
TODO
gamefilecore
physicscore
audiocore
networkcore
aicore
external tool -- creare de harti
temporary test files for render and physics cores.
*/

/*
STODO
integrate IOCore into everything
*/
