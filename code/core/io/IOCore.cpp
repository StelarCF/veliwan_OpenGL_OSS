#include "IOCore.h"

void IOCore::addListener(IOListener *l) {
	listeners.push_back(l);
}

void IOCore::processEvent(SDL_Event ev) {
	long i;
	for(i = 0; i < listeners.size(); i++)
		if(listeners[i]->handleEvent(ev))
			break;
}