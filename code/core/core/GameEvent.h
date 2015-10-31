#pragma once

typedef int EventID_t;

struct GameEvent {
	EventID_t eventtype;
	void *data;
};