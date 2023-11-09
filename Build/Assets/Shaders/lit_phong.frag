#version 430

#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

#define ALBEDO_TEXTURE_MASK		(1 << 0) //0001
#define SPECULAR_TEXTURE_MASK	(1 << 1) //0010
#define NORMAL_TEXTURE_MASK		(1 << 2) //0100
#define EMISSIVE_TEXTURE_MASK	(1 << 3) //1000

in layout(location = 0) vec3 fposition;
in layout(location = 1) vec3 fnormal;
in layout(location = 2) vec2 ftexcoord;


out layout(location = 0) vec4 ocolor;

uniform struct Material
{
	uint params;
	vec3 albedo;
	vec3 specular;
	vec3 emissive;
	float shininess;

	vec2 offset;
	vec2 tiling;
} material;


uniform vec3 ambientLight;

uniform int numLights = 3;

uniform struct Light
{
	int type;
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float range;
	float innerAngle;
	float outerAngle;

} lights[3];

layout(binding = 0) uniform sampler2D albedoTexture;
layout(binding = 1) uniform sampler2D specularTexture;
layout(binding = 2) uniform sampler2D normalTexture;
layout(binding = 3) uniform sampler2D emissiveTexture;

void phong(in Light light, in vec3 position, in vec3 normal, out vec3 diffuse, out vec3 specular)
{
	//DIFFUSE
	// find the direction the light is moveing in by subtracting the fragment position from the light source position and then normalize it
	vec3 lightDirection = (light.type == DIRECTIONAL) ? normalize(light.direction) : normalize(light.position - position);
	
	float spotIntensity = 1;
	if(light.type == SPOT){
		float cosine = acos(dot(light.direction, -lightDirection));
		//if (cosine > light.innerAngle) spotIntensity = 0;
		spotIntensity = smoothstep(light.outerAngle + 0.001f, light.innerAngle, cosine);
	}
	
	// find the intensity by doing the dot product of the light direction with the fragment normal then we clampp this value between 0 and 1
	float intensity = max(dot(lightDirection, normal), 0) * spotIntensity;	
	//finally we get the diffuse color by multiplying the light color by the intensity and then multiplying that product by the materials diffuse
	diffuse = (light.color * intensity);

	//SPECULAR
	specular = vec3(0);
	//first we check to see if the intensity is greater then 0 or not
	if(intensity > 0){
	//if it is then we find the reflection by using reflect on the inverse of our light direction and the fragment normal
		vec3 reflection = reflect(-lightDirection, normal);
		//we find the view direction by normalizing the inverse of the fragment normal
		vec3 viewDirection = normalize(-position);
		// then we find the intensity by doing the dot product of the reflection and view direction and then we clamp it
		intensity = max(dot(reflection, viewDirection), 0);
		//next we raise intensity by the power of shininess
		intensity = pow(intensity, material.shininess);
		//then we multiply the material specular by the intesity
		specular = vec3(intensity * spotIntensity);
	}
	//finally we add all of our lights and return the sum
}

float attenuation(in vec3 position1, in vec3 position2, in float range)
{
	float distanceSqr = dot(position1 - position2, position1 - position2);
	float rangeSqr = pow(range, 2.0);
	float attenuation = max(0, 1 - pow((distanceSqr / rangeSqr), 2.0));
	attenuation = pow(attenuation, 2.0);
 
	return attenuation;
}


//vec3 ads(in vec3 position, in vec3 normal)
//{
//	//AMBIENT
//	vec3 ambient = ambientLight;
//
//	//ATTENUATION
//	float attenuation = 1;
//	if(light.type != DIRECTIONAL){
//		float distanceSqr = dot(light.position - position, light.position - position);
//		float rangeSqr = light.range *light.range;
//		attenuation = max(0, 1 - pow((distanceSqr / rangeSqr), 2.0f));
//		attenuation = attenuation * attenuation;
//	}
//
//	
//	return ambient + (diffuse + specular) * light.intensity * attenuation;
//}
//
void main()
{
	vec4 albedoColor = bool(material.params & ALBEDO_TEXTURE_MASK) ? texture(albedoTexture, ftexcoord) : vec4(material.albedo, 1);
	vec4 specularColor = bool(material.params & SPECULAR_TEXTURE_MASK) ? texture(specularTexture, ftexcoord) : vec4(material.specular, 1);
	vec4 emissiveColor = bool(material.params & EMISSIVE_TEXTURE_MASK) ? texture(emissiveTexture, ftexcoord) : vec4(material.emissive, 1);
	
	//vec4 albedoColor = texture(albedoTexture, ftexcoord);//vec4(material.specular, 1);
	//vec4 specularColor = texture(specularTexture, ftexcoord);//vec4(material.specular, 1);
	//vec4 emissiveColor = texture(emissiveTexture, ftexcoord);//vec4(material.emissive, 1);


	//vec4 texcolor = texture(tex, ftexcoord);
	
	// set ambient light + emissive color
	ocolor = vec4(ambientLight, 1) * albedoColor + emissiveColor;
 
	// set lights
	for (int i = 0; i < numLights; i++)
	{
		vec3 diffuse;
		vec3 specular;
 
		float attenuation = (lights[i].type == DIRECTIONAL) ? 1 : attenuation(lights[i].position, fposition, lights[i].range);
 
		phong(lights[i], fposition, fnormal, diffuse, specular);
		ocolor += ((vec4(diffuse, 1) * albedoColor) + vec4(specular, 1) * specularColor) * lights[i].intensity * attenuation;
	}
}