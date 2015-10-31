#pragma once
#include "vmath.h"
#include <vector>
#include "RenderCore.h"
using namespace std;
using namespace vmath;

class GameObject {
public:
	vec3 position;
	vec3 rotation;
	Task *rendertask;
	void update();
};
