#include "RenderGroup.h"

void RenderGroup::recompute() {
	int i;
	rot_mat = vmath::rotate(rot[0], rot[1], rot[2]);
	pos_mat = vmath::translate(pos[0], pos[1], pos[2]);
	vector<float> t_v, t_vt;
	for(i = 0; i < faces.size(); i++) {
		if(faces[i].nr > 4)
			throw "FATAL TOO MANY VERTICES ON FACE";
		if(faces[i].nr == 3) {
			// First point
			t_v.push_back(vertices[faces[i].v[0]][0]);
			t_v.push_back(vertices[faces[i].v[0]][1]);
			t_v.push_back(vertices[faces[i].v[0]][2]);
			// Second point
			t_v.push_back(vertices[faces[i].v[1]][0]);
			t_v.push_back(vertices[faces[i].v[1]][1]);
			t_v.push_back(vertices[faces[i].v[1]][2]);
			// Third point
			t_v.push_back(vertices[faces[i].v[2]][0]);
			t_v.push_back(vertices[faces[i].v[2]][1]);
			t_v.push_back(vertices[faces[i].v[2]][2]);

			// Now for the UV map
			// First point
			t_vt.push_back(tex_vertices[faces[i].t[0]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[0]][1]);
			// Second point
			t_vt.push_back(tex_vertices[faces[i].t[1]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[1]][1]);
			// Third point
			t_vt.push_back(tex_vertices[faces[i].t[2]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[2]][1]);
		} else if(faces[i].nr == 4) {
			//throw "FATAL QUADS NOT IMPLEMENTED YET";
			// First point
			t_v.push_back(vertices[faces[i].v[0]][0]);
			t_v.push_back(vertices[faces[i].v[0]][1]);
			t_v.push_back(vertices[faces[i].v[0]][2]);
			// Second point
			t_v.push_back(vertices[faces[i].v[1]][0]);
			t_v.push_back(vertices[faces[i].v[1]][1]);
			t_v.push_back(vertices[faces[i].v[1]][2]);
			// Third point
			t_v.push_back(vertices[faces[i].v[2]][0]);
			t_v.push_back(vertices[faces[i].v[2]][1]);
			t_v.push_back(vertices[faces[i].v[2]][2]);

			// Now for the UV map
			// First point
			t_vt.push_back(tex_vertices[faces[i].t[0]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[0]][1]);
			// Second point
			t_vt.push_back(tex_vertices[faces[i].t[1]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[1]][1]);
			// Third point
			t_vt.push_back(tex_vertices[faces[i].t[2]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[2]][1]);

			// And now for the second tri that makes up the quad
			// First point
			t_v.push_back(vertices[faces[i].v[0]][0]);
			t_v.push_back(vertices[faces[i].v[0]][1]);
			t_v.push_back(vertices[faces[i].v[0]][2]);
			// Second point
			t_v.push_back(vertices[faces[i].v[3]][0]);
			t_v.push_back(vertices[faces[i].v[3]][1]);
			t_v.push_back(vertices[faces[i].v[3]][2]);
			// Third point
			t_v.push_back(vertices[faces[i].v[2]][0]);
			t_v.push_back(vertices[faces[i].v[2]][1]);
			t_v.push_back(vertices[faces[i].v[2]][2]);

			// Now for the UV map
			// First point
			t_vt.push_back(tex_vertices[faces[i].t[0]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[0]][1]);
			// Second point
			t_vt.push_back(tex_vertices[faces[i].t[3]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[3]][1]);
			// Third point
			t_vt.push_back(tex_vertices[faces[i].t[2]][0]);
			t_vt.push_back(tex_vertices[faces[i].t[2]][1]);
		}
	}
	if(v != NULL && cv) {
		delete[] v;
	}
	if(vt != NULL && ct) {
		delete[] vt;
	}
	cv = ct = true;
	pnr = t_v.size() / 3;
	v = new float[t_v.size()];
	for(i = 0; i < t_v.size(); i++)
		v[i] = t_v[i];
	vt = new float[t_vt.size()];
	for(i = 0; i < t_vt.size(); i++)
		vt[i] = t_vt[i];
}
