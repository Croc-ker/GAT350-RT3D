#version 430

in layout(location = 0) vec3 color;
in layout(location = 1) vec2 texcoord;

out layout(location = 0) vec4 ocolor;

layout(binding = 0) uniform sampler2D tex;

uniform vec2 offset;
uniform vec2 tiling;

uniform float time;

void main()
{
	vec3 texcolor = texture(tex, (texcoord * tiling) + offset).rgb;
	ocolor = vec4(texcolor, 1);
}
