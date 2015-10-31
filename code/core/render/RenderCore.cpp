#include "RenderCore.h"
#include "lodepng.h"
#include "GameCore.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include "GameCore.h"
// TODO: remove using namespace stds and properly use namespaces, dammit! you're hitting collisions (like log())
using namespace std;
// For use in places where you have math functions
#define logg(level, format, ...) gc->logger.debug_log(__FILE__, __FUNCTION__, __LINE__, level, format "\n", ##__VA_ARGS__)

// No GameCore logging in this one, I'm afraid
GLuint RenderCore::LCShader(string fileName, GLenum shaderType) {
	stringstream strStream;
	string source;
	ifstream file(fileName.c_str());
	if(!file.is_open()) {
		logg(LOG_CRIT, "Fatal error: Shader source not found: %s", fileName.c_str());
		throw (string("Fatal Error: Shader source not found: ") + fileName).c_str();
	}
	strStream << file.rdbuf();
	source = strStream.str();
	GLuint shader;
	shader = glCreateShader(shaderType);
	char *charSource;
	charSource = new GLchar[source.size() + 1]; // +1 to include the end character \0
	#ifdef _WIN32
	strcpy_s(charSource, source.size() + 1, source.c_str());
	#else
	strcpy(charSource, source.c_str());
	#endif
	glShaderSource(shader, 1, (const GLchar **) &charSource, NULL);
	glCompileShader(shader);
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
		logg(LOG_ERROR, "Shader compilation fail: ");
	else logg(LOG_NOTIFY, "Shader compilation success.");
	GLint maxLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	//The maxLength includes the NULL character
	if(!success) {
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		logg(LOG_ERROR, "%s\n", &errorLog[0]);
	}
	return shader;
}

list<Task>::iterator RenderCore::addTask(Task t) {
	return tasks.insert(tasks.end(), t);
}

void RenderCore::removeTask(list<Task>::iterator where) {
	tasks.erase(where);
}

// Attributes
// Posbuffer - 0
// Uvbuffer - 1

// Uniforms
// mv_loc - 0
// rot_loc - 1
// view_loc - 2
// proj_loc - 3
// hc - 4
// First texture - 5

// TODO: Find a way to make these NOT be all over the place
GLuint posbuffer, uvbuffer;
GLuint mv_loc, rot_loc, view_loc, proj_loc, hc_loc; // Standardized (see above)

void RenderCore::makeBuffer(framebuffer &fb, int resx, int resy) {
	glGenFramebuffers(1, &fb.id);
	logg(LOG_NOTIFY, "Making framebuffer with id %d\n", fb.id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);

	glGenTextures(1, &fb.col_buf);
	logg(LOG_INFO, "Color Buffer: %d\n", fb.col_buf);
	glBindTexture(GL_TEXTURE_2D, fb.col_buf);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resx, resy, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.col_buf, 0);

	glGenRenderbuffers(1, &fb.sten_buf);
	logg(LOG_INFO, "Stencil/Depth Buffer: %d\n", fb.sten_buf);
	glBindRenderbuffer(GL_RENDERBUFFER, fb.sten_buf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resx, resy);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb.sten_buf);
}

