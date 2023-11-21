#version 430

#define INVERT_MASK		(1 << 0)
#define	GRAYSCALE_MASK	(1 << 1)
#define	COLORTINT_MASK	(1 << 2)
#define GRAIN_MASK		(1 << 3)
#define SCANLINE_MASK	(1 << 4)
#define EDGE_MASK		(1 << 5)
#define BLUR_MASK		(1 << 6)
#define CUSTOM_MASK		(1 << 7)

in layout(location = 0) vec2 ftexcoord;

out layout(location = 0) vec4 ocolor;

uniform float blend = 1;
uniform uint params = 0;
uniform vec3 colorTint;

layout(binding = 0) uniform sampler2D screenTexture;


vec4 invert(in vec4 color)
{
	return vec4(vec3(1) - color.rgb, color.a);
}

vec4 grayscale(in vec4 color)
{
	return vec4(vec3((0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b) / 3), color.a);
}

vec4 tint(in vec4 color){
	return vec4(colorTint, 1) * color;
}

float random(vec2 st) 
{
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 grain(in vec4 color){

	float noise = random(ftexcoord);
    return vec4(color.rbg += noise * 0.1, color.a);
}

vec4 scanline(in vec4 color){
	return (int(gl_FragCoord.y) % 3 != 0 ? vec4(0,0,0,color.a) : (color));
}

vec4 edge()
{
	const float offset_x = 1.0f / 800.0f;  
	const float offset_y = 1.0f / 800.0f;  

	vec2 offsets[9] = vec2[]
	(
	    vec2(-offset_x,  offset_y), vec2( 0.0f,    offset_y), vec2( offset_x,  offset_y),
	    vec2(-offset_x,  0.0f),     vec2( 0.0f,    0.0f),     vec2( offset_x,  0.0f),
	    vec2(-offset_x, -offset_y), vec2( 0.0f,   -offset_y), vec2( offset_x, -offset_y) 
	);

	float kernel[9] = float[]
	(
	    1,  1, 1,
	    1, -8, 1,
		1,  1, 1
	);

    vec3 color = vec3(0.0f);
    for(int i = 0; i < 9; i++){
        color += vec3(texture(screenTexture, ftexcoord.st + offsets[i])) * kernel[i];
		}
    return vec4(color, 1);
}

vec4 blur()
{
	const float offset_x = 1.0f / 800.0f;  
	const float offset_y = 1.0f / 800.0f;  

	vec2 offsets[9] = vec2[]
	(
	    vec2(-offset_x,  offset_y), vec2( 0.0f,    offset_y), vec2( offset_x,  offset_y),
	    vec2(-offset_x,  0.0f),     vec2( 0.0f,    0.0f),     vec2( offset_x,  0.0f),
	    vec2(-offset_x, -offset_y), vec2( 0.0f,   -offset_y), vec2( offset_x, -offset_y) 
	);

	float kernel[9] = float[](
	    1.0 / 16, 2.0 / 16, 1.0 / 16,
	   2.0 / 16, 4.0 / 16, 2.0 / 16,
	   1.0 / 16, 2.0 / 16, 1.0 / 16  
	);

	vec3 color = vec3(0.0f);
    for(int i = 0; i < 9; i++){
        color += vec3(texture(screenTexture, ftexcoord.st + offsets[i])) * kernel[i];
		}
    return vec4(color, 1);
}

vec4 custom(){
	// Sample the original scene texture
    vec3 color = texture(screenTexture, ftexcoord).rgb;

    // Calculate the distance from the center of the screen
    float distance = length(ftexcoord - vec2(0.5));

    // Apply vignette effect
    float vignette = 1.0 - smoothstep(0.5, 0.5 + (blend * 0.05), distance);

    // Darken the color based on vignette strength
    color *= vignette;

    // Output the final color
	return vec4(color, 1.0);
}

void main()
{
	vec4 basecolor = texture(screenTexture, ftexcoord);
	vec4 postprocess = basecolor;

	if (bool(params & INVERT_MASK)) postprocess = invert(postprocess);
	if (bool(params & GRAYSCALE_MASK)) postprocess = grayscale(postprocess);
	if (bool(params & COLORTINT_MASK)) postprocess = tint(postprocess);
	if (bool(params & GRAIN_MASK)) postprocess = grain(postprocess);
	if (bool(params & SCANLINE_MASK)) postprocess = scanline(postprocess);
	if (bool(params & EDGE_MASK)) postprocess = edge();
	if (bool(params & BLUR_MASK)) postprocess = blur();
	if (bool(params & CUSTOM_MASK)) postprocess = custom();

	ocolor = mix(basecolor, postprocess, blend);
}
