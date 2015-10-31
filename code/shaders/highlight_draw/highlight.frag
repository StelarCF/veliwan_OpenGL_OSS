#version 430 core

in vec4 highlight_color;

void main(void) {
	gl_FragColor = highlight_color;
}