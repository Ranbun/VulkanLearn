# PipeLine Layout

- PipeLine layout 是 Shader资源的描述, 像是Shader的参数列表。
- Pipeline Layout 是一个配置对象，它定义了图形管线能够访问的所有资源的“蓝图”。

我们先看看创建一个PipelineLayout的参数：

*VkPipelineLayoutCreateInfo*

```C++
typedef struct VkPipelineLayoutCreateInfo {
    VkStructureType                 sType;
    const void*                     pNext;
    VkPipelineLayoutCreateFlags     flags;
    uint32_t                        setLayoutCount;
    const VkDescriptorSetLayout*    pSetLayouts;
    uint32_t                        pushConstantRangeCount;
    const VkPushConstantRange*      pPushConstantRanges;
} VkPipelineLayoutCreateInfo;

```

*VkDescriptorSetLayoutCreateInfo*

```C++
typedef struct VkDescriptorSetLayoutCreateInfo {
    VkStructureType                        sType;
    const void*                            pNext;
    VkDescriptorSetLayoutCreateFlags       flags;
    uint32_t                               bindingCount;
    const VkDescriptorSetLayoutBinding*    pBindings;
} VkDescriptorSetLayoutCreateInfo;

```

*VkDescriptorSetLayoutBinding*

```C++

typedef struct VkDescriptorSetLayoutBinding {
    uint32_t              binding;
    VkDescriptorType      descriptorType;
    uint32_t              descriptorCount;
    VkShaderStageFlags    stageFlags;
    const VkSampler*      pImmutableSamplers;
} VkDescriptorSetLayoutBinding;

```

我们关注两个信息：

- pSetLayouts

- pPushConstantRanges， 此处先不关注这个

## VkDescriptorSetLayout
- pSetLayouts + setLayoutCount 描述需要绑定多少个 Set， 每个set 都有一个SetLayout。


## Pipeline layout And Shader

Shader 程序是怎么个Pipeline layout产生联系的呢 ？

### Shader
```glsl

// frag.shader
// 这就像是函数内部使用了两个全局变量

layout(set = 0, binding = 0) uniform UBO { mat4 viewProj; } camera;   // 全局数据
layout(set = 1, binding = 0) uniform sampler2D texSampler;            // 材质数据

void main() {
    // ... 具体的计算逻辑 ...
}

```

在CPU端，需要创建一个 Pipeline Layout来描述描述上面的Shader;

### pipeline layout

```C++

PipelineLayout = {
    Set_0: { binding_0: UniformBuffer }, // 对应 Shader 里的 set=0 -- VkDescriptorSetLayoutCreateInfo 对象描述
    Set_1: { binding_0: ImageSampler },  // 对应 Shader 里的 set=1 -- VkDescriptorSetLayoutCreateInfo 对象描述
    PushConstants: None
}

```

### Bind

记录 Command Buffer 进行绘制

```C++

// 告诉 GPU 使用哪个“签名”
vkCmdBindPipeline(..., graphicsPipeline);

// 传递具体的“实参”
// 这里的 layout 必须和创建 pipeline 时用的一样，也必须和 shader 里的 set 序号对应
vkCmdBindDescriptorSets(..., pipelineLayout, 0, &descriptorSetCamera);
vkCmdBindDescriptorSets(..., pipelineLayout, 1, &descriptorSetTexture);

vkCmdDraw(...);

```
这个时候就是实际传递数据的过程。


## Shader 资源

### GPU (Shader)

```glsl
// vertex_shader.vert
#version 450

// set=0: 表示这属于第 0 个描述符集
// binding=0: 表示这是该集里面的第 0 个绑定点
layout(set = 0, binding = 0) uniform MVPBuffer {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    // 使用 ubo.model ...
}

```

- `set=N`对应CPU端的VkDescriptorSet(描述符集)， 你可以把它看作一个“资源包”
    - 可以有多个binding
- `binding=M` 对应这个资源包里的 第M号插槽

Shader 声明：我要在第 0 号资源包的第 0 号插槽读数据。

### CPU

在 CPU 端，要让这个 set=0, binding=0 生效，需要经历三个阶段：定义布局 -> 分配集合 -> 填入数据（更新）。

#### Step 1
- 定义布局

```C++
// C++ 代码
VkDescriptorSetLayoutBinding uboLayoutBinding = {};
uboLayoutBinding.binding = 0; // 【关键】这里必须对应 Shader 里的 binding=0
uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // 类型必须对应 uniform
uboLayoutBinding.descriptorCount = 1; // 这是一个数组吗？不是，就是1个
uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // 只有顶点着色器能看到

// 创建 Layout 对象
VkDescriptorSetLayoutCreateInfo layoutInfo = {};
layoutInfo.bindingCount = 1;
layoutInfo.pBindings = &uboLayoutBinding;

VkDescriptorSetLayout descriptorSetLayout;
vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);

```

#### Step 2
- 分配 Set

```C++
VkDescriptorSetAllocateInfo allocInfo = {};
allocInfo.pSetLayouts = &descriptorSetLayout; // 使用上面的布局
// ...
vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
```

#### Step 3
- 填入具体资源

```C++
// 1. 准备好具体的显存 buffer 信息
VkDescriptorBufferInfo bufferInfo = {};
bufferInfo.buffer = myActualUniformBuffer; // 这是你通过 vkCreateBuffer 创建的实际 Buffer
bufferInfo.offset = 0;
bufferInfo.range = sizeof(UniformBufferObject);

// 2. 编写更新结构体
VkWriteDescriptorSet descriptorWrite = {};
descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
descriptorWrite.dstSet = descriptorSet; // 对应 Shader 里的 set=0 (这是刚才分配的那个对象)
descriptorWrite.dstBinding = 0;         // 【关键】对应 Shader 里的 binding=0
descriptorWrite.dstArrayElement = 0;    // 数组索引，非数组就是0
descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
descriptorWrite.descriptorCount = 1;
descriptorWrite.pBufferInfo = &bufferInfo; // 指向实际的数据

// 3. 执行更新：此时连接建立！
vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
```

#### 总结

- Binding = 单个插槽（口袋）。
- DescriptorSetLayout = 一个资源包的结构（一个背包的设计图，背包里可以有多个口袋）。
- Pipeline Layout = 整个人的装备清单（这个人背了几个背包）。

1. 最底层：VkDescriptorSetLayoutBinding (单个插槽)
- 这是最小单位。它描述了某一个坑位。
    - 比如：binding = 0 是一个 UBO。
    - 比如：binding = 1 是一个纹理。

2. 中间层：VkDescriptorSetLayout (单套资源的布局)
- 这是集合的定义。它把上面的 Binding 组合在一起。
    - 它描述了 Set N 长什么样。
    - 代码体现：创建它时，你传入一个 VkDescriptorSetLayoutBinding 的数组。
    - 一个 Layout 包含 多个 Binding。

3. 最顶层：VkPipelineLayout (整个管线的布局)
- 这是全局定义。它把上面的 Layout 组合在一起。
    - 它描述了整个管线用了多少个 Set（Set 0, Set 1...）。
    - 代码体现：创建它时，你传入一个 VkDescriptorSetLayout 的数组。
    - 一个 Pipeline Layout 包含 多个 DescriptorSetLayout。


- `DescriptorLayout` 与 `VkDescriptorSet`是一种 1 : N关系
- `vkAllocateDescriptorSets` 只是一个申请内存的过程，实际来说可以通过`vkUpdateDescriptorSets`函数对每一个`VkDescriptorSet`填入不同的数据，且完全独立开的。

