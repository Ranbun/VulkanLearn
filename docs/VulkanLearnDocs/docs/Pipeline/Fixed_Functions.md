# Fixed Functions

- Pipeline 除了可编程的部分就剩下固定功能部分了
固定功能部分是指运行固定算法的部分，应用程序只能对此进行输入，然后等待他的输出传递到下一阶段。

## 固定阶段

- Vertex input
    - 输入的顶点定义

- Input assembly
    - 图元装配 - 将顶点组装成渲染的图元

- Viewports and scissors
    - 省略

- Rasterizer
    - 光栅化阶段

- Multisampling
    - 多重采样

- Depth and stencil testing
    - 深度测试&模板测试

- Color blending
    - 颜色混合

- Pipeline layout
    - 管线布局

### Pipeline layout

我觉得有必要详细记录下PipeLine layout这个东西：
[PipeLine Layout](./PipeLineLayout/pipelinelayout.md)

基本步骤:
- VkPipelineLayout 在创建管线的时候约定好，我将来给的资源一定是这样的，除此之外， Shader也一定需要这样的资源
- 根据约定去申请固定大小内存
- 设置申请的内存的值
- 绑定


总之`PipelineLayout`是一个相当复杂的东西，倒不是他复杂，他整个的衍生关系是相当庞大的，我认为在此处的学习最好了解vulkan的设计理念，你会知道他为什么要这样做。


