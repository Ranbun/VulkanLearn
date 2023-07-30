### WSI Surface

本地平台的表面或窗口对象通过表面对象进行抽象，这些表面对象由`VkSurfaceKHR`句柄来表示：


```C++
// Provided by VK_KHR_surface
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSurfaceKHR)

```

`VK_KHR_surface`扩展声明了`VkSurfaceKHR`对象，并提供了一个用于销毁`VkSurfaceKHR`对象的函数。不同的平台特定扩展为各自的平台提供了创建`VkSurfaceKHR`对象的函数。从应用程序的角度来看，这是一个不透明的句柄，就像其他`Vulkan`对象的句柄一样。

<p style="color: #707070" Size=10>
    <font color=red Size=4>Notes</font>:<br>
    &ensp;&ensp;在某些平台上，<code>Vulkan</code>加载器和<code>ICDs</code>（实现）可能采用一种约定，将句柄视为指向包含有关表面的平台特定信息的结构体指针。这将在加载器-ICD接口的文档和<code>LoaderAndTools</code>源代码仓库中的<code>vk_icd.h</code>头文件中进行描述。这不会影响加载器层接口；层可以封装<code>VkSurfaceKHR</code>对象。
 </p>

#### Android Platform 安卓

loading ......

#### Wayland Platform

`Wayland`平台是一种用于`Linux`系统的图形显示服务器协议，用于处理图形渲染和窗口管理。在`Vulkan`中，`Wayland`平台作为一种窗口系统集成（`WSI`）扩展，允许应用程序与`Wayland`显示服务器进行交互，并在`Wayland`窗口上进行渲染。 要在`Vulkan`中使用`Wayland`平台，应用程序需要包含`Wayland`特定的头文件，并在编译时启用相应的预处理器符号。使用`VK_KHR_wayland_surface`扩展，应用程序可以创建`VkSurfaceKHR`对象，用于表示与`Wayland`窗口相关的抽象表面。通过使用`Wayland`平台，`Vulkan`应用程序可以利用`Linux`系统的图形功能，并与`Wayland`窗口系统进行交互，从而实现高性能的图形渲染和窗口管理。

loading ......

#### Win32 Platform

要为Win32窗口创建一个VkSurfaceKHR对象，调用如下函数：

```C++

// Provided by VK_KHR_win32_surface
VkResult vkCreateWin32SurfaceKHR(
    VkInstance                                  instance,
    const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);

```

- `instance`,该表面关联的 `VkInstance` 实例。
- `pCreateInfo` 是一个指向 `VkWin32SurfaceCreateInfoKHR` 结构的指针，其中包含影响创建表面对象的参数。
- `pSurface` 是一个指向 `VkSurfaceKHR` 句柄的指针，用于返回创建的表面对象。


- 注意，为了使用该函数，你需要在编译时启用`VK_KHR_win32_surface`扩展，并在创建`VkInstance`实例时确保该扩展被启用。


&ensp;&ensp;在 `Win32` 平台上，`minImageExtent`、`maxImageExtent` 和 `currentExtent` 必须始终等于窗口的大小。
&ensp;&ensp;`Win32` 表面的 `currentExtent` 必须同时满足宽度和高度大于 0，或者它们都为 0。

<p style="color: #707070" Size=10>
    <font color=red Size=4>Notes</font>:<br>
    &ensp;&ensp;由于上述限制，在此平台上只能创建一个 <code>imageExtent</code> 等于窗口当前大小的新交换链，该大小由 <code>VkSurfaceCapabilitiesKHR::currentExtent</code> 报告。<br>
&ensp;&ensp;在此平台上，窗口大小可能会变为 (0, 0)（例如当窗口最小化时），因此在大小发生变化之前，无法创建交换链
 </p>

#### XCB Platform

loading ......

#### XCB Platform

loading .....

#### 与平台无关的信息 Platform-Independent Information

&ensp;&ensp;一旦创建，`VkSurfaceKHR`对象可以在本扩展以及其他扩展中使用，尤其是在`VK_KHR_swapchain`扩展中。

&ensp;&ensp;在`WSI`中，有几个函数会在`Surface`不再可用时返回`VK_ERROR_SURFACE_LOST_KHR`错误。在这种错误发生后，应该销毁`Surface`（以及任何子`Swapchain`，如果存在），因为没有办法将它们恢复到非丢失状态。应用程序可以尝试使用相同的本地平台窗口对象创建一个新的`VkSurfaceKHR`，但是否成功取决于平台，并可能取决于`Surface`不可用的原因。丢失的`Surface`不会导致设备丢失。

&ensp;&ensp;销毁VkSurfaceKHR对象，调用以下函数：
```C++
// Provided by VK_KHR_surface
void vkDestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator);

```
- `instance`是用于创建该表面的实例。
- `surface`是要销毁的表面。
- `pAllocator`是在没有更具体的分配器可用时用于为表面对象分配主机内存的分配器（请参阅内存分配）。

&ensp;&ensp;销毁`VkSurfaceKHR`仅仅断开了`Vulkan`与本地表面之间的连接，并不意味着销毁本地表面、关闭窗口或产生类似的行为。
















