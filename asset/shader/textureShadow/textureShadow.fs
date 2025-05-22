#version 440

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform sampler2D texture0;
uniform sampler2D shadowMap;

uniform vec3 CamPos;

uniform int NumberLights;

struct LightInfo {
    vec4 Position;   // Light position (x, y, z) + w (Type of light)
    vec4 Colour;     // Light colour (r, g, b) + a (intensity)
};

layout(std140, binding = 0) buffer LightBuffer {
    LightInfo Light[];
};

struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shiness; // Specular shininess factor (phong exponent)
};
uniform MaterialInfo Material;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main() {
    vec3 base_color = texture(texture0, TexCoord).rgb;
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    vec3 ambient = vec3(0.0, 0.0, 0.0);

    float shadow = ShadowCalculation(FragPosLightSpace); 

    for (int i = 0; i < NumberLights; i++) {
        int type = int(Light[i].Position.w);
        vec3 pos = Light[i].Position.xyz;
        vec3 colour = Light[i].Colour.rgb;

        if (type == 0) { // Point light
            vec3 L = normalize(pos - Position);
            vec3 V = normalize(CamPos - Position);
            vec3 HalfwayVector = normalize(V + L);

            vec3 diffuse = Material.Kd * colour * max(dot(L, Normal), 0.0);
            vec3 specular = Material.Ks * colour * pow(max(dot(HalfwayVector, Normal), 0.0), Material.Shiness);
            // Apply shadow to diffuse and specular
            finalColor += (diffuse + specular) * (1.0 - shadow);
        } else if (type == 1) { // Ambient light
            ambient += Material.Ka * colour;
        }
    }
    finalColor += ambient;
    FragColor = vec4(finalColor, 1.0);
}
