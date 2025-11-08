## Create Surface

`kSurfaceKHR` 对象是平台无关的 ,但它的创建依赖窗口系统。在`Windows`系统上，它的创建需要`HWND`和`HMODULE`。存在一个叫做`VK_KHR_win32_surface` 的`Windows` 平台特有扩展，用于处理与`Windows` 系统窗口交互有关的问题，
### Windows
必须同时支持`VK_KHR_surface` & `VK_KHR_win32_surface`扩展

调用函数`vkCreateWin32SurfaceKHR`创建`win32`平台的`surface`:

```C++
// Provided by VK_KHR_win32_surface
VkResult vkCreateWin32SurfaceKHR(
    VkInstance                                  instance,
    const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);

```
- `instance`使用的实例
- `pCreateInfo`需要填写的结构体用于创建
- `pSurface` 接受创建的`surface`


<font color=red>注: `vkCreateWin32SurfaceKHR`函数 需要手动加载 </font>

##### VkWin32SurfaceCreateInfoKHR defined as
```C++
// Provided by VK_KHR_win32_surface
typedef struct VkWin32SurfaceCreateInfoKHR {
    VkStructureType                 sType;
    const void*                     pNext;
    VkWin32SurfaceCreateFlagsKHR    flags;
    HINSTANCE                       hinstance;
    HWND                            hwnd;
} VkWin32SurfaceCreateInfoKHR;

```

#### 销毁`Surface`

- 创建的`surface`对象需要手动销毁

```C++

// Provided by VK_KHR_surface
void vkDestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator);

```
- 必须在实例被销毁之前销毁`surface`