void clearBuffer() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void clearDepthBuffer() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderCore::init() {
	factory.gc = gc;
	factory.init();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	
	aspect = resx / resy;

	load("mainprog.prog", FileType::prog); // [0]
	load("highlight.prog", FileType::prog); // [1]
	load("blur1.prog", FileType::prog); // [2]
	load("blur2.prog", FileType::prog); // [3]
	load("screencopy.prog", FileType::prog); // [4]
	load("screencomposite.prog", FileType::prog); // [5]

	load("RF.png", FileType::png);
	load("checkerboard.png", FileType::png);
	load("box_texture_DONOTUSEINFINAL.png", FileType::png);
	//load("cube.mod", FileType::mod);
	load("checkerboard.png", FileType::png);
	load("wood_texture_DONOTUSEINFINAL.png", FileType::png);
	//load("flat.mod", FileType::mod);
	load("render.ini", FileType::ini);

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ai = aniso;
	logg(LOG_INFO, "Using anisotropic: x%f\n", aniso);

	glGenBuffers(1, &posbuffer); //bufferul asta e folosit asa temporar; inca nu m-am decis cat de abstract sa fie
	glBindBuffer(GL_ARRAY_BUFFER, posbuffer);
	glBufferData(GL_ARRAY_BUFFER, 1024 * 1024, NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &uvbuffer); //bufferul asta e folosit asa temporar; inca nu m-am decis cat de abstract sa fie
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, 1024 * 1024, NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);

	// Standardized, see above (they're like this for easy standard change)
	mv_loc = glGetUniformLocation(programs[0], "mv_matrix");
	view_loc = glGetUniformLocation(programs[0], "view_matrix");
	proj_loc = glGetUniformLocation(programs[0], "proj_matrix");
	rot_loc = glGetUniformLocation(programs[0], "rot_matrix");
	hc_loc = glGetUniformLocation(programs[1], "hc");

	// TODO: move this to GhostCamera
	cam->proj_matrix = vmath::perspective(50.0f, aspect, 0.05f, 10000.0f);

	// Setting up the framebuffers
	makeBuffer(fb1, resx, resy);
	makeBuffer(fb2, resx, resy);
	makeBuffer(fb3, resx, resy);
	makeBuffer(fb4, resx, resy);

	makeBuffer(fb_scene, resx, resy);
	makeBuffer(fb_highlight, resx, resy);
	makeBuffer(fb_gui, resx, resy);

	makeBuffer(fb_select, resx, resy);
}

void checkError() {
	GLenum glErr;
	while(true) {
		glErr = glGetError();
		if(glErr != GL_NO_ERROR)
			printf("%s\n", gluErrorString(glErr));
		else break;
	}
}

// glBindFramebuffer(GL_FRAMEBUFFER, 0);
// glBindFramebuffer(GL_FRAMEBUFFER, fb1.id);
// glClearBufferfv(GL_COLOR, 0, color2);
// glClearBufferfv(GL_DEPTH, 0, &one);

void RenderCore::drawModel_3D(list<Task>::iterator it, GLuint program, GLuint textureOverride, bool send_tc, bool send_tex, void (*inputter)(vector<RenderGroup>::iterator)) {
	vector<RenderGroup>::iterator itg;
	for(itg = it->ro->rendergroups.begin(); itg < it->ro->rendergroups.end(); itg++) {
		glUseProgram(program);

		glBindBuffer(GL_ARRAY_BUFFER, posbuffer); //input the position of the points
		glBufferSubData(GL_ARRAY_BUFFER, 0, itg->pnr * 3 * sizeof(float), itg->v);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // assume 0 is the position vector

		if(send_tc) {
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer); //input the uv map
			glBufferSubData(GL_ARRAY_BUFFER, 0, itg->pnr * 2 * sizeof(float), itg->vt);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL); // assume 1 is the uv vector
		}

		if(send_tex) {
			glActiveTexture(GL_TEXTURE0); //input the texture
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
			if(textureOverride == -1)
				glBindTexture(GL_TEXTURE_2D, itg->material->diffuseTex);
			else glBindTexture(GL_TEXTURE_2D, textureOverride);
		}

		if(inputter != NULL)
			inputter(itg);

		mat4 p, r;

		p = itg->pos_mat * it->ro->pos_mat;
		r = itg->rot_mat * it->ro->rot_mat;

		glUniformMatrix4fv(mv_loc, 1, GL_FALSE, p);
		glUniformMatrix4fv(rot_loc, 1, GL_FALSE, r);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, cam->proj_matrix);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, cam->view_matrix);

		glDrawArrays(GL_TRIANGLES, 0, itg->pnr);
	}
}

void RenderCore::drawFullscreen(GLuint program, bool send_tex, void (*inputter)(), GLuint textureOverride) {
	GLfloat fs[] = {-1.0f, -1.0f, -0.5f,
	                -1.0f, 1.0f, -0.5f,
	                1.0f, 1.0f, -0.5f,
	                1.0f, -1.0f, -0.5f};
	glUseProgram(program);

	glBindBuffer(GL_ARRAY_BUFFER, posbuffer); //input the position of the points
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * 4 * sizeof(float), fs);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // assume 0 is the position vector

	if(send_tex) {
		glActiveTexture(GL_TEXTURE0); //input the texture
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		if(textureOverride == -1)
			throw 84741; // Fatal
		else glBindTexture(GL_TEXTURE_2D, textureOverride);
	}

	if(inputter != NULL)
		inputter();
	// We don't use any matrices, because it's assumed to be screenspace already; no, not even the highlight color

	glDrawArrays(GL_QUADS, 0, 4);
}

