#version 440 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform vec4 color;
void main()
{
    vec3 base_color = texture(texture0, TexCoord).rgb;
    vec4 finalColor = color / vec4(255.0, 255.0, 255.0, 255.0);
    FragColor = vec4(finalColor.rgb * base_color, finalColor.a);
}