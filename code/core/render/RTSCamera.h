#pragma once
#include "RenderCamera.h"
#include <ctime>
#include <iostream>

class RTSCamera : public RenderCamera {
	// arguments
	float ytofloat;
	float miny, maxy;
	float minz, maxz;
	float minzoom, maxzoom;
	float zoomstep;
	float movestep;

	// position stuff
	float xpoz, zpoz;
	float zoom; // 0.0 to 1.0

	// movement stuff
	int fforward, bbackward, lleft, rright;

	// movement multiplier
	float sm;

	// other info
	bool holding_mmb;
	bool wireframe;
	int ai; // anisotropy - NOTE: Why is this even here

	// time info
	clock_t lt;

	int startx, starty; // box select

	void sendRotation(float x, float y);
	void sendMotion(float x, float y, float z);
public:
	RTSCamera() {
		ytofloat = 3.0f;
		miny = -0.2f;
		maxy = 20.0f;
		minz = 1.0f;
		maxz = 16.0f;
		zoom = minzoom = 0.1f;
		maxzoom = 2.0f;
		// Zoom Step could be a setting (zoom sensitivity)
		zoomstep = 0.05f;
		// Move Step could also be a setting (move sensitivity)
		movestep = 1.0f;
		wireframe = false;
		sm = 1.0f;
		fforward = bbackward = lleft = rright = false;
		xpoz = zpoz = 0.0f;
		holding_mmb = false;
		lt = clock();
	}
	void init();
	bool handleEvent(SDL_Event);
	void update();
	void OnEvent(GameEvent ev);
};