const GLfloat hc_color[] = {0.7, 1.0, 0.7, 1.0};
const GLfloat dark_color[] = {0.0, 0.0, 0.0, 0.0};

static void highlight_inputter(vector<RenderGroup>::iterator it) {
	glUniform4fv(hc_loc, 1, hc_color);
}

static void dark_inputter(vector<RenderGroup>::iterator it) {
	glUniform4fv(hc_loc, 1, dark_color);
}

void RenderCore::blur(framebuffer fb1, framebuffer fb2, framebuffer fb3) { /// Takes it from fb1, uses fb2 as intermediary and draws it into fb3 (fb3 can be the same as fb1)
	glBindFramebuffer(GL_FRAMEBUFFER, fb2.id);
	clearBuffer();
	drawFullscreen(programs[2], true, do_not2, fb1.col_buf);

	glBindFramebuffer(GL_FRAMEBUFFER, fb3.id);
	clearBuffer();
	drawFullscreen(programs[3], true, do_not2, fb2.col_buf);
}

void RenderCore::tripleblur(framebuffer fb1, framebuffer fb2, framebuffer fb3) {
	glBindFramebuffer(GL_FRAMEBUFFER, fb3.id);
	blur(fb1, fb2, fb3);
	glBindFramebuffer(GL_FRAMEBUFFER, fb1.id);
	blur(fb3, fb2, fb1);
	glBindFramebuffer(GL_FRAMEBUFFER, fb3.id);
	blur(fb1, fb2, fb3);
}

framebuffer fb_ext;

void introduce_fb_ext() {
	glActiveTexture(GL_TEXTURE1); //input the texture
	glBindTexture(GL_TEXTURE_2D, fb_ext.col_buf);
}
void introduce_fb_ext_with_hc() {
	glActiveTexture(GL_TEXTURE1); //input the texture
	glBindTexture(GL_TEXTURE_2D, fb_ext.col_buf);
	glUniform4fv(hc_loc, 1, hc_color);
}

void RenderCore::drawScene(framebuffer fb) {
	list<Task>::iterator it;
	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
	clearBuffer();

	for(it = tasks.begin(); it != tasks.end(); it++) {
		if(it->type == Task::model_3d) {
			drawModel_3D(it, programs[0]);
		}
	}
}

void RenderCore::drawHighlight(framebuffer fb) {
	list<list<Task>::iterator>::iterator it;
	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
	clearBuffer();
	
	for(it = tasks_highlighted.begin(); it != tasks_highlighted.end(); it++) {
		// Draw the object with the highlight color into fb1
		drawModel_3D(*it, programs[1], -1, true, false, highlight_inputter);
	}
	// Blur the result x3, and place it back in fb
	tripleblur(fb, fb1, fb);

	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
	clearDepthBuffer();
	for(it = tasks_highlighted.begin(); it != tasks_highlighted.end(); it++) {
		// Draw the object with black into fb1
		drawModel_3D(*it, programs[1], -1, true, false, dark_inputter);
	}
}

void RenderCore::compositeFBHighlight(framebuffer fb1, framebuffer fb2, framebuffer into) {
	glBindFramebuffer(GL_FRAMEBUFFER, into.id);
	clearBuffer();
	fb_ext = fb1;
	drawFullscreen(programs[5], true, introduce_fb_ext_with_hc, fb2.col_buf);
}

bool oneh = true, twoh = true; // Temporary workaround for highlighting crash (when no object is highlighted initially)

void RenderCore::render() {

	if(oneh) {
		tasks.begin()->ro->highlighted = true; // THIS METHOD WILL CRASH IS THE FIRST TASK IS NOT A MODEL_3D
		oneh = false;
	} else if(twoh) {
		tasks.begin()->ro->highlighted = false;
		twoh = false;
	}

	/// Highlighting into fb_highlight
	drawHighlight(fb_highlight);

	// Scene into fb_scene
	drawScene(fb_scene);

	// Composite the highlight onto the scene
	compositeFBHighlight(fb_scene, fb_highlight, screen);

	checkError();
}

bool ff = true;

