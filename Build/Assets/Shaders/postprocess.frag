#version 430

#define INVERT_MASK		(1 << 0)
#define GRAYSCALE_MASK	(1 << 1)
#define COLORTINT_MASK	(1 << 2)
#define GRAIN_MASK		(1 << 3)
#define SCANLINE_MASK	(1 << 4)
#define CUSTOM_MASK		(1 << 5)

in layout(location = 0) vec2 ftexcoord;
out layout(location = 0) vec4 ocolor;

uniform float blend = 1;
uniform uint params = 0;
uniform vec3 tint;
uniform float time;

layout(binding = 0) uniform sampler2D screenTexture;

vec4 invert(in vec4 color)
{
	return vec4(vec3(1)- color.rgb, color.a);
}

vec4 grayscale(in vec4 color)
{
	return vec4(vec3((color.r + color.b + color.g) / 3), color.a);
}

vec4 colortint(in vec4 color, in vec3 tint)
{
	return vec4(color.rgb * tint, color.a);
}

float rand(vec2 n) { 
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

vec4 grain(in vec4 color)
{
	vec2 p = ftexcoord * 500.0f; 
	vec2 ip = floor(p);
    vec2 u = fract(p);

    float res = mix(mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
	return vec4(color.rgb * (res+0.5), 1);
}

vec4 scanline(in vec4 color)
{   
	float scanline = sin(ftexcoord.y * 1000.0f + time * 1000.0f) * 0.5f;
	return vec4(color.rgb - scanline, color.a);
}

vec4 custom(in vec4 color)
{
	color.r = color.r * (sin(time)+1)/2;
	color.g = color.g * (cos(time)+1)/3;
	color.b = color.b * (sin(time/2)+1)/4;
	color *= texture(screenTexture, ftexcoord);
	color -= ((sin(time)+1)/6) * invert(color);

	return vec4(color.r,color.g, color.b, 1);
}

void main()
{
	vec4 basecolor = texture(screenTexture, ftexcoord);
	vec4 postprocess = basecolor;

	if(bool(params & INVERT_MASK)) postprocess = invert(postprocess);
	if(bool(params & COLORTINT_MASK)) postprocess = colortint(postprocess, tint);
	if(bool(params & GRAYSCALE_MASK)) postprocess = grayscale(postprocess);
	if(bool(params & GRAIN_MASK)) postprocess = grain(postprocess);
	if(bool(params & SCANLINE_MASK)) postprocess = scanline(postprocess);
	if(bool(params & CUSTOM_MASK)) postprocess = custom(postprocess);
	
	ocolor = mix(basecolor, postprocess, blend);
}