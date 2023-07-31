### Presenting Directly to Display Devices
- 直接向显示设备呈现

在某些环境中，应用程序可以直接向显示设备呈现（`Presenting Directly to Display Devices`），而无需使用中间的窗口系统。这对于嵌入式应用程序或使用`Vulkan`实现窗口系统的渲染/呈现后端非常有用。`VK_KHR_display`扩展提供了枚举显示设备和创建面向显示设备的`VkSurfaceKHR`对象所需的功能。

#### Display Enumeration 设备显示枚举

- 显示设备枚举（`Display Enumeration`）是通过`VK_KHR_display`扩展来实现的。该扩展允许应用程序查询系统中可用的显示设备，并获取有关这些设备的信息。要开始显示设备的枚举，应用程序需要创建一个`VkDisplayPropertiesKHR`结构体数组来存储查询到的显示设备信息。通过调用`vkGetPhysicalDeviceDisplayPropertiesKHR`函数，应用程序可以获取与给定物理设备相关联的所有显示设备的属性。此函数将填充`VkDisplayPropertiesKHR`结构体数组，并返回显示设备的数量。

显示设备通过`VkDisplayKHR`句柄进行表示。该句柄用于标识`Vulkan`中的特定显示设备。应用程序可以使用这些句柄来进行与显示设备相关的操作，比如创建表面对象（`VkSurfaceKHR`）以进行显示呈现。每个`VkDisplayKHR`句柄都与一个特定的显示设备相关联，它可以用于在`Vulkan API`中引用该显示设备。

```C++
// Provided by VK_KHR_display
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDisplayKHR)
```

要查询有关可用显示设备的信息，调用以下函数:

```C++
// Provided by VK_KHR_display
VkResult vkGetPhysicalDeviceDisplayPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPropertiesKHR*                     pProperties);

```
- `physicalDevice` is a physical device.
- `pPropertyCount` 是一个指向与可用或查询的显示设备数量相关的整数的指针.
- `pProperties`可以是`NULL`，也可以是指向`VkDisplayPropertiesKHR`结构数组的指针

##### Or

```C++
// Provided by VK_KHR_get_display_properties2
VkResult vkGetPhysicalDeviceDisplayProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayProperties2KHR*                    pProperties);

```

#### 显示平面（Display Planes）
显示平面（`Display Planes`）是 `Vulkan` 中用于将图像呈现在显示设备上的一种机制。显示平面允许图像直接呈现在显示设备的表面上，而无需使用窗口系统或交换链。显示平面是由 `VkDisplayPlaneKHR` 句柄表示的。您可以使用 `vkGetPhysicalDeviceDisplayPlanePropertiesKHR` 函数来查询物理设备上可用的显示平面。
显示平面的一种常见用途是在多个图层之上叠加显示内容，例如在一个视频播放器应用中将视频图像叠加在图形用户界面之上。通过使用显示平面，可以在不需要使用交换链的情况下实现这种叠加效果。要将图像呈现到显示平面上，可以使用 `vkQueuePresentKHR` 函数。您需要在调用此函数时指定用于显示平面的 `VkDisplayKHR` 句柄。


为了在显示设备上呈现图像，图像会被呈现到显示的各个平面上。物理设备必须至少支持显示的一个平面。平面可以叠加和混合，以在一个显示上组合多个图像。设备可以只支持固定的叠加顺序和平面与显示之间的固定映射，或者它们可以允许任意由应用程序指定的叠加顺序和平面与显示之间的映射。为了查询设备显示平面的属性，可以调用 `vkGetPhysicalDeviceDisplayPlanePropertiesKHR` 函数:

```C++

// Provided by VK_KHR_display
VkResult vkGetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlanePropertiesKHR*                pProperties);

```

##### or

