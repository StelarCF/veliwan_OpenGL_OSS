#include "RenderFactory.h"
#include "RenderCore.h"
#include "GameCore.h"
#include "lodepng.h"
#include <cstdio>
#include <cstring>
#include <fstream>

void checkError2() {
	GLenum glErr;
	glErr = glGetError();
	if(glErr != GL_NO_ERROR)
		printf("%s\n", gluErrorString(glErr));
}

void RenderFactory::init() {
	missing = loadPngFile("missingTexture.png");
}

basic_texture_t RenderFactory::loadPngFile(string filename, bool mipmap) {
	GLuint tex;
	unsigned int w, h, i, j;
	vector<unsigned char> img_data, tex_data;
	basic_texture_t fin;
	glGenTextures(1, &tex);
	lodepng::decode(img_data, w, h, filename);
	for(i = 0; i < h; i++)
		for(j = 0; j < w; j++) {
			tex_data.push_back(img_data[4 *((h - i - 1) * w + j)]);
			tex_data.push_back(img_data[4 *((h - i - 1) * w + j) + 1]);
			tex_data.push_back(img_data[4 *((h - i - 1) * w + j) + 2]);
			tex_data.push_back(img_data[4 *((h - i - 1) * w + j) + 3]);
		}
	glBindTexture(GL_TEXTURE_2D, tex);
	if(mipmap)
 		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &tex_data[0]);
 	else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &tex_data[0]);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	checkError2();
			int max_level;
			glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &max_level );
			int max_mipmap = -1;
			for(int i = 0; i < max_level; ++i) {
				int width;
				glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_WIDTH, &width );
				if(0 == width) {
				    max_mipmap = i-1;
				    break;
				}
			}
			log(LOG_NOTIFY, "Mipmap %d out of %d\n", max_mipmap, GL_TEXTURE_MAX_LEVEL);
	fin.tex = tex;
	fin.w = w;
	fin.h = h;
	return fin;
}

// Parsing functions
// Guarantee they always get the next non-null string
char *getfirst(char *s) {
	char *p;
	p = strtok(s, " \t");
	while(p != NULL && *p == 0) strtok(NULL, " \t");
	return s;
}

char *advance() {
	char *p;
	p = strtok(NULL, " \t");
	while(p != NULL && *p == 0) strtok(NULL, " \t");
	return p;
}

float advgetf() {
	char *p;
	float rez;
	p = advance();
	if(p == NULL)
		throw int(0);
	sscanf(p, "%f", &rez);
	return rez;
}

pair<int, int> advget2i() {
	char *p;
	int rez, rez2;
	p = advance();
	if(p == NULL)
		throw int(0);
	sscanf(p, "%d/%d", &rez, &rez2);
	return make_pair(rez, rez2);
}

int advgeti() {
	char *p;
	int rez;
	p = advance();
	if(p == NULL)
		throw int(0);
	sscanf(p, "%d", &rez);
	return rez;
}

void advgets(char *s) {
	char *p;
	p = advance();
	if(p == NULL)
		throw int(0);
	sscanf(p, "%s", s);
}

// Reset functions
void resetog(RenderObject &obj, RenderGroup &gr) {
	gr.rot = obj.rot = vec3(0.0f, 0.0f, 0.0f); //of, of, of ce trist :))
	gr.pos = obj.pos = vec3(0.0f, 0.0f, 0.0f);
	gr.vertices.clear();
	gr.tex_vertices.clear();
	gr.faces.clear();
	gr.material = NULL;
	obj.rendergroups.clear();
	obj.highlighted = false;
}

void resetg(RenderGroup &gr) {
	gr.rot = vec3(0.0f, 0.0f, 0.0f);
	gr.pos = vec3(0.0f, 0.0f, 0.0f);
	gr.vertices.clear();
	gr.tex_vertices.clear();
	gr.faces.clear();
	gr.material = NULL;
}

