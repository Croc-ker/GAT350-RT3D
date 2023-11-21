#version 430

 #define INVERT_MASK  (1<<0)
 #define GRAYSCALE_MASK  (1 << 1)
 #define COLORTINT_MASK  (1 << 2)
 #define GRAIN_MASK  (1 << 3)
 #define SCANLINE_MASK  (1 << 4)
 #define KERNEL_MASK  (1 << 5)

in layout(location = 0) vec2 ftexCoord;
out layout(location = 0) vec4 ocolor;

uniform float blend = 1;
uniform uint params = 0;

layout (binding = 0) uniform sampler2D screenTexture;

vec4 invert(vec4 color)
{
	return vec4(vec3(1) - color.rgb, color.a);
}
vec4 grayscale(in vec4 color)
{
	return vec4(vec3((color.r+color.b+color.g)/3), color.a);
}
vec4 colorTint(in vec4 color)
{
	 vec4 tintColor = vec4(1.0, 0.5, 0.5, 1.0); 
    return color * tintColor;
}

float random(vec2 st) 
{
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 grain(vec4 color)
{
    float intensity = 0.1; 
    vec3 grainColor = vec3(random(ftexCoord) * intensity);
    return mix(color, vec4(grainColor, color.a), 0.5); 

}
vec4 scanline(vec4 color)
{
    float scanlineIntensity = 0.1; 
    float scanlineSpacing = 20.0;
    float scanline = sin(ftexCoord.y * scanlineSpacing);
    vec3 scanlineColor = vec3(scanlineIntensity * scanline);
    return mix(color, vec4(scanlineColor, color.a), 0.5);

}

const float offset = 1.0 / 300.0;  

vec4 kernel(vec4 color)
{
	 vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        1, 1, 1,
        1,  -8, 1,
        1, 1, 1
    ); //outline kernel
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, ftexCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    return vec4(col, 1.0);
}

void main()
{
	vec4 baseColor = texture(screenTexture, ftexCoord);
	vec4 postprocess = baseColor;
	if (bool(params & INVERT_MASK)) postprocess = invert(postprocess);
	if (bool(params & GRAYSCALE_MASK)) postprocess = grayscale(postprocess);
	if (bool(params & COLORTINT_MASK)) postprocess = colorTint(postprocess);
	if (bool(params & GRAIN_MASK)) postprocess = grain(postprocess);
	if (bool(params & SCANLINE_MASK)) postprocess = scanline(postprocess);
	if (bool(params & KERNEL_MASK)) postprocess = kernel(postprocess);

	ocolor = mix(baseColor, postprocess, blend);
}




//color * (sin(time)+1) * 0.5