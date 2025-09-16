#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;
uniform float zMin;
uniform float zMax;

out float zRelative;

void main()
{
    zRelative = (aPos.z - zMin) / (zMax - zMin);
    gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}