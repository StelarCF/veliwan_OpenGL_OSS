#pragma once
#include "RenderObject.h"
#include "RenderMaterial.h"
#include <string>
#include <map>

class GameCore;
class RenderCore;

class RenderFactory {
	basic_texture_t missing;
	RenderCore *rc;
public:
	GameCore *gc;
	void init();
	// Image loading functions.
	basic_texture_t loadPngFile(string filename, bool mipmap=true);
	// Each of these two files might contain multiple object definitions
	map<string, RenderObject> loadObjFile(string filename);
	map<string, material_t> loadMtlFile(string filename);
	friend class RenderCore;
};