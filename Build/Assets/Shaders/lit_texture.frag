#version 430

#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2


in layout(location = 0) vec3 fposition;
in layout(location = 1) vec3 fnormal;
in layout(location = 2) vec2 ftexcoord;


out layout(location = 0) vec4 ocolor;

uniform struct Material
{
	vec3 diffuse;
	vec3 specular;
	float shininess;

	vec2 offset;
	vec2 tiling;
} material;


uniform vec3 ambientLight;

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

} light;

layout(binding = 0) uniform sampler2D tex;

vec3 ads(in vec3 position, in vec3 normal)
{
	//AMBIENT
	vec3 ambient = ambientLight;

	//ATTENUATION
	float attenuation = 1;
	if(light.type != DIRECTIONAL){
		float distanceSqr = dot(light.position - position, light.position - position);
		float rangeSqr = light.range *light.range;
		attenuation = max(0, 1 - pow((distanceSqr / rangeSqr), 2.0f));
		attenuation = attenuation * attenuation;
	}

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
	vec3 diffuse = material.diffuse * (light.color * intensity);

	//SPECULAR
	vec3 specular = vec3(0);
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
		specular = material.specular * intensity;
	}
	//finally we add all of our lights and return the sum
	return ambient + (diffuse + specular) * light.intensity * attenuation;
}

void main()
{
	vec4 texcolor = texture(tex, ftexcoord);
	ocolor = texcolor * vec4(ads(fposition,fnormal), 1);
}
