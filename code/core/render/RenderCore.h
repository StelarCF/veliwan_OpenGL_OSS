#pragma once
#include "sdlgl.h"
#include <string>
#include <list>
#include <map>
#include <vector>
#include "vmath.h"
#include "IOCore.h"
#include "RenderMaterial.h"
#include "RenderFactory.h"
#include "RenderCamera.h"
#include "GhostCamera.h"
#include "RTSCamera.h"
using namespace std;
using namespace vmath;

class GameCore;

struct Task
{
	enum Task_t {
		unknown,
		model_3d
	} type;
	union {
		RenderObject *ro;
	};
};

typedef list<Task>::iterator TaskPtr;

namespace FileType { // TODO: (35) Move this to FileCore
	enum FileType {
		png,
		glsl,
		prog,
		obj,
		mtl,
		ini // Initializer file; currently only used in RenderCore; WARNING: Ini files should only be loaded once for each core!
	};
};

struct framebuffer {
	GLuint id, col_buf, sten_buf, dep_buf;
};

vec3 multiply(mat4 a, vec3 b);


// TODO: Split the part of RenderCore that deals with camera position into a new class base with extensions called RenderCamera
// ----- As well as extended classes: RTSCamera, FPSCamera, EagleCamera
// ----- Then, RenderCore will no longer implement IOListener
// NOTE: Most of the camera relevant variables are currently in RenderCore.cpp
// ----- Also, object selection should be changed, to not depend on RenderCore or RenderCamera

/// The RenderCore is the main graphical unit of the game.
/// It displays objects and effects, aka Task%s and creates the final game scene
class RenderCore : public IOListener {
	list<Task> tasks;
	list<list<Task>::iterator> tasks_highlighted;
	vector<GLuint> programs;
	map<string, texture_t> textures; // Deprecated textures
	map<string, material_t> material_map;
	map<string, RenderObject> ro_map;
	RenderFactory factory;
	int ai;
	float aniso;
	float aspect;
	framebuffer screen;
	framebuffer fb1, fb2, fb3, fb4; // For swaps
	framebuffer fb_scene, fb_highlight, fb_gui;

	framebuffer fb_select;

public:
	float resx, resy;
	RenderCamera *cam;
	GameCore *gc;

	RenderCore()
	{
		// make sure the factory knows of our existence
		factory.rc = this;
		screen.id = 0;
		screen.col_buf = screen.sten_buf = screen.dep_buf = -1;
	}
	/// The init initializes the RenderCore, loading the programs required. It might also load certain textures it requires, however these may be loaded by other parts of the game (filecore?)
	void init();

	/// The rendering loop is executed once a cycle. It runs through all the Task%s and creates the scene
	void render();

	// Inputter/Helper functions
	static void do_not(vector<RenderGroup>::iterator it) {
		return;
	}
	static void do_not2() {
		return;
	}

	// Some other useful functions
	void makeBuffer(framebuffer &fb, int resx, int resy);

	/// Functions for rendering various things
	void blur(framebuffer fb1, framebuffer fb2, framebuffer fb3);
	void tripleblur(framebuffer fb1, framebuffer fb2, framebuffer fb3);
	void drawModel_3D(list<Task>::iterator it, GLuint program, GLuint textureOverride = -1, bool send_tc = true, bool send_tex = true, void (*inputter)(vector<RenderGroup>::iterator) = &do_not);
	void drawFullscreen(GLuint program, bool send_tex = false, void (*inputter)() = &do_not2, GLuint textureOverride = -1);
	void drawScene(framebuffer fb);
	void drawHighlight(framebuffer fb);
	void compositeFBHighlight(framebuffer fb1, framebuffer fb2, framebuffer into);

	/// Function for getting selected object
	list<Task>::iterator whichone(int x, int y, int resx, int resy);
	list<list<Task>::iterator> areaselect(int x, int y, int w, int h);
	void addToHighlight(list<Task>::iterator it) {
		if(it != tasks.end())
			tasks_highlighted.push_back(it);
	}
	void addToHighlight(list<list<Task>::iterator> things) {
		list<list<Task>::iterator>::iterator it;
		for(it = things.begin(); it != things.end(); it++)
			if(*it != tasks.end())
				tasks_highlighted.push_back(*it);
	}
	void clearHighlighted() {
		tasks_highlighted.clear();
	}

	/// Load a RenderCore specific file. The return value may be ignored, unless you are loading a glsl filetype
	GLuint load(string file, FileType::FileType filetype, GLenum w = 0); // w depinde de fisier
	void spawnObject(string which, vec3 pos, vec3 rot);
	list<Task>::iterator addTask(Task t);
	void removeTask(list<Task>::iterator where);
	GLuint LCShader(string fileName, GLenum shaderType);

	/// Temporary function for testing movement and IO
	bool handleEvent(SDL_Event);
	void doFrame();
	list<Task> *getTaskList() {
		return &tasks;
	}
	friend class RenderFactory;
	// TODO: Cut the friends, Charlie
	friend class RenderCamera;
	friend class GhostCamera;
};
