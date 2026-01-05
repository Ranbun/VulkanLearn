# RenderPass

- 对于`Pipeline`来说， `renderpass`是一套规则，规定了渲染结果的输出的所有信息。
它是“场景的上下文”：它不包含具体的“画三角形”的指令（那是 Pipeline 干的事），它定义了**“我在哪里画”以及“画板的生与死”**。

## SubPass

- 基本上一个pipeline就会对应一个SubPass;
- 一个renderpass 可以有多个pipeline;
- 一个 Pipeline 在创建时，必须明确声明它属于哪个 Render Pass 的哪个Subpass;

```C++

vkCmdBeginRenderPass(cmdBuf, &renderPassBeginInfo, ...); // 进入舞台

    // --- 第一场：画石头 ---
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineA);
    vkCmdBindDescriptorSets(...);
    vkCmdDraw(...);

    // --- 第二场：画玻璃 ---
    // 直接切换！不需要退出 Render Pass
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineB);
    vkCmdBindDescriptorSets(...);
    vkCmdDraw(...);

vkCmdEndRenderPass(cmdBuf); // 演出结束

```

如果是一个renderpass -> subpass 对应一个pipeline的情况

```C++

vkCmdBeginRenderPass(cmdBuf, ...); // 开始 Subpass 0

    // 绑定 Subpass 0 的 Pipeline
    vkCmdBindPipeline(cmdBuf, ..., pipelineGeometry);
    vkCmdDraw(...);

// 切换到下一个工序（Subpass 1）
vkCmdNextSubpass(cmdBuf, VK_SUBPASS_CONTENTS_INLINE);

    // 绑定 Subpass 1 的 Pipeline
    // 注意：这对 Shader 专门负责光照计算，输入是刚才画好的 G-Buffer
    vkCmdBindPipeline(cmdBuf, ..., pipelineLighting);
    vkCmdDraw(...);

vkCmdEndRenderPass(cmdBuf);

```

## RenderPass与 VKImage

```C++

// --- 1. RenderPass 定义 (规则) ---
VkAttachmentDescription attachments[2];
attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM; // 0号坑位：颜色
attachments[1].format = VK_FORMAT_D32_SFLOAT;     // 1号坑位：深度

VkRenderPassCreateInfo rpInfo = { .pAttachments = attachments, ... };
vkCreateRenderPass(device, &rpInfo, ..., &hRenderPass);

// --- 2. 具体的图片 (显存) ---
VkImageView myViews[2] = { colorImageView, depthImageView };

// --- 3. Framebuffer 产生联系 (粘合) ---
VkFramebufferCreateInfo fbInfo = {};
fbInfo.renderPass = hRenderPass; // 引用规则
fbInfo.attachmentCount = 2;
fbInfo.pAttachments = myViews;   // 【关键点】按顺序对应 Index 0 和 1
vkCreateFramebuffer(device, &fbInfo, ..., &hFramebuffer);
```C++



