#version 440

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 lightSpaceMatrix;

void main()
{
    Normal = normalize(NormalMatrix * VertexNormal);
    Position = (ModelMatrix * VertexPosition).xyz;
    TexCoord = VertexTexCoord;
    FragPosLightSpace = lightSpaceMatrix * vec4(Position, 1.0);
    gl_Position = MVP * VertexPosition;
}
