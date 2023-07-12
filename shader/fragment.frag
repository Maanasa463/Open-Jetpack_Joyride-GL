#version 330 core
in vec2 TexCoords;
out vec4 color;
// out vec4 textcolor;

uniform sampler2D image;
uniform vec3 spriteColor;

// uniform sampler2D text;
// uniform vec3 textColor;

void main()
{
    // vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
    // textcolor = vec4(textColor, 1.0) * sampled;

}