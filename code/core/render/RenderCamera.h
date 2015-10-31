#pragma once
#include "IOCore.h"
#include "vmath.h"
#include "GameLogic.h"
using namespace vmath;

class RenderCore;

class RenderCamera : public IOListener, public GameLogic {
public:
	void init() {
		
	}
	RenderCore *rc;
	mat4 proj_matrix, view_matrix;
	virtual void update() = 0; // called every frame
};