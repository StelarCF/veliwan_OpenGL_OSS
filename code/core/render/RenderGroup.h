#pragma once
#include <vector>
#include "vmath.h"
#include "RenderMaterial.h"
using namespace std;
using namespace vmath;

struct face {
	int nr; // >= 3, unsupported for > 4
	int *v, *t;
};

class RenderObject;

class RenderGroup {
	// NOTE: rot, pos in RenderGroups are by default null, unless animated
	vec3 rot, pos; // TODO: switch this to quaternion rotation, just so we have compatibility with klu
	mat4 rot_mat, pos_mat; // NOTE: dynamically computed when rot and pos are changed (which is why we will have setters and getters)
	vector<vec3> vertices;
	vector<vec2> tex_vertices;
	vector<face> faces;
	material_t *material;
	float *v, *vt;
	bool cv, ct;
	int pnr;
	void recompute();
public:
	RenderGroup() {
		cv = ct = false;
	}
	friend class RenderFactory;
	friend class RenderObject;
	friend class RenderCore; // Deprecated !
	friend void resetog(RenderObject &obj, RenderGroup &gr);
	friend void resetg(RenderGroup &gr);
};