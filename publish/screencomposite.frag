#version 430 core

in vec2 tc;

// Composites the second texture onto the first
layout(binding=0) uniform sampler2D tex1;
layout(binding=1) uniform sampler2D tex2;
layout(location = 4) uniform vec4 highlight_color;

void main(void) {
	vec4 t1, t2;
	t1 = texture2D(tex1, tc);
	t2 = texture2D(tex2, tc);
	t1.a = ((t1.r / highlight_color.r) + (t1.g / highlight_color.g) + (t1.b / highlight_color.b)) / 3;
	if(t1.a < 0.001)
		gl_FragColor = t2;
	else gl_FragColor = (t2 * (1 - t1.a) + t1 * t1.a);
	// gl_FragColor = vec4(t1.a, t1.a, t1.a, t1.a);
	// else gl_FragColor = ((1 - t2.a) * t1 + t2 * t2.a) / 2;
}