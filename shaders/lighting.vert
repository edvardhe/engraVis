#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 projection;
uniform vec3 lightDir;
uniform mat3 normalMatrix;

out vec3 normal;
out vec3 lightDirFrag;

void main()
{
    gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    normal = normalize(normalMatrix * aNormal);
    lightDirFrag = normalize(lightDir);
}