map<string, RenderObject> RenderFactory::loadObjFile(string filename) {
	int offsv, offsvt;
	log(LOG_NOTIFY, "Loading obj file %s", filename.c_str());
	offsv = offsvt = 0;
	map<string, RenderObject> result;
	RenderObject crtO;
	string crtOs;
	RenderGroup crtG;
	string crtGs;
	crtOs = filename;
	crtGs = "base";
	resetog(crtO, crtG);
	char s[101];
	char tol[101];
	char *p;
	FILE *f;
	f = fopen(filename.c_str(), "r");
	log(LOG_INFO, "Loading object %s\n", crtOs.c_str());
	while(fgets(s, 100, f) != NULL) {
		strtok(s, "#\n"); // No comments, no new lines
		if(s[0] == 0) // Either this is a complete comment line, or a blank line
			continue;
		p = getfirst(s);
		if(p == NULL) { // Nothing on this line but (maybe) comments, ignore it
			continue;
		} else if(strcmp(p, "o") == 0) {
			offsv += crtG.vertices.size();
			offsvt += crtG.tex_vertices.size();
			crtO.rendergroups.push_back(crtG);
			result.insert(make_pair(crtOs, crtO));
			crtGs = "base";
			resetog(crtO, crtG);
			advgets(tol);
			crtOs = tol;
			log(LOG_INFO, "Loading object %s\n", crtOs.c_str());
		} else if(strcmp(p, "g") == 0) {
			offsv += crtG.vertices.size();
			offsvt += crtG.tex_vertices.size();
			// TODO: Have the group name actually be used
			crtO.rendergroups.push_back(crtG);
			advgets(tol);
			resetg(crtG);
			crtGs = tol;
			log(LOG_INFO, "Loading group %s\n", crtGs.c_str());
		} else if(strcmp(p, "highlighted") == 0) {
			crtO.highlighted = true;
			log(LOG_INFO, "Loading group %s\n", crtGs.c_str());
		} else if(strcmp(p, "mtllib") == 0) {
			advgets(tol);
			log(LOG_INFO, "Loading material lib %s\n", tol);
			map<string, material_t> tmp;
			tmp = loadMtlFile(tol);
			rc->material_map.insert(tmp.begin(), tmp.end());
		} else if(strcmp(p, "usemtl") == 0) {
			advgets(tol);
			printf("Using material %s\n", tol);
			crtG.material = &(rc->material_map.at(string(tol)));
		} else if(strcmp(p, "v") == 0) {
			float x, y, z;
			x = advgetf();
			y = advgetf();
			z = advgetf();
			crtG.vertices.push_back(vec3(x, y, z));
		} else if(strcmp(p, "vt") == 0) {
			float x, y, z;
			x = advgetf();
			y = advgetf();
			crtG.tex_vertices.push_back(vec2(x, y));
		} else if(strcmp(p, "f") == 0) {
			vector<int> v, vt;
			face fc;
			try {
				while(true) {
					pair<int, int> crt;
					crt = advget2i();
					if(crt.first < 0)
						crt.first = crtG.vertices.size() + crt.first;
					else crt.first -= offsv + 1;
					if(crt.second < 0)
						crt.second = crtG.tex_vertices.size() + crt.second;
					else crt.second -= offsvt + 1;
					v.push_back(crt.first);
					vt.push_back(crt.second);
				}
			} catch(int e) {
				if(e == 0) {
					int i;
					fc.nr = v.size();
					fc.v = new int[fc.nr];
					fc.t = new int[fc.nr];
					for(i = 0; i < v.size(); i++) {
						fc.v[i] = v[i];
						fc.t[i] = vt[i];
					}
					crtG.faces.push_back(fc);
				} else {
					printf("FATAL ERROR IN READING FILE (WRONG INT THROWN)\n"); // I can't wait for when it actually happens and someone gets confused
				}
			}
		} else if(p[0] == '#' || p[0] == '\n') {
			continue;
		} else printf("Warning: Read unknown identifier \"%s\"\n", p);
	}
	crtO.rendergroups.push_back(crtG);
	result.insert(make_pair(crtOs, crtO));
	return result;
}

