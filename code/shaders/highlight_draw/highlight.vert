#version 430 core

layout(location = 0) attribute vec4 position;

layout(location = 0) uniform mat4 mv_matrix;
layout(location = 1) uniform mat4 rot_matrix;
layout(location = 2) uniform mat4 proj_matrix;
layout(location = 3) uniform mat4 view_matrix;
layout(location = 4) uniform vec4 hc;

out vec4 highlight_color;

void main(void) {
	gl_Position = proj_matrix * view_matrix * mv_matrix * rot_matrix * position;
	highlight_color = hc;
}