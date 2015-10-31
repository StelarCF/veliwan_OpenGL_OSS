#pragma once
#include <vector>
#include <map>
#include <string>
#include "sdlgl.h"
#include "IOCore.h"
#include "RenderCore.h"
#include "GameObject.h"
#include "GameLogic.h"
#include "GameEvent.h"
#include "Logger.h"
#pragma clang diagnostic ignored "-Wextra-tokens"
#undef log(level, format, ...)
#define log(level, format, ...) gc->logger.debug_log(__FILE__, __FUNCTION__, __LINE__, level, format "\n", ##__VA_ARGS__)

class GameCore : IOListener {
	SDL_Window *window;
	SDL_Surface *screen;
	SDL_GLContext context;
	RenderCore rc;
	IOCore ic;
	vector<GameObject> objects;

	RenderCamera *camera;

	struct {
		bool debug;
		bool ssaa;
		float resx, resy;
		GLenum type;
	} args;

	map<string, EventID_t> registeredEvents;
	int recount; //see map^ above 

	EventID_t Frame_Event;
	GameCore *gc;
public:
	map<string, GameLogic*> gameLogics;
	Logger logger;
	GameCore() {
		recount = 0;
		gc = this;
	}
	void init();
	void argument_interpreter(int argv, char **argc);
	void updateAll();
	void run();
	void die();
	EventID_t registerEvent(string name); // to make sure we speak "the same language"
	void throwEvent(GameEvent ev);
	void triggerEvents();
};
