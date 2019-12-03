#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
//in float sampleExtraOutput;
in vec3 fragPos;
in vec3 fragNormal;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear;
};

//uniform vec3 color;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform vec3 color;
uniform float norm_factor;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (material.diffuse) * diff;
    
    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (material.specular) * spec;
    
    //Normal
    vec3 norm_color = (normalize(fragNormal) + 1 )/ 2;
    
    float attenuation = 1.0f / (length(light.position - fragPos) * light.linear);
    vec3 result = (ambient + diffuse + specular) * attenuation * (1.0f - norm_factor) + norm_color*norm_factor;
    //fragColor = vec4(color, 1.0f);
    fragColor = vec4(result, 1.0f);
}
