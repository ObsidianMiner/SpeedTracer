#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}