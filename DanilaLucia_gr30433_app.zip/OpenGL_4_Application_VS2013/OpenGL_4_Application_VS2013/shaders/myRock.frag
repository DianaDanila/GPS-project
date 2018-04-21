#version 410 core
out vec4 FragColor;
in vec2 fragTexCoords;

uniform sampler2D diffuseTexture;

void main()
{
    FragColor = vec4(texture(diffuseTexture, fragTexCoords).rgb, 1.0f);
    //FragColor = vec4(vec3(0.11f, 0.56f, 1.0f) * texture(diffuseTexture, fragTexCoords).rgb, 1.0f);
}