GLuint RenderCore::load(string file, FileType::FileType filetype, GLenum w) {
	logg(LOG_INFO, "Loading %s", file.c_str());
	if (filetype == FileType::png) {
		GLuint tex;
		unsigned int w, h;
		vector<unsigned char> tex_data;
		texture_t fin;
		glGenTextures(1, &tex);
		lodepng::decode(tex_data, w, h, file);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &tex_data[0]);
		fin.diffuseTex = tex;
		fin.w = w;
		fin.h = h;
		textures.insert(make_pair(file, fin));
	} else if(filetype == FileType::prog) {
		ifstream f(file.c_str());
		int nr;
		GLuint p;
		p = glCreateProgram(); // Cerem lui OpenGL sa creeze un nou program
		programs.push_back(p);
		GLuint crt;
		f >> nr;
		string a, b;
		while(nr) {
			nr--;
			f >> a >> b;
			GLenum type;
			if(b == "vertex")
				type = GL_VERTEX_SHADER;
			else if(b == "fragment")
				type = GL_FRAGMENT_SHADER;
			else if(b == "tessco")
				type = GL_TESS_CONTROL_SHADER;
			else if(b == "tessev")
				type = GL_TESS_EVALUATION_SHADER;
			else if(b == "geometry")
				type = GL_GEOMETRY_SHADER;
			crt = load(a, FileType::glsl, type);
			glAttachShader(p, crt);
		}
		glLinkProgram(p);
		GLint validProgram;
		glGetProgramiv(p, GL_VALIDATE_STATUS, &validProgram);
		if(!validProgram) {
			printf("Failed validation of program: ");
			GLint maxLength = 0;
			glGetProgramiv(p, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char> errorLog(maxLength);
			glGetProgramInfoLog(p, maxLength, &maxLength, &errorLog[0]);
			printf("%s\n", &errorLog[0]);
		}
	} else if(filetype == FileType::glsl) {
		return LCShader(file, w);
	} else if(filetype == FileType::ini) {
		// Warning: this won't load files with spaces in them!
		int i;
		ifstream f(file.c_str());
		// Format:
		// integer (number of 3D models to load)
		int nrtoload;
		// strings (3D file format) strings (the 3D models to load)
		string format, nexttoload;
		// integer (number of actual objects to place)
		int nrtoplace;
		// string (which of the 3D models to use) positions (6 floats each, indicating the position of the object)
		string whichtoplace;
		float px, py, pz, rx, ry, rz;
		f >> nrtoload;
		for(i = 0; i < nrtoload; i++) {
			f >> format >> nexttoload;
			//if(format == "mod")
			//	load(nexttoload, FileType::mod);
			if(format == "obj")
				load(nexttoload, FileType::obj);
		}
		f >> nrtoplace;
		for(i = 0; i < nrtoplace; i++) {
			f >> whichtoplace >> px >> py >> pz >> rx >> ry >> rz;
			spawnObject(whichtoplace, vec3(px, py, pz), vec3(rx, ry, rz));
		}
	} else if(filetype == FileType::obj) {
		map<string, RenderObject> tmp;
		tmp = factory.loadObjFile(file);
		ro_map.insert(tmp.begin(), tmp.end());
	}
	return 0;
}

void RenderCore::spawnObject(string which, vec3 pos, vec3 rot) {
	Task tmp;
	RenderObject *nt;
	nt = new RenderObject;
	*nt = ro_map[which];
	cout << ro_map[which].rendergroups.size() << endl;
	cout << "Spawning " << which << " at " << pos[0] << " " << pos[1] << " " << pos[2] << endl;
	nt->pos = pos;
	nt->rot = rot;
	nt->recompute();
	tmp.ro = nt;
	tmp.type = Task::model_3d;
	tasks.push_back(tmp);
}

vec3 multiply(mat4 a, vec3 b) { // FIXME: Have this be in vmath.h
	vec3 rez(0.0f, 0.0f, 0.0f);
	rez[0] = a[0][0] * b[0] + a[0][1] * b[1] + a[0][2] * b[2];
	rez[1] = a[1][0] * b[0] + a[1][1] * b[1] + a[1][2] * b[2];
	rez[2] = a[2][0] * b[0] + a[2][1] * b[1] + a[2][2] * b[2];
	return rez;
}

int fforward, bbackward, lleft, rright, uup, ddown;
float sm;
bool wireframe = false;
bool holding_rmb;

