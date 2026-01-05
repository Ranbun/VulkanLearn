# Surface

由于 Vulkan 是一个平台无关的 API，它本身无法直接与窗口系统交互。为了建立 Vulkan 与窗口系统之间的连接， 并将结果呈现在屏幕上，我们需要使用 WSI（窗口系统集成）扩展 - `VK_KHR_surface`.

它公开了一个`VkSurfaceKHR`对象，该对象代表一种抽象的表面，用于显示渲染后的图像。 根据不同的平台需要加载不同的扩展以创建`VkSurfaceKHR`

Vulkan 本身的渲染调用不需要依赖窗口，这也是和OpenGL的区别所在。

SUrface 更像是一种描述信息(描述窗口具有的能力)，它决定的是：这个窗口能不能作为 Vulkan Present 的目标

实际的渲染载体并不是VkSurfaceKHR实例， 而是swapchain的这一组图像- Images, 我们的渲染的结果都是写入这些图像上的。

在渲染完成之后，通过呈现请求告诉窗口系统，我们要显示这个图像， WSI会完成后续的一切操作，实际作为vulkan开发人员的话你的工作到此结束。





