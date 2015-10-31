#pragma once
#include <vector>
#include "vmath.h"
#include "RenderGroup.h"
using namespace std;
using namespace vmath;

class RenderObject {
	vec3 rot, pos; // TODO: switch this to quaternion rotation, just so we have compatibility with klu
	mat4 rot_mat, pos_mat; // NOTE: dynamically computed when rot and pos are changed (which is why we will have setters and getters)
	vector<RenderGroup> rendergroups;
	// NOTE: If I will have animations, animation info will be stored here
	void recompute();
	void recomputePosition() {
		rot_mat = vmath::rotate(rot[0], rot[1], rot[2]);
		pos_mat = vmath::translate(pos[0], pos[1], pos[2]);
	}
	material_t *getBase() {
		return rendergroups[0].material;
	}
public:
	bool highlighted;
	friend void resetog(RenderObject &obj, RenderGroup &gr);
	friend class RenderCore; // Deprecated !
	friend class RenderFactory;
	friend class GameObject;
};