GLuint ot_color[] = {0.0, 0.0, 0.0, 0.0}; 
GLfloat ttc[] = {0.0, 0.0, 0.0, 0.00392157};
static void ot_inputter(vector<RenderGroup>::iterator it) {
	ttc[0] = (float)ot_color[0] / 255.0f;
	ttc[1] = (float)ot_color[1] / 255.0f;
	ttc[2] = (float)ot_color[2] / 255.0f;
	ttc[3] = 1.0f;
	glUniform4fv(hc_loc, 1, ttc);
}

// Seriously, why do I need resx and resy as arguments here? Fix ASAP - April 7th, 2015
list<Task>::iterator RenderCore::whichone(int x, int y, int resx, int resy) { // Coordinates are from 0 to 1
	list<Task>::iterator it;
	int i;
	glBindFramebuffer(GL_FRAMEBUFFER, fb_select.id);
	clearBuffer();
	i = 1;
	for(it = tasks.begin(); it != tasks.end(); it++) if(it->type == Task::model_3d)  {
		ot_color[3] = 1.0f;
		ot_color[2] = (i % 256);
		ot_color[1] = ((i / 256) % 256);
		ot_color[0] = ((i / (256 * 256)) % 256);
		drawModel_3D(it, programs[1], -1, false, false, ot_inputter);
		i++;
	}
	glReadPixels(x, resy - y, 1, 1, GL_RGBA, GL_FLOAT, ttc);
	ot_color[0] = ttc[0] * 255.0f;
	ot_color[1] = ttc[1] * 255.0f;
	ot_color[2] = ttc[2] * 255.0f;
	i = ot_color[0] * 256 * 256 + ot_color[1] * 256 + ot_color[2] - 1;
	cout << i << endl;
	if(i == -1) // We didn't actually click any object
		return tasks.end();
	it = tasks.begin();
	while(i > 0) {
		i--;
		it++;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return it;
}

// For efficiency reasons, we don't really need 1920x1080 pixels for selecting
#define SELECT_RESOLUTION_X 4
#define SELECT_RESOLUTION_Y 4

list<list<Task>::iterator> RenderCore::areaselect(int x, int y, int w, int h) { // Coordinates are from 0 to 1
	list<list<Task>::iterator> rez;
	list<Task>::iterator it;
	list<list<Task>::iterator>::iterator it2;
	int i, j, k;
	glBindFramebuffer(GL_FRAMEBUFFER, fb_select.id);
	clearBuffer();
	i = 1;
	for(it = tasks.begin(); it != tasks.end(); it++) if(it->type == Task::model_3d)  {
		ot_color[3] = 1.0f;
		ot_color[2] = (i % 256);
		ot_color[1] = ((i / 256) % 256);
		ot_color[0] = ((i / (256 * 256)) % 256);
		drawModel_3D(it, programs[1], -1, false, false, ot_inputter);
		i++;
	}
	if(w < 0) {
		x += w;
		w *= -1;
	}
	if(h < 0) {
		y += h;
		h *= -1;
	}
	// Fuck this shit, not going to bother coding a proper solution, glReadPixels is retarded
	for(i = 0; i < w; i += SELECT_RESOLUTION_X)
		for(j = 0; j < h; j += SELECT_RESOLUTION_Y) {
			glReadPixels(x + i, resy - y - j, 1, 1, GL_RGBA, GL_FLOAT, ttc);
			ot_color[0] = ttc[0] * 255.0f;
			ot_color[1] = ttc[1] * 255.0f;
			ot_color[2] = ttc[2] * 255.0f;
			k = ot_color[0] * 256 * 256 + ot_color[1] * 256 + ot_color[2] - 1;
			if(k == -1) // We didn't actually click any object
				continue;
			it = tasks.begin();
			while(k > 0) {
				k--;
				it++;
			}
			bool ff;
			ff = true;
			if(it != tasks.end()) {
				for(it2 = rez.begin(); it2 != rez.end(); it2++) { // Inefficient, please fix this whole fucking function D: it's horrifying
					if(*it2 == it) {
						ff = false;
						break;
					}
				}
				if(ff)
					rez.push_back(it);
			}
		}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return rez;
}

bool RenderCore::handleEvent(SDL_Event ev) { // replaced with RenderCamera::handleEvent
	return cam->handleEvent(ev);
}

void RenderCore::doFrame() { // Replaced with a single RenderCamera::update
	cam->update();
}
