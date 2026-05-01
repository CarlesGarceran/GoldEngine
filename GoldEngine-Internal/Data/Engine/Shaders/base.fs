#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D albedoMap;
uniform vec4 baseColor;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(albedoMap, fragTexCoord);

    // NOTE: Implement here your fragment shader code

    finalColor = texelColor*baseColor;
}