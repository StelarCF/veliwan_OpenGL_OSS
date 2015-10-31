#pragma once
#include "RenderCamera.h"
#include <ctime>

class GhostCamera : public RenderCamera {
	// position stuff
	float xrot, yrot;
	float xpoz, ypoz, zpoz;

	// movement stuff
	int fforward, bbackward, lleft, rright, uup, ddown;

	// movement multiplier
	float sm;

	// other info
	bool holding_rmb;
	bool wireframe;
	int ai; // anisotropy

	// time info
	clock_t lt;

	void sendRotation(float x, float y);
	void sendMotion(float x, float y, float z);
public:
	GhostCamera() {
		wireframe = false;
		sm = 1.0f;
		fforward = bbackward = lleft = rright = uup = ddown = false;
		xrot = yrot;
		xpoz = ypoz = zpoz = 0.0f;
		holding_rmb = false;
		lt = clock();
	}
	bool handleEvent(SDL_Event);
	void update();
};