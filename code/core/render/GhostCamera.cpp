#include "GhostCamera.h"
#include "RenderCore.h"
#include <list>
#include <cstdio>

class Task;

#define CAMERA_SPEED_MULTIPLIER (300)
// TODO: I think you can figure what should be fixed here
#define SENSITIVITY 0.2

void print_mat(mat4 m) {
	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n\n",

		    m[0][0], m[0][1], m[0][2], m[0][3],
		    m[1][0], m[1][1], m[1][2], m[1][3],
		    m[2][0], m[2][1], m[2][2], m[2][3],
		    m[3][0], m[3][1], m[3][2], m[3][3]);
}

void GhostCamera::sendRotation(float x, float y) {
	xrot += x * SENSITIVITY;
	yrot += y * SENSITIVITY;
	if(yrot > 90.0f)
		yrot = 90.0f;
	if(yrot < -90.0f)
		yrot = -90.0f;
	if(xrot > 360.0f)
		xrot -= 360.0f;
	if(xrot < 360.0f)
		xrot += 360.0f;
}

void GhostCamera::sendMotion(float x, float y, float z) {
	vmath::vec3 rez;
	rez = multiply(vmath::rotate(yrot, 0.0f, 0.0f) * vmath::rotate(0.0f, xrot, 0.0f), vec3(x / 12, y / 12, z / 12));
	xpoz += rez[0];
	ypoz += rez[1];
	zpoz += rez[2];
}

bool GhostCamera::handleEvent(SDL_Event ev) {
	switch (ev.type) { // check the event type
	case SDL_MOUSEBUTTONDOWN:
		if(ev.button.button == SDL_BUTTON_RIGHT) {
			holding_rmb = true;
			SDL_SetRelativeMouseMode(SDL_TRUE);
			SDL_ShowCursor(false);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if(ev.button.button == SDL_BUTTON_RIGHT) {
			holding_rmb = false;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_ShowCursor(true);
		} else if(ev.button.button == SDL_BUTTON_LEFT) {
			list<Task>::iterator tmp;
			try {
				tmp = rc->whichone(ev.button.x, ev.button.y, rc->resx, rc->resy);
				if(tmp != rc->tasks.end()) {
					tmp->ro->highlighted = !(tmp->ro->highlighted);
				}
			} catch(int e) {
				if(e != 44)
					throw "FATAL ERROR WRONG INT";
			}
		}
		break;
	case SDL_MOUSEMOTION:
		if(holding_rmb)
			sendRotation(ev.motion.xrel, ev.motion.yrel);
		break;
	case SDL_KEYDOWN:
		sm = 1;
		if((ev.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))) // Either of shifts is pressed
			sm = 3;
		if((ev.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))) // Either of controls is pressed
			sm = 0.25;
		if((ev.key.keysym.mod & (KMOD_LALT | KMOD_RALT))) // Either of alts is pressed
			sm = 0.1;
		sm *= CAMERA_SPEED_MULTIPLIER;
		switch(ev.key.keysym.sym) {
			case SDLK_w:
				fforward = true;
				break;
			case SDLK_s:
				bbackward = true;
				break;
			case SDLK_d:
				rright = true;
				break;
			case SDLK_a:
				lleft = true;
				break;
			case SDLK_z:
				ddown = true;
				break;
			case SDLK_x:
				uup = true;
				break;
			case SDLK_o:
				if(!wireframe) {
					wireframe = true;
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				} else {
					wireframe = false;
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				break;
			case SDLK_u:
				if(ai == 1)
					ai = 2;
				else if(ai == 2)
					ai = 4;
				else if(ai == 4)
					ai = 8;
				else if(ai == 8)
					ai = 16;
				else if(ai == 16)
					ai = 1;
				rc->aniso = ai;
				printf("Using aniso: %f\n", rc->aniso);
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
			case SDLK_d:
				rright = false;
				break;
			case SDLK_a:
				lleft = false;
				break;
			case SDLK_z:
				ddown = false;
				break;
			case SDLK_x:
				uup = false;
				break;
		}
		break;
	default:
		break;
	}
	return false;
}

void GhostCamera::update() {
	clock_t tn;
	tn = clock();
	float diff;
	diff = ((float) (tn - lt)) / CLOCKS_PER_SEC;
	lt = tn;
	if(fforward) {
		sendMotion(0.0f, 0.0f, -1.0f * sm * diff);
	}
	if(bbackward) {
		sendMotion(0.0f, 0.0f, 1.0f * sm * diff);
	}
	if(rright) {
		sendMotion(1.0f * sm * diff, 0.0f, 0.0f);
	}
	if(lleft) {
		sendMotion(-1.0f * sm * diff, 0.0f, 0.0f);
	}
	if(ddown) {
		sendMotion(0.0f, -1.0f * sm * diff, 0.0f);
	}
	if(uup) {
		sendMotion(0.0f, 1.0f * sm * diff, 0.0f);
	}
	
	view_matrix = vmath::rotate(yrot, 0.0f, 0.0f) * vmath::rotate(0.0f, xrot, 0.0f) * vmath::translate(-xpoz, -ypoz, -zpoz);
}