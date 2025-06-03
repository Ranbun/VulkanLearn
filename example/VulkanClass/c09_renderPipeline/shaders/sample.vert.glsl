#version 450
layout(location = 0) out vec3 vertexColor;


vec3 position[3] = {vec3(0.0,0.5,0.0),
                    vec3(-0.5, -0.5, 0.0),
                    vec3(0.5,-0.5,0.0)};


vec3 color[3] = {vec3(0,0,1),vec3(0,1,0),vec3(0,0,1)};

void main()
{
    vertexColor = color[gl_VertexID];
    gl_Position = vec4(position[gl_VertexID], 1.0);
}