```C++
// Provided by VK_KHR_get_display_properties2
VkResult vkGetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlaneProperties2KHR*               pProperties);

```
- `vkGetPhysicalDeviceDisplayPlaneProperties2KHR` 函数与 `vkGetPhysicalDeviceDisplayPlanePropertiesKHR` 函数类似，但可以通过链接的输出结构返回扩展信息。


要确定平面可用于哪些显示器，请调用以下函数：
```C++
// Provided by VK_KHR_display
VkResult vkGetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    planeIndex,
    uint32_t*                                   pDisplayCount,
    VkDisplayKHR*                               pDisplays);
```
- `planeIndex` 是应用程序希望使用的显示平面的索引，必须在范围 [0, 物理设备支持的显示平面数量 - 1] 内。
- `planeIndex`必须小于设备支持的显示平面数量，这可以通过调用`vkGetPhysicalDeviceDisplayPlanePropertiesKHR`来确定。

使用专门的查询函数可以查询显示的其他属性。


#### Display Modes 显示模式

显示模式是指显示设备支持的不同显示配置。每个显示模式包含显示器的分辨率、刷新率和其他相关信息。应用程序可以查询显示设备支持的所有显示模式，并根据需要选择最适合的模式来进行显示。

显示模式通过`VkDisplayModeKHR`句柄来表示:

```C++
// Provided by VK_KHR_display
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDisplayModeKHR)
```

每个显示器默认关联有一个或多个支持的显示模式。可以通过调用以下函数来查询这些内置模式:

```C++

// Provided by VK_KHR_display
VkResult vkGetDisplayModePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModePropertiesKHR*                 pProperties);

```

- `physicalDevice`是与显示器相关联的物理设备。
- `display`是要查询的显示器。

##### or
```C++

// Provided by VK_KHR_get_display_properties2
VkResult vkGetDisplayModeProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModeProperties2KHR*                pProperties);

```

`vkGetDisplayModeProperties2KHR`的行为类似于`vkGetDisplayModePropertiesKHR`，它可以通过链接的输出结构返回扩展信息。

可以通过调用vkCreateDisplayModeKHR来创建其他模式。
```C++

// Provided by VK_KHR_display
VkResult vkCreateDisplayModeKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    const VkDisplayModeCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDisplayModeKHR*                           pMode);

```
- `display`是要为其创建附加模式的显示器。
- `pCreateInfo`是一个指向`VkDisplayModeCreateInfoKHR`结构的指针，用于描述要创建的新模式的属性

##### Host Synchronization
主机对显示设备的访问必须进行外部同步控制。

希望直接向显示设备进行展示的应用程序必须选择要针对的显示层或"plane"，以及要在显示设备上使用的模式。每个显示设备至少支持一个`plane`。调用以下函数来确定给定模式和`plane`组合的能力：

```C++

// Provided by VK_KHR_display
VkResult vkGetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayModeKHR                            mode,
    uint32_t                                    planeIndex,
    VkDisplayPlaneCapabilitiesKHR*              pCapabilities);

```
- `physicalDevice`是与指定的显示设备关联的物理设备。
- `mode`是应用程序在使用指定平面时打算使用的显示模式。注意，此参数还隐式指定了一个显示设备。
- `planeIndex`是应用程序打算与显示设备一起使用的平面索引，其值应小于设备支持的显示平面数量。
- `pCapabilities`是一个指向`VkDisplayPlaneCapabilitiesKHR`结构的指针，用于返回平面的能力信息。

##### Host Synchronization
主机对显示设备的访问必须进行外部同步控制。

##### or
要查询给定模式和平面组合的能力信息，调用以下函数：
```C++

// Provided by VK_KHR_get_display_properties2
VkResult vkGetDisplayPlaneCapabilities2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkDisplayPlaneInfo2KHR*               pDisplayPlaneInfo,
    VkDisplayPlaneCapabilities2KHR*             pCapabilities);

```

`vkGetDisplayPlaneCapabilities2KHR`与`vkGetDisplayPlaneCapabilitiesKHR`的行为类似，可以通过链接的输入结构指定扩展输入，并通过链接的输出结构返回扩展信息。


