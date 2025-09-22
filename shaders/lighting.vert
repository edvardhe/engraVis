#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 projection;
uniform vec3 lightDir;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

out vec3 normal;
out vec4 fragPosLightSpace;
out vec3 lightDirFrag;

void main()
{
    gl_Position = projection * model * vec4(aPos.xyz, 1.0);
    normal = normalize(normalMatrix * aNormal);
    lightDirFrag = normalize(lightDir);
    fragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);
}