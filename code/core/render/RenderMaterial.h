#pragma once
#include "sdlgl.h"
using namespace vmath;

struct basic_texture_t {
	GLuint tex;
	int w, h;
};

struct texture_t {
	GLuint diffuseTex; // diffuse texture e.g. the actual image on it
	GLuint ambientTex; // ambient
	GLuint normalTex; // normal map
	GLuint specularTex; // specular
	GLuint bumpTex; // bump map
	GLuint dispTex; // displacement map
	GLuint transparentTex; // transparency
	int w, h;
};

class material_t : public texture_t {
public:
	vec3 kAmbient;
	vec3 kDiffuse;
	vec3 kSpecular;
	float eSpecular;
	float transparency;
	int illum;
};
