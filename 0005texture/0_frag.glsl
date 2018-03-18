#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 1) uniform sampler2D samplerColorMap;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inViewVec;
layout (location = 3) in vec3 inLightVec;

layout (location = 0) out vec4 outFragColor;

void main() 
{
    vec4 color = texture(samplerColorMap, inUV);

    vec3 normal = normalize(inNormal);
    vec3 surfaceToLight = normalize(inLightVec);
    vec3 surfaceToCamera = normalize(inViewVec);

    //ambient
    vec3 ambient = color.rgb / 150.0;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * color.rgb / 15.0;

    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), 8);
    vec3 specular = specularCoefficient * vec3(255,255,255);

    //attenuation
    float distanceToLight = length(inLightVec);
    float attenuation = 1.0 / (1.0 + 0.2 * pow(distanceToLight, 2));

    //linear color (color before gamma correction)
    vec3 linearColor = ambient + attenuation*(diffuse + specular);

    //final color (after gamma correction)
    vec3 gamma = vec3(1.0/2.2);

    outFragColor = vec4(pow(linearColor, gamma), color.a);
}
