#version 430 core

layout(location = 0) attribute vec4 position;

out vec2 tc;

void main(void) {
	gl_Position = position;
	tc = (position.xy + 1.0) / 2;
}