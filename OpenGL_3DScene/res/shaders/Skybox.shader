#shader vertex
#version 460

layout (location = 0) in vec3 vertex_position;

out vec3 TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    vec4 pos = u_Projection * u_View * vec4(vertex_position,1.0f);
    gl_Position=vec4(pos.x,pos.y,pos.w,pos.w);
    TexCoords = vec3(vertex_position.x,vertex_position.y,-vertex_position.z);
}

#shader fragment
#version 460

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
}