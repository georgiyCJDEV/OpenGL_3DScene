#shader vertex
#version 460
        
layout(location = 0) in vec3 vertex_position;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    gl_Position = (u_Projection * u_View * u_Model) * vec4(vertex_position,1.0f);
};

#shader fragment
#version 460

out vec4 color;

void main()
{
    color = vec4(1.0f);
};