map<string, material_t> RenderFactory::loadMtlFile(string filename) {
	map<string, material_t> result;
	material_t loadedMaterial;
	string crtMat;
	char s[101];
	char tol[101];
	char *p;
	FILE *f;
	crtMat = filename;
	f = fopen(filename.c_str(), "r");
	while(fgets(s, 100, f) != NULL) {
		strtok(s, "#\n"); // Using strtok's destructive capabilities to our advantage
		if(s[0] == 0)
			continue;
		p = getfirst(s);
		if(p == NULL) {
			continue;
		} else if(strcmp(p, "newmtl") == 0) {
			if(crtMat != filename) {
				result.insert(make_pair(crtMat, loadedMaterial));
			}
			loadedMaterial.diffuseTex = missing.tex;
			loadedMaterial.ambientTex = missing.tex;
			loadedMaterial.normalTex = missing.tex;
			loadedMaterial.specularTex = missing.tex;
			loadedMaterial.bumpTex = missing.tex;
			loadedMaterial.dispTex = missing.tex;
			loadedMaterial.transparentTex = missing.tex;
			loadedMaterial.w = missing.w;
			loadedMaterial.h = missing.h;
			loadedMaterial.kAmbient = vec3(1.0f, 1.0f, 1.0f);
			loadedMaterial.kDiffuse = vec3(1.0f, 1.0f, 1.0f);
			loadedMaterial.kSpecular = vec3(1.0f, 1.0f, 1.0f);
			p = advance();
			crtMat = p;
		} else if(strcmp(p, "Ka") == 0) {
			float r, g, b;
			r = advgetf();
			g = advgetf();
			b = advgetf();
			loadedMaterial.kAmbient = vec3(r, g, b);
		} else if(strcmp(p, "Kd") == 0) {
			float r, g, b;
			r = advgetf();
			g = advgetf();
			b = advgetf();
			loadedMaterial.kDiffuse = vec3(r, g, b);
		} else if(strcmp(p, "Ks") == 0) {
			float r, g, b;
			r = advgetf();
			g = advgetf();
			b = advgetf();
			loadedMaterial.kSpecular = vec3(r, g, b);
		} else if(strcmp(p, "Ns") == 0) {
			float eS;
			eS = advgetf();
			loadedMaterial.eSpecular = eS;
		} else if(strcmp(p, "d") == 0 || strcmp(p, "Tr") == 0) {
			float d;
			d = advgetf();
			loadedMaterial.transparency = d;
		} else if(strcmp(p, "illum") == 0) {
			int il;
			il = advgeti();
			loadedMaterial.illum = il;
		} else if(strcmp(p, "map_Kd") == 0) { // TODO: Some of these will have optional options, check to make sure they don't interfere
			advgets(tol);
			basic_texture_t bt;
			bt = loadPngFile(tol);
			loadedMaterial.diffuseTex = bt.tex;
			loadedMaterial.w = bt.w;
			loadedMaterial.h = bt.h;
		} else if(strcmp(p, "map_Ka") == 0) {
			advgets(tol);
			loadedMaterial.ambientTex = loadPngFile(tol).tex;
		} else if(strcmp(p, "map_Kn") == 0) {
			advgets(tol);
			loadedMaterial.normalTex = loadPngFile(tol).tex;
		} else if(strcmp(p, "map_d") == 0) {
			advgets(tol);
			loadedMaterial.transparentTex = loadPngFile(tol).tex;
		} else if(strcmp(p, "map_bump") == 0 || strcmp(p, "bump") == 0) {
			advgets(tol);
			loadedMaterial.bumpTex = loadPngFile(tol).tex;
		} else if(strcmp(p, "disp") == 0) {
			advgets(tol);
			loadedMaterial.dispTex = loadPngFile(tol).tex;
		} else if(p[0] == '#' || p[0] == '\n') {
			continue;
		} else printf("Warning: Read unknown identifier \"%s\"\n", p);
	}
	if(crtMat != filename) {
		result.insert(make_pair(crtMat, loadedMaterial));
	}
	return result;
}
