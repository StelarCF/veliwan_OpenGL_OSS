#include "RTSCamera.h"
#include <iostream>
#include "RenderCore.h"

void RTSCamera::init() {
	// Add event registering here
}

void RTSCamera::update() {
	if(fforward) {
		zpoz += -movestep;
	}
	if(bbackward) {
		zpoz += movestep;
	}
	if(rright) {
		xpoz += movestep;
	}
	if(lleft) {
		xpoz += -movestep;
	}

	view_matrix = vmath::lookat(vec3(xpoz, ytofloat + miny + (maxy - miny) * zoom, zpoz + minz + (maxz - minz) * sqrt(zoom)), vec3(xpoz, ytofloat, zpoz), vec3(0.0f, 1.0f, 0.0f));
}

bool RTSCamera::handleEvent(SDL_Event ev) {
	switch(ev.type) {
		case SDL_MOUSEWHEEL:
			// TODO: Smooth scrolling
			zoom += ev.wheel.y * zoomstep;
			if(zoom < minzoom)
				zoom = minzoom;
			if(zoom > maxzoom)
				zoom = maxzoom;
			break;
		case SDL_KEYDOWN:
			//int fforward, bbackward, lleft, rright;
			switch(ev.key.keysym.sym) {
				case SDLK_w:
					fforward = true;
					break;
				case SDLK_s:
					bbackward = true;
					break;
				case SDLK_a:
					lleft = true;
					break;
				case SDLK_d:
					rright = true;
					break;
			}
			break;
		case SDL_KEYUP:
			switch(ev.key.keysym.sym) {
				case SDLK_w:
					fforward = false;
					break;
				case SDLK_s:
					bbackward = false;
					break;
				case SDLK_a:
					lleft = false;
					break;
				case SDLK_d:
					rright = false;
					break;
			}
			break;
		// The following currently copied from GhostCamera. TODO: Make it so that SDL_MOUSEBUTTONUP makes it a rectangle selection if held for more than a certain amount of time, instead of click select		
		case SDL_MOUSEBUTTONDOWN:
			if(ev.button.button == SDL_BUTTON_LEFT) {
				startx = ev.button.x;
				starty = ev.button.y;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if(ev.button.button == SDL_BUTTON_LEFT) {
				list<list<Task>::iterator> tmp;
				tmp = rc->areaselect(startx, starty, ev.button.x - startx + 1, ev.button.y - starty + 1);
				const Uint8 *state = SDL_GetKeyboardState(NULL);
				if(!state[SDL_SCANCODE_LCTRL])
					rc->clearHighlighted();
				rc->addToHighlight(tmp);
			}
		break;
		default:
			break;
	}
	return false;
}

void RTSCamera::OnEvent(GameEvent ev) {
}