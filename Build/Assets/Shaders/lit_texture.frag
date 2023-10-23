#version 430

in layout(location = 0) vec3 fposition;
in layout(location = 1) vec3 fnormal;
in layout(location = 2) vec2 ftexcoord;
out layout(location = 0) vec4 ocolor;

uniform vec3 ambientLight;

uniform struct Material
{
	vec3 diffuse;
	vec3 specular;
	float shininess;
	vec2 offset;
	vec2 tiling;
} material;

uniform struct Light
{

	vec3 position;
	vec3 color;

} light;

layout(binding = 0) uniform sampler2D tex;

vec3 ads(in vec3 position, in vec3 normal)
{
	//AMBIENT
	vec3 ambient = ambientLight;

	//DIFFUSE
	//this determines the direction of the light by subtracting the fragment position from the light source position before normalizing it
	vec3 lightDirection = normalize(light.position - position);

	//rhis determines the intensity of the light getting the dot product of the light's direction with the fragment normal and clamping this value between 0 and 1
	float intensity = max(dot(lightDirection, normal), 0);	

	//this determines the diffuse by multiplying the light's color and intensity then multiplying with the material's diffuse
	vec3 diffuse = material.diffuse * (light.color * intensity);

	//SPECULAR
	vec3 specular = vec3(0);
	if(intensity > 0){
		//if the intensity is greater than 0, we determine the reflection from using the reflect() function on the inversed light direction and the fragment's normal
		//man, i really don't like this.
		vec3 reflection = reflect(-lightDirection, normal);

		//this determines the view direction by normalizing the fragment position
		vec3 viewDirection = normalize(-position);

		//then we determine the intensity by getting the dot product of the reflection and view direction
		intensity = max(dot(reflection, viewDirection), 0);

		//after that we raise the intensity to the power of the material's shininess
		intensity = pow(intensity, material.shininess);

		//then after THAT, we multiply the light's color with the intensity and multiply that with the material's specular
		specular = material.specular * intensity;
	}
	//at the very end, we add all the values together to get the final color
	//this is awful and now i hate phong shading.
	return ambient + diffuse + specular;
}

//ctrl+c ctrl+v
void main()
{
	vec4 texcolor = texture(tex, ftexcoord);
	ocolor = texcolor * vec4(ads(fposition,fnormal), 1);
}