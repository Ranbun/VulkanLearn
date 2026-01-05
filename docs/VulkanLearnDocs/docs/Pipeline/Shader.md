# Shader

- Shader - glsl
- Shader Module
- Pack as VkPipelineShaderStageCreateInfo Ins
- Create Pipeline

## Shader

- Vulkan 使用glsl 来编写shader, 使用过OpenGL的应该是对此有了解，vulkan的shader和OpenGL的shader没有什么太大的区别;
- Vulkan 规定shader必须以字节码读取，这就是他和OpenGL的其中一个区别; 当然这是Khronos的行为,限制了GPU厂商对于glsl语法翻译的灵活性，但是：
- 极大的增加了shader 程序的兼容性
- 减小了GPU厂商开发的复杂程度
- 编译使用 glslangValidator程序:
    - 可以提前编译好之后然后在读取编译好的文件
    - 讲应用程序作为库包含，然后在运行的过程中编译这个着色器代码为字节码

- 其他的就和写OpenGL的一样的

## Shader Model

- 如果我们认为可读的glsl是高级的语法状态，那么从可读glsl到pipeline可以直接运行的状态就是逐步退化的过程

### SPIR-V
- glsl -> spir-v字节码
- spir-v字节码 是一种中间表示，对于驱动来说，他是一个可以操作的程序对象

### Shader Model
- 他是在 spir-v基础上进行校验的版本，简单是就是通过了语法树的检查
- 做一些提前编译
- 记录程序信息
    - 入口
    - 接口
    - 资源
    - ......

vulkan 前期的繁琐都是为了更大程度的优化，在整个过程里面，创建Shader Model的好处是可以提前发现问题以及让pipeline创建高效




