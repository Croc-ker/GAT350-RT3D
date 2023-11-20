#version 430

layout (location = 0) in vec3 vposition;

uniform mat4 model;
uniform mat4 lightProjection;


void main()
{
    gl_Position = lightProjection * model * vec4(vposition, 1.0);
}
