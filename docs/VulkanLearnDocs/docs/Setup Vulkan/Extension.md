# Extension

扩展是 Vulkan 在不破坏核心 API 兼容性的前提下引入新功能的机制。它们允许硬件厂商（NVIDIA, AMD...）暴露新的硬件特性（如 Ray Tracing, Mesh Shaders），或者允许 Khronos 组织在将特性并入核心规范前进行标准化测试。

## 实例扩展(Instance Extensions)

修改加载器行为或处理与操作系统及显示系统的交互。

- 典型用例： VK_KHR_surface（抽象表面创建），VK_KHR_win32_surface / VK_KHR_xcb_surface（平台特定表面创建），VK_EXT_debug_utils（高级调试回调）。

- 作用范围： 影响整个应用程序的初始化、物理设备枚举以及全局回调。必须在 vkCreateInstance 时启用。


## 设备扩展（Device Extensions）

扩展逻辑设备和 GPU 驱动程序的能力，直接关联到渲染和计算功能。

- 典型用例： VK_KHR_swapchain（交换链图像呈现），VK_KHR_ray_tracing_pipeline（光线追踪），VK_EXT_memory_priority（内存优先级控制）。

- 作用范围： 仅影响启用了该扩展的逻辑设备。必须在 vkCreateDevice 时启用。

## 扩展的进化

Vulkan 特性通常遵循 EXT（多厂商实验性） -> KHR（Khronos 批准） -> Core（核心规范）的晋升路径


