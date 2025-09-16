#version 330 core
out vec4 FragColor;
in vec3 vertexPos;
in float zRelative;

void main()
{
    // shallow = white, deep = red, zRelative in [0,1]
    vec3 col = mix(vec3(1.0), vec3(1.0, 0.0, 0.0), zRelative);
    FragColor = vec4(col, 1.0);
}