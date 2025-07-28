#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    float sampled = texture(text, TexCoords).r;
    float dropShadow = texture(text, TexCoords - vec2(0.0, 0.15)).r;
    color = vec4(textColor, sampled);
    if(sampled < 0.1)
        color = vec4(1 - textColor, dropShadow);
} 