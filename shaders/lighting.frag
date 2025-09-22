#version 330 core
out vec4 FragColor;
in vec3 vertexPos;
in vec3 normal;
in vec3 lightDirFrag;
in vec4 fragPosLightSpace;

uniform sampler2D shadowMap;

void main()
{       
    vec3 shadowCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
    shadowCoord = shadowCoord * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, shadowCoord.xy).r; 
    float currentDepth = shadowCoord.z;
    float shadow = currentDepth > closestDepth + 0.005 ? 1.0 : 0.0;
    vec3 col = abs(dot(normal, lightDirFrag)) * vec3(1.0, 1.0, 1.0);
    FragColor = vec4(col * (1.0 - shadow), 1.0);
}