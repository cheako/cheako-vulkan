#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec4 inBoneWeights;
layout (location = 4) in ivec4 inBoneIDs;

#define MAX_BONES 64

layout (binding = 0) uniform UBO 
{
	mat4 model;
	mat4 bones[MAX_BONES];
} ubo;

layout (binding = 2) uniform SUBO 
{
	mat4 vp;
	vec3 lightPos;
	float pada;
	vec3 viewPos;
	float padb;
} subo;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outViewVec;
layout (location = 3) out vec3 outLightVec;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() {
	mat4 boneTransform = ubo.bones[inBoneIDs[0]] * inBoneWeights[0];
	boneTransform     += ubo.bones[inBoneIDs[1]] * inBoneWeights[1];
	boneTransform     += ubo.bones[inBoneIDs[2]] * inBoneWeights[2];
	boneTransform     += ubo.bones[inBoneIDs[3]] * inBoneWeights[3];	

	outUV = inUV;

	mat4 skinPos = ubo.model * boneTransform;
	gl_Position = subo.vp * skinPos * vec4(inPos, 1.0);

	vec4 pos = skinPos * vec4(inPos, 1.0);
	outNormal = inverse(transpose(mat3(skinPos))) * inNormal;
	outLightVec = subo.lightPos - pos.xyz;
	outViewVec = subo.viewPos - pos.xyz;
}
