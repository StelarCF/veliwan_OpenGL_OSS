#include "RenderObject.h"

void RenderObject::recompute() {
	int i;
	rot_mat = vmath::rotate(rot[0], rot[1], rot[2]);
	pos_mat = vmath::translate(pos[0], pos[1], pos[2]);
	for(i = 0; i < rendergroups.size(); i++)
		rendergroups[i].recompute();
}