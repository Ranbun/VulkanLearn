#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject   /// set 描述在那个描述符集  binding 表示的是第几个资源
{
    mat4 modelview;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 vertexColor;
layout(location = 1) out vec2 vertexUV;

void main()
{
    vertexColor = color;
    vertexUV = uv;
    gl_Position = ubo.proj * ubo.modelview * vec4(position, 1.0);
}
