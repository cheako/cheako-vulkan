#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPos;

#define MAX_BONES 64

layout (binding = 0) uniform UBO 
{
	mat4 model;
	mat4 bones[MAX_BONES];
	mat4 vp;
	vec3 lightPos;
	float pada;
	vec3 viewPos;
	float padb;
} ubo;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() {
	gl_Position = ubo.vp * ubo.model * vec4(inPos, 1.0);
}
