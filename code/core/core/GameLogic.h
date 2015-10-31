#pragma once
#include "GameEvent.h"
#include "LogicFunction.h"
#include <queue>

/******************************************************************************
* Important note for GameLogic                                                *
* GameLogic serves as the most important building block of the engine         *
* Components                                                                  *
* - the event queue is for processing various non-critical events             *
* - the properties map is for accessing various data another GameLogic may    *
*   require                                                                   *
* - the function map is for accessing functions, for critical events          *
******************************************************************************/

class GameCore;

class GameLogic {
	queue<GameEvent> ev_q;
public:
	GameCore *gc;
	virtual void OnEvent(GameEvent ev) = 0;
	void addToEventQueue(GameEvent ev) {
		ev_q.push(ev);
	}
	void processEvents() {
		while(!ev_q.empty()) {
			OnEvent(ev_q.front());
			ev_q.pop();
		}
	}
	virtual void functionCall(FunctionCallArgs args) {}
};
