## 呈现队列

具体的Vulkan 实现可能对窗口系统进行了支持，但这并不意味着所有平台的Vulkan 实现都支持同样的特性。需要确保物理设备可以在创建的表面上显示图像。

- 支持创建呈现队列
    - 可以在指定表面(`surface`)显示图像，调用一下函数查询支持。

```C++

// Provided by VK_KHR_surface
VkResult vkGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported);


```

- `physicalDevice`查询的物理设备
- `queueFamilyIndex`队列族索引
    -  队列族索引从`0`开始,依次`++`
- `surface`需要在上面呈现图像的表面
- `pSupported`返回结果 - `true or false`