#### Display Surfaces 显示表面
完整的显示配置包括一个模式（mode）、一个或多个显示平面（`display planes`），以及描述它们行为的任何参数，以及描述与这些平面相关联的图像的一些方面的参数。显示表面（`display surfaces`）描述完整显示配置中单个平面的配置。要为显示平面创建一个`VkSurfaceKHR`对象，调用`vkCreateDisplayPlaneSurfaceKHR`函数：

```C++

// Provided by VK_KHR_display
VkResult vkCreateDisplayPlaneSurfaceKHR(
    VkInstance                                  instance,
    const VkDisplaySurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);

```
- `instance`是对应于目标显示所在物理设备的`VkInstance`实例。
- `pCreateInfo`是一个指向`VkDisplaySurfaceCreateInfoKHR`结构的指针，用于指定要使用的模式、平面和其他参数.
- `pSurface`是一个指向`VkSurfaceKHR`句柄的指针，用于返回创建的表面对象。

<p> <font color=#707070>Notes:</font>
创建显示表面不能修改其命名的显示器、平面或其他资源的状态。例如，它不能应用指定的模式在关联的显示器上。应用显示配置是通过向显示表面进行呈现的副作用发生的
</p>


##### `Display Planes` & `VkDisplayKHR`
在 `Vulkan` 中，`Display Planes`（显示层面）和 `VkDisplayKHR` 是用于表示和控制显示设备的两个不同概念，它们之间有如下关系：

`Display Planes`（显示层面）：
显示层面是显示设备（显示器或屏幕）上的硬件组件，用于将来自多个源（图像）的内容合成到最终的显示输出中。每个显示设备通常有多个显示层面可供使用，它们可以独立地合成多个图像，并以特定的方式进行叠加显示。例如，一个显示层面可以用于合成2D界面元素，另一个用于视频播放，还可以有其他显示层面用于不同的图像组合。

`VkDisplayKHR`：
`VkDisplayKHR` 是 `Vulkan` 中表示显示设备的对象。它是一个由 `Vulkan` 提供的句柄类型，用于标识和访问物理设备上的显示设备。每个 `VkDisplayKHR` 对象表示一个显示设备，其中包含显示设备的属性信息，如名称、物理尺寸和支持的显示模式等。

关于 `Display Planes` 与 `VkDisplayKHR` 之间的关系：
`VkDisplayKHR` 表示单个显示设备：每个 `VkDisplayKHR` 对象代表物理设备上的一个显示设备，例如一个显示器或屏幕。
显示设备可以有多个 `Display Planes`：每个显示设备通常都有多个硬件显示层面，用于将来自不同源的图像合成为最终的显示输出。
`VkDisplayKHR` 通过 `VkDisplayPlanePropertiesKHR` 提供显示层面信息：使用 `vkGetPhysicalDeviceDisplayPropertiesKHR` 函数，你可以获取物理设备上支持的显示设备列表。对于每个显示设备，你可以使用 `vkGetDisplayPlanePropertiesKHR` 函数获取其支持的显示层面属性。

与特定显示层面相关联的表面：通过 `vkCreateDisplayPlaneSurfaceKHR` 函数，你可以创建一个 `VkSurfaceKHR` 对象，该对象与指定的显示层面相关联。这个表面可以用于将渲染结果直接呈现到该层面上。

总结来说，`Display Planes` 是显示设备上的硬件组件，用于将多个图像合成为最终显示输出的显示层面。而 `VkDisplayKHR` 是 `Vulkan` 中表示显示设备的对象，通过它可以获取显示设备的属性信息，并通过 `VkDisplayPlanePropertiesKHR` 获取特定显示设备上支持的显示层面信息。通过创建与特定显示层面相关联的表面，你可以在 `Vulkan `中实现将渲染结果直接呈现到指定显示层面的操作。













