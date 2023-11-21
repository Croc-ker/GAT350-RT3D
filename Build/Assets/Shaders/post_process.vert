#version 430

in layout(location = 0) vec3 vposition;
in layout(location = 1) vec2 vtexCoord;


out layout(location = 0) vec2 otexCoord;

void main()
{
	otexCoord = vtexCoord;
	gl_Position = vec4(vposition, 1.0);
}
