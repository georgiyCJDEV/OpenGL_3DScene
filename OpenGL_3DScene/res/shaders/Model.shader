#shader vertex
#version 460
        
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 vertexAmbient;
layout(location = 4) in vec3 vertexDiffuse;
layout(location = 5) in vec3 vertexSpecular;
layout(location = 6) in float vertexOpacity;

out vec3 Normal;
out vec3 FragPos;

out vec2 TexCoords;

out vec3 FragAmbient;
out vec3 FragDiffuse;
out vec3 FragSpecular;

out float FragOpacity;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    gl_Position = (u_Projection * u_View * u_Model) * vec4(position, 1.0f);
    FragPos = vec3(u_Model * vec4(position, 1.0f));
    Normal = normalize(vec3(transpose(inverse(u_Model)) * vec4(normal,1.0f)));
    TexCoords = texCoords;

    FragAmbient=vertexAmbient;
    FragDiffuse=vertexDiffuse;
    FragSpecular=vertexSpecular;

    FragOpacity=vertexOpacity;
};

#shader fragment
#version 460

struct Material
{
    sampler2D texDiffuse;
    sampler2D texSpecular;
    float shininess;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 direction;
    vec3 position;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 color;

in vec3 FragPos;
in vec3 Normal;

in vec2 TexCoords;

in vec3 FragAmbient;
in vec3 FragDiffuse;
in vec3 FragSpecular;

in float FragOpacity;

const int NUMBER_OF_POINT_LIGHTS = 11;

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform DirLight u_DirLight;
uniform PointLight u_PointLight[NUMBER_OF_POINT_LIGHTS];
uniform SpotLight u_SpotLight;

vec3 calcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 _texDif, vec3 _texSpec );
vec3 calcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 _texDif, vec3 _texSpec );

void main()
{
    vec3 norm = normalize( Normal );
    vec3 viewDir = normalize ( u_ViewPos - FragPos );
    
    vec3 texDif = vec3(texture(u_Material.texDiffuse, TexCoords));
    vec3 texSpec = vec3(texture(u_Material.texSpecular, TexCoords));

    vec3 result;
    for(int i =0; i<NUMBER_OF_POINT_LIGHTS; i++)
    {
        result += calcPointLight(u_PointLight[i], norm, FragPos, viewDir, texDif, texSpec);
    }

    color = vec4(result,FragOpacity);
};

vec3 calcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 _texDif, vec3 _texSpec )
{
    vec3 lightDir = normalize( light.position - fragPos );

    float diff = max( dot( normal,lightDir ), 0.0 );

    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), u_Material.shininess );

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );

    vec3 ambient  = FragAmbient * _texDif * light.ambient * attenuation;
    vec3 diffuse  = FragDiffuse * _texDif * light.diffuse * diff * attenuation;
    vec3 specular = FragSpecular * _texSpec * light.specular * spec * attenuation;

    return ( ambient + diffuse + specular );
}

vec3 calcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 _texDif, vec3 _texSpec )
{
    vec3 lightDir = normalize( light.position - fragPos );

    float diff = max( dot( normal,lightDir ), 0.0 );

    vec3 reflectDir = reflect( -lightDir, normal ); 
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), u_Material.shininess );

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * (distance * distance ) );

    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( theta - light.outerCutOff ) / epsilon, 0.0, 1.0 );

    vec3 ambient =  FragAmbient * _texDif * light.ambient * attenuation * intensity;
    vec3 diffuse =  FragDiffuse * _texDif * light.diffuse * diff * attenuation * intensity;
    vec3 specular = FragSpecular * _texSpec * light.specular * spec * attenuation * intensity;

    return ( ambient + diffuse + specular );
}