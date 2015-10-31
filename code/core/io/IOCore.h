#pragma once
#include "sdlgl.h"
#include "IOListener.h"
#include <vector>
using namespace std;

class IOCore {
	vector<IOListener *> listeners;
public:
	void addListener(IOListener *);
	void processEvent(SDL_Event);
};