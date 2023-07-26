### WSI Platform

平台是窗口系统、操作系统等的抽象表示。一些示例包括`MS Windows、Android`和`Wayland`。`Vulkan API`可以以独特的方式集成到每个平台中。<br>
`Vulkan API`不定义任何类型的平台对象。特定于平台的`WSI`扩展被定义为包含特定于平台的WSI函数的扩展。使用这些扩展受预处理器符号的保护，这些符号在[`Window System-Specific Header Control`][Window System-Specific Header Control]附录中定义。<br>

为了编译一个应用程序以在特定平台上使用`WSI`，必须采取以下之一的方法：
- 在包含`vulkan.h`头文件之前，定义适当的预处理器符号。
- 包含`vulkan_core.h`和任何本地平台头文件，然后再包含适当的特定于平台的头文件。<br>

预处理器符号和特定于平台的头文件在[`Window System Extensions and Headers`][Window System Extensions and Headers]表中定义。

每个特定于平台的扩展都是实例扩展。在使用它们之前，应用程序必须通过vkCreateInstance启用实例扩展。


[Window System-Specific Header Control]: https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap49.html#boilerplate-wsi-header "Window System-Specific Header Control"

[Window System Extensions and Headers]: https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap49.html#boilerplate-wsi-header-table "Window System Extensions and Headers"



