#version 330 core
out vec4 FragColor;
in vec3 vertexPos;
in vec3 normal;
in vec3 lightDirFrag;

void main()
{
    vec3 col = abs(dot(normal, lightDirFrag)) * vec3(1.0, 1.0, 1.0);
    FragColor = vec4(col, 1.0);
}