# Setup Vulkan

整个初始化过程可以被视为一个多级漏斗：

- 实例创建： 过滤系统环境（操作系统版本、驱动程序存在性）。

- 物理设备枚举： 过滤硬件存在性。

- 特性/扩展查询： 过滤硬件兼容性（筛选算法的核心）。

- 逻辑设备创建： 实例化最终过滤出的资源集合。


## 1. Instance

- [VkInstance](./VKInstance.md)


## 2. Validation layers

#### [Validation layers](./Validation_layers.md)

在 Vulkan 的早期版本（1.0）中，规范区分了“实例层”（Instance Layers）和“设备层”（Device Layers）。这在理论上允许开发者仅针对特定的设备启用某些调试功能。然而，这种区分在实践中被证明过于复杂且收益甚微。

- 当前架构： 现行的 Vulkan 规范已经废弃了设备层的概念。现在，层只能在 实例 级别启用 。一旦在 VkInstance 创建时启用了某个层，它将自动应用到从该实例创建的所有物理设备和逻辑设备上。

- 向后兼容性： 尽管 VkDeviceCreateInfo 结构体中仍然保留了 ppEnabledLayerNames 字段，但在现代加载器中，该字段会被忽略。最佳实践要求开发者始终通过 VkInstanceCreateInfo 来管理所有层的启用 。


## 3. Physical Device

- [Physical Device](./Physical_Device.md)

### Queue Family

Vulkan 设备暴露了“队列族”，即一组具有相同能力的指令队列。主要类型包括图形（Graphics）、计算（Compute）、传输（Transfer）和稀疏绑定（Sparse Binding）。

- 硬性要求： 绝大多数渲染应用都需要至少一个支持 VK_QUEUE_GRAPHICS_BIT 的队列族。

- 呈现支持（Presentation Support）： 这是一个极其微妙的陷阱。支持图形渲染的队列族不一定支持将图像呈现到屏幕上（WSI）。应用程序必须针对每个队列族调用 vkGetPhysicalDeviceSurfaceSupportSupportKHR。

- 筛选逻辑： 应用程序必须找到一个物理设备，该设备包含：

    1. 一个图形队列族索引。

    2. 一个呈现队列族索引。

- 优化策略： 理想情况下，这两个索引应该是相同的。这避免了在图形队列和呈现队列之间进行昂贵的图像所有权转移（Ownership Transfer）。如果设备强制要求不同的队列族（这种情况在现代硬件上很少见，但在某些旧的嵌入式设备上可能存在），开发者必须处理 VK_SHARING_MODE_CONCURRENT 或显式的 VK_SHARING_MODE_EXCLUSIVE 屏障 。



## 4. LogicDevice
- [Physical Device](./Logic_Device.md)

### 物理与逻辑的映射

- 菜单与订单： 物理设备是硬件提供的“菜单”，列出了所有可能的菜品（特性）；逻辑设备是应用程序下的“订单”，明确指定了今天要吃什么。你不能点菜单上没有的菜（不支持的特性），菜单也不会因为你没点某道菜而改变（物理设备不可变）。
- 多设备协同： 一个 VkInstance 可以作为多个 VkDevice 的父节点。这在异构计算场景中尤为重要，例如使用集成显卡处理 UI 和视频解码，同时使用独立显卡进行 3D 渲染。


### Queue

## 5. Extension
- [Extension](./Extension.md)


