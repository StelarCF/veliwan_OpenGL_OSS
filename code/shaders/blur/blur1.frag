#version 430 core

in vec2 tc;

uniform sampler2D tex;

const float blurSize = 1.0 / 1024.0;

void main(void) {
	vec4 sum;
	sum += texture2D(tex, vec2(tc.x - 4.0 * blurSize, tc.y)) * 0.05;
	sum += texture2D(tex, vec2(tc.x - 3.0 * blurSize, tc.y)) * 0.09;
	sum += texture2D(tex, vec2(tc.x - 2.0 * blurSize, tc.y)) * 0.12;
	sum += texture2D(tex, vec2(tc.x - 1.0 * blurSize, tc.y)) * 0.15;
	sum += texture2D(tex, vec2(tc.x, tc.y)) * 0.16;
	sum += texture2D(tex, vec2(tc.x + 1.0 * blurSize, tc.y)) * 0.15;
	sum += texture2D(tex, vec2(tc.x + 2.0 * blurSize, tc.y)) * 0.12;
	sum += texture2D(tex, vec2(tc.x + 3.0 * blurSize, tc.y)) * 0.09;
	sum += texture2D(tex, vec2(tc.x + 4.0 * blurSize, tc.y)) * 0.05;
	gl_FragColor = sum;
}