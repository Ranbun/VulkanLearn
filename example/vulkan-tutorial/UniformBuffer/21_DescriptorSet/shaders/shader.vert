#version 450

layout(location=0) in vec2 inPosition;
layout(location=1) in vec3 inColor;
layout(location=0) out vec3 fragColor;

// uniform buffer
layout(binding=0) uniform UniformBufferObject
{
    /// 对齐要求
    // 标量必须按照N对齐， 对于float - 浮点类型，N == 4字节
    vec2 foo;    // vec2 按照8字节对齐 -- 2N
    // vec3 , vec4 按照16字节对齐 -- 4N
    // 对于mat4 glsl 要求必须16字节对齐  -- 所以读取的时候必须是16的倍数开头的
    mat4 model;  // 4 * 4 * 4N 字节对齐
    mat4 view;
    mat4 proj;
} ubo;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}
