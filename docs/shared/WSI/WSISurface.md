## 窗口表面

&ensp;&ensp;`Vulkan` 是一个平台无关的`API`，它不能直接和窗口系统交互。为了将`Vulkan` 渲染的图像显示在窗口上，我们需要使用`WSI(Window System Integration) `扩展。<br>

&ensp;&ensp;`VK_KHR_surface `是一个实例级别的扩展，它已经被包含在使用`glfwGetRequiredInstanceExtensions `函数获取的扩展列表中，所以，我们不需要自己请求这一扩展。`WSI` 扩展同样也被包含在`glfwGetRequiredInstanceExtensions`函数获取的扩展列表中，也不需要我们自己请求。由于窗口表面对物理设备的选择有一定影响，它的创建只能在`Vulkan`实例创建之后进行。

* [创建表面](./CreateSurface.md)
* [呈现队列](./QueuePresent.md)






