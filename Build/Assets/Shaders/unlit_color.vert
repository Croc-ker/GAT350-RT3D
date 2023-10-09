#version 430

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 color;

out layout(location = 0) vec3 ocolor;

uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	ocolor = color;
	mat4 mvp = projection * view * model;
	float xo = sin(time + position.y * position.x) * 0.5;
	float yo = sin(time + position.y / position.x) * 0.5;
	float zo = cos(time + 10);
	gl_Position = mvp * vec4(position, 1.0);
}
