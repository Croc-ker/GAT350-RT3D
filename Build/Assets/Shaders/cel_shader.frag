#version 430

#define POINT       0 
#define DIRECTIONAL 1
#define SPOT        2

#define ALBEDO_TEXTURE_MASK    (1 << 0)
#define SPECULAR_TEXTURE_MASK  (1 << 1)
#define NORMAL_TEXTURE_MASK    (1 << 2)
#define EMISSIVE_TEXTURE_MASK  (1 << 3)

in layout(location = 0) vec3 fposition;
in layout(location = 1) vec3 fnormal;
in layout(location = 2) vec2 ftexcoord;
in layout(location = 3) vec4 fshadowcoord;
in layout(location = 4) vec3 fviewdir;

out layout(location = 0) vec4 ocolor;

uniform struct Material
{
    uint params;
    vec3 albedo;
    vec3 specular;
    vec3 emissive;
    float shininess;

    vec2 tiling;
    vec2 offset;

} material;

uniform struct Light
{
   int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float range;
    float intensity;
    float innerangle;
    float outerangle;
} light[3];

uniform vec3 ambientLight;
uniform int numLights = 3;
uniform float shadowBias = 0.005;
uniform int shadingLevels = 5;
uniform float celSpecularCutoff = 0.3;
const float celScaleFactor = 1.0 / shadingLevels;
uniform float celOutline = 0.3;



layout(binding = 0) uniform sampler2D albedoTexture;
layout(binding = 1) uniform sampler2D specularTexture;
layout(binding = 2) uniform sampler2D normalTexture;
layout(binding = 3) uniform sampler2D emissiveTexture;
layout(binding = 4) uniform sampler2D cubemapTexture;
layout(binding = 5) uniform sampler2D shadowTexture;

float attenuation(in vec3 position1, in vec3 position2, in float range)
{
	float distanceSqr = dot(position1 - position2, position1 - position2);
	float rangeSqr = pow(range, 2.0);
	float attenuation = max(0, 1 - pow((distanceSqr / rangeSqr), 2.0));
	attenuation = pow(attenuation, 2.0);
 
	return attenuation;
}

float calcShadow(vec4 shadowcoord, float bias)
{
       return texture(shadowTexture, shadowcoord.xy).x < shadowcoord.z - shadowBias? 0:1;
}


void phong(in Light light, in vec3 position, in vec3 normal, out vec3 diffuse, out vec3 specular)
{
 //diffuse
   vec3 lightDir = (light.type == DIRECTIONAL) ? normalize(-light.direction) : normalize(light.position - position);
   
   float spotIntensity = 1;
   if(light.type == SPOT)
   {
    float angle = acos(dot(light.direction, -lightDir));
    //if(angle > light.innerangle) spotIntensity = 0;
    spotIntensity = smoothstep(light.outerangle + 0.001, light.innerangle, angle);

   }
   
    float intensity = max(dot(lightDir, normal), 0) * spotIntensity;
    float cellIntensity = floor(intensity * shadingLevels) * celScaleFactor;
    diffuse = (light.color * cellIntensity);
    

    //specular 
    specular = vec3(0);
    if (intensity > 0)
    {
        vec3 viewDir = normalize(-position);
        //phong
        //vec3 reflection = reflect(-lightDir, fnormal);
        //intensity = max(dot(reflection, viewDir), 0);

        //blinn phong 
        vec3 h = normalize(viewDir + lightDir);
        intensity = max(dot(h, normal), 0);

        intensity = pow(intensity, material.shininess);
        intensity = (intensity < celSpecularCutoff) ? 0 : 1;
        specular = vec3(intensity * spotIntensity);
    } 
}


void main()
{	
    vec4 albedoColor = bool(material.params & ALBEDO_TEXTURE_MASK)? texture(albedoTexture, ftexcoord)  : vec4(material.albedo, 1);
    vec4 specularColor = bool(material.params & SPECULAR_TEXTURE_MASK)? texture(specularTexture,ftexcoord) : vec4(material.specular,1);
    vec4 emissiveColor = bool(material.params & EMISSIVE_TEXTURE_MASK)?texture(emissiveTexture,ftexcoord) : vec4(material.emissive,1);

    //set ambient and emissive colors
	ocolor = vec4(ambientLight, 1) * albedoColor + emissiveColor;
 
    float shadow = calcShadow(fshadowcoord, shadowBias);
	// set lights
	for (int i = 0; i < numLights; i++)
	{   
        // outline
        // check cosine between surface normal and view direction
        //if less than cel outline threshold use outline color
        float outline = dot(fnormal, fviewdir);
        if (outline < celOutline)
        {
            ocolor = vec4(1);
            return; // done rendering this fragment (pixel)
        }

		vec3 diffuse;
		vec3 specular;
 
		float attenuation = (light[i].type == DIRECTIONAL) ? 1 : attenuation(light[i].position, fposition, light[i].range);
        
		phong(light[i], fposition, fnormal, diffuse, specular); 

       ocolor += ((vec4(diffuse, 1) * albedoColor) + vec4(specular, 1) * specularColor) * light[i].intensity * attenuation * shadow;
	}
}
