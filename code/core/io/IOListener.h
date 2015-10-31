#pragma once
#include "sdlgl.h"

class IOListener {
public:
	virtual bool handleEvent(SDL_Event) {
		// By default, do nothing
		return false;
	}
};
