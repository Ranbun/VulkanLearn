### WSI Swapchain 交换链

![SwapChain](../resources/WSI/WSI_swapChain.png)


交换链对象（也称为交换链）提供了向表面呈现渲染结果的能力。交换链对象通过`VkSwapchainKHR`句柄来表示:

```C++

// Provided by VK_KHR_swapchain
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSwapchainKHR)

```

交换链是与表面相关联的可呈现图像数组的抽象表示。这些可呈现图像由平台创建的`VkImage`对象来表示。每次只显示一张图像（对于多视图/立体3D表面可能是图像数组），但是可以将多个图像排队等待呈现。应用程序将渲染内容绘制到图像上，然后将图像排队提交以呈现到表面上。

在同一时间内，一个本地窗口不能与多个未废弃（non-retired）的交换链关联。此外，不能为与非`Vulkan`图形`API`表面相关联的本地窗口创建交换链。

<p style="color: #707070" Size=10>
    <font color=red Size=4>Notes</font>:<br>
    &ensp;&ensp;展示引擎是对平台的合成器或显示引擎的抽象。
展示引擎可能相对于应用程序和/或逻辑设备是同步或异步的。
一些实现可能使用设备的图形队列或专用的展示硬件来执行展示操作。
 </p>

在交换链中，可呈现图像由展示引擎所拥有。应用程序可以通过调用`vkAcquireNextImageKHR`获取可呈现图像的使用权。应用程序必须在可呈现图像通过`vkAcquireNextImageKHR`返回后、通过`vkQueuePresentKHR`释放之前，进行图像的使用操作，包括转换图像布局和提交渲染命令等操作。

通过调用`vkAcquireNextImageKHR`，应用程序可以获取可呈现图像的使用权。在获取可呈现图像后且在对其进行修改之前，应用程序必须使用同步原语来确保展示引擎已经完成对图像的读取。然后，应用程序可以对图像的布局进行转换，将渲染命令提交到图像上等操作。最后，应用程序使用`vkQueuePresentKHR`呈现图像，这将释放对图像的使用权。

展示引擎控制着获取可呈现图像的顺序，以供应用程序使用。

- 在`Vulkan`中，"presentation engine"（展示引擎）是指负责将渲染结果呈现到屏幕或显示设备上的部分。它是`Vulkan`与窗口系统或显示设备交互的中间层。展示引擎可以是平台的合成器或显示引擎，负责接收应用程序渲染的图像，并在适当的时机将其呈现到屏幕上。展示引擎可以是同步的或异步的，具体取决于实现和硬件。有些实现会使用设备的图形队列或专用的展示硬件来执行呈现操作。对于每个交换链，都会有一个相应的展示引擎与之关联，管理交换链中的可呈现图像。应用程序通过与展示引擎交互，使用`vkAcquireNextImageKHR`获取可呈现图像的使用权，并通过`vkQueuePresentKHR`将图像提交给展示引擎进行呈现。展示引擎负责处理图像的呈现，以确保在正确的时机显示渲染结果到屏幕上。

<p style="color: #707070" Size=10>
    <font color=red Size=4>Notes</font>:<br>
    &ensp;&ensp;这使得平台可以处理需要在呈现后无序返回图像的情况。同时，它允许应用程序在初始化时生成引用交换链中所有图像的命令缓冲区，而不是在其主循环中生成。
 </p>

下面将描述这一切是如何工作的。

如果使用`VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR`或`VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR`创建交换链（`swapchain`），将获得一个称为共享呈现图像的单个可呈现图像。共享呈现图像允许应用程序和呈现引擎同时访问，而无需在初始呈现后对图像的布局进行转换。

- 使用`VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR`模式时，呈现引擎仅在完成呈现后才需要更新共享可呈现图像的最新内容。应用程序必须调用`vkQueuePresentKHR`来确保更新。但是，呈现引擎可以在任何时候进行更新。

- 而在使用`VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR`模式时，呈现引擎会在每个刷新周期内自动呈现共享可呈现图像的最新内容。应用程序只需要在初始时调用一次`vkQueuePresentKHR`，之后呈现引擎会在没有进一步呈现调用的情况下从图像中更新。应用程序可以通过调用`vkQueuePresentKHR`来指示图像内容已经更新，但这不保证更新的时间。

呈现引擎在共享可呈现图像首次呈现后随时可以访问它。为了避免撕裂现象，应用程序应该与呈现引擎协调访问。这需要通过特定于平台的机制获得呈现引擎的时序信息，并确保颜色附件的写入操作在呈现引擎刷新周期的适当时段可用。

<p style="color: #707070" Size=10>
    <font color=red Size=4>Notes</font>:<br>
    &ensp;&ensp;VK_KHR_shared_presentable_image扩展不提供用于确定呈现引擎刷新周期时序的功能。
 </p>

为了在渲染到共享可呈现图像时查询交换链的状态，可以调用以下函数：

```C++

// Provided by VK_KHR_shared_presentable_image
VkResult vkGetSwapchainStatusKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain);

```

- `device`是与`swapchain`关联的设备。
- `swapchain`是要查询的交换链。必须已创建、分配或可以在`device`中检索到

<p style="color: #707070" Size=10>
    <font color=red Size=4>Notes</font>:<br>
    &ensp;&ensp;
交换链状态可能会被实现缓存，因此在使用VkPresentModeKHR设置为VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR的交换链时，应用程序应定期调用vkGetSwapchainStatusKHR来查询其状态。
 </p>

创建交换链请调用：

```C++

// Provided by VK_KHR_swapchain
VkResult vkCreateSwapchainKHR(
    VkDevice                                    device,
    const VkSwapchainCreateInfoKHR*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchain);

// Provided by VK_KHR_swapchain
typedef struct VkSwapchainCreateInfoKHR {
    VkStructureType                  sType;
    const void*                      pNext;
    VkSwapchainCreateFlagsKHR        flags;
    VkSurfaceKHR                     surface;
    uint32_t                         minImageCount;
    VkFormat                         imageFormat;
    VkColorSpaceKHR                  imageColorSpace;
    VkExtent2D                       imageExtent;
    uint32_t                         imageArrayLayers;
    VkImageUsageFlags                imageUsage;
    VkSharingMode                    imageSharingMode;
    uint32_t                         queueFamilyIndexCount;
    const uint32_t*                  pQueueFamilyIndices;
    VkSurfaceTransformFlagBitsKHR    preTransform;
    VkCompositeAlphaFlagBitsKHR      compositeAlpha;
    VkPresentModeKHR                 presentMode;
    VkBool32                         clipped;
    VkSwapchainKHR                   oldSwapchain;
} VkSwapchainCreateInfoKHR;


```
- `device`是用于创建交换链的设备。
- `pCreateInfo`是指向`VkSwapchainCreateInfoKHR`结构的指针，用于指定创建交换链的参数。
- `pSwapchain`是一个指向`VkSwapchainKHR`句柄的指针，用于返回创建的交换链对象。

如上所述，如果`vkCreateSwapchainKHR`成功，它将返回一个包含至少`pCreateInfo->minImageCount`个可呈现图像的交换链句柄。

Notes: 在被应用程序获取时，可呈现图像可以以与等效的非可呈现图像相同的方式使用。可呈现图像相当于使用以下`VkImageCreateInfo`参数创建的非可呈现图像：
| VkImageCreateInfo Field	| Value  |
|--|--|
| flags| VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT is set if <br> VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR is set<br> VK_IMAGE_CREATE_PROTECTED_BIT is set if <br>VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR is set <br>VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT and <br> VK_IMAGE_CREATE_EXTENDED_USAGE_BIT_KHR are both set if <br>VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR is set <br>all other bits are unset |
| imageType | VK_IMAGE_TYPE_2D|
| format | pCreateInfo->imageFormat |
| extent | {pCreateInfo->imageExtent.width, pCreateInfo->imageExtent.height, 1} |
| mipLevels | 1 |
| arrayLayers | pCreateInfo->imageArrayLayers |
| samples | VK_SAMPLE_COUNT_1_BIT  |
| tiling | VK_IMAGE_TILING_OPTIMAL |
| usage | pCreateInfo->imageUsage  |
| sharingMode | pCreateInfo->imageSharingMode |
| queueFamilyIndexCount | pCreateInfo->queueFamilyIndexCount |
| pQueueFamilyIndices | pCreateInfo->pQueueFamilyIndices |
| initialLayout | VK_IMAGE_LAYOUT_UNDEFINED |


`pCreateInfo->surface`在`swapchain`被销毁之前不能被销毁。

如果`oldSwapchain`为`VK_NULL_HANDLE`，并且`pCreateInfo->surface`所引用的本地窗口已经关联了一个Vulkan交换链，则必须返回VK_ERROR_NATIVE_WINDOW_IN_USE_KHR错误。

如果`pCreateInfo->surface`所引用的本地窗口已经与非`Vulkan`图形`API`表面相关联，则必须返回`VK_ERROR_NATIVE_WINDOW_IN_USE_KHR`错误。

在`pCreateInfo->surface`所引用的本地窗口与任何关联的`Vulkan`交换链被销毁之前，不能将其与非`Vulkan`图形`API`表面相关联。

如果逻辑设备丢失，`vkCreateSwapchainKHR`将返回`VK_ERROR_DEVICE_LOST`。`VkSwapchainKHR`是设备的子对象，在设备之前必须被销毁。然而，`VkSurfaceKHR`不是任何`VkDevice`的子对象，不受设备丢失的影响。成功重新创建`VkDevice`后，可以使用相同的`VkSurfaceKHR`创建一个新的`VkSwapchainKHR`，前提是先销毁了之前的`Swapchain`

在调用`vkCreateSwapchainKHR`时，如果`oldSwapchain`不是`VK_NULL_HANDLE`，那么`oldSwapchain`将被标记为已弃用（retired），即使新交换链的创建失败。无论`oldSwapchain`是否是`VK_NULL_HANDLE`，新的交换链都将以非弃用状态创建。

在调用`vkCreateSwapchainKHR`时，如果`oldSwapchain`不是`VK_NULL_HANDLE`，那么未被应用程序获取的`oldSwapchain`中的任何图像可能会被实现释放，即使创建新的交换链失败也可能会发生。应用程序可以销毁`oldSwapchain`来释放与其相关联的所有内存。

<p style="color: #707070" Size=10>
    <font color=red Size=4>Notes</font>:<br>
    &ensp;&ensp;通过多次使用oldSwapchain来关联多个已废弃的交换链与同一个VkSurfaceKHR相关联，次数多于调用vkDestroySwapchainKHR的次数。在oldSwapchain被废弃后，应用程序可以通过vkQueuePresentKHR提交任何已经从oldSwapchain中获取的图像。例如，应用程序可以在来自新交换链的图像准备好被呈现之前，先呈现来自旧交换链的图像。正常情况下，如果oldSwapchain已经进入导致返回VK_ERROR_OUT_OF_DATE_KHR的状态，那么vkQueuePresentKHR可能会失败。应用程序可以继续使用从oldSwapchain获得的共享可呈现图像，直到从新交换链获取了可呈现图像，前提是它尚未进入导致返回VK_ERROR_OUT_OF_DATE_KHR的状态。
 </p>

##### Valid Usage:

- [`VkSwapchainCreateInfoKHR` Valid Usage][VkSwapchainCreateInfoKHR_vaild_usage]

##### Valid Usage (Implicit)

- [`VkSwapchainCreateInfoKHR` Valid Usage Implicit][VkSwapchainCreateInfoKHR_vaild_usage_Implicit]

可以在VkSwapchainCreateInfoKHR::flags字段中设置的位，用于指定交换链创建的参数，包括：

```C++

// Provided by VK_KHR_swapchain
typedef enum VkSwapchainCreateFlagBitsKHR {
  // Provided by VK_VERSION_1_1 with VK_KHR_swapchain, VK_KHR_device_group with VK_KHR_swapchain
    VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR = 0x00000001,
  // Provided by VK_VERSION_1_1 with VK_KHR_swapchain
    VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR = 0x00000002,
  // Provided by VK_KHR_swapchain_mutable_format
    VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR = 0x00000004,
} VkSwapchainCreateFlagBitsKHR;

```
- `VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR`指定从交换链创建的图像（即使用`VkImageSwapchainCreateInfoKHR`的`swapchain`成员设置为此交换链句柄）必须使用`VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT`。
- `VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR`指定从交换链创建的图像是受保护的图像。
- `VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR`指定交换链的图像可以用于创建一个`VkImageView`，其格式与创建交换链时指定的格式不同。通过在`VkSwapchainCreateInfoKHR`的`pNext`链中添加`VkImageFormatListCreateInfo`结构来指定允许的图像视图格式列表。此外，该标志还允许交换链使用在创建交换链时不支持的图像格式用途标志，但必须至少支持允许的某个图像视图格式。这个标志为应用程序提供了更大的灵活性，使其可以在运行时选择使用不同的图像格式，从而更好地满足特定的需求和限制。

```C++

// Provided by VK_KHR_swapchain
typedef VkFlags VkSwapchainCreateFlagsKHR;

```
`VkSwapchainCreateFlagsKHR`是一种位掩码类型，用于设置一个包含零个或多个`VkSwapchainCreateFlagBitsKHR`的掩码。

如果`VkSwapchainCreateInfoKHR`的`pNext`链包含`VkDeviceGroupSwapchainCreateInfoKHR`结构，则该结构包含了一组设备组呈现模式，该交换链可以与这些设备组呈现模式一起使用。这允许多个物理设备在同一交换链上进行呈现操作，从而实现分布式呈现。

`VkDeviceGroupSwapchainCreateInfoKHR`结构定义如下：

```C++

// Provided by VK_VERSION_1_1 with VK_KHR_swapchain, VK_KHR_device_group with VK_KHR_swapchain
typedef struct VkDeviceGroupSwapchainCreateInfoKHR {
    VkStructureType                     sType;
    const void*                         pNext;
    VkDeviceGroupPresentModeFlagsKHR    modes;
} VkDeviceGroupSwapchainCreateInfoKHR;

```
- `sType` ：指定结构类型，必须为[VkStructureType][VkStructureType]
- `pNext`：指向一个扩展结构的指针链，可用于扩展此结构。
- `modes`：指定了一组设备组呈现模式的标志位。设备组呈现模式由`VkDeviceGroupPresentModeFlagBitsKHR`枚举类型表示，它指定了一种或多种呈现模式。

如果不存在`VkDeviceGroupSwapchainCreateInfoKHR`结构，则`modes`被视为`VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR`。

##### Valid Usage
- [VkDeviceGroupSwapchainCreateInfoKHR Valid Usage Implicit][VkDeviceGroupSwapchainCreateInfoKHR_Valid_Usage_Implicit]


To destroy a swapchain object call:
```C++

// Provided by VK_KHR_swapchain
void vkDestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator);

    // device是与交换链相关联的设备
    // swapchain 销毁的交换链对象
    // ......

```

应用程序在销毁交换链之前，必须确保所有从交换链获取的图像上的所有操作都已完成。交换链及其关联的所有`VkImage`句柄都将被销毁，并且不能再由应用程序获取或使用。每个`VkImage`的内存将只在它不再被呈现引擎使用时被释放。例如，如果交换链的一个图像正在窗口中显示，那么该图像的内存可能在窗口被销毁或为窗口创建了另一个交换链后才会被释放。销毁交换链不会使父级`VkSurfaceKHR`失效，可以使用它创建一个新的交换链。

当与显示表面相关联的交换链被销毁时，如果最近向显示表面呈现的图像来自即将销毁的交换链，那么实现必须执行以下两个选项之一：
- 实现必须将由于从任何与显示表面关联的交换链呈现图像而修改的显示资源恢复到它们在这些交换链的第一次呈现之前的状态。
    - 这意味着在销毁交换链之前，实现会撤消对显示资源所做的任何修改，使其回到最初的状态。这样做的目的是确保在销毁交换链后，显示表面不会留下任何不良的影响。
- 或者实现必须将这些资源保留在它们当前的状态中。
    - 这意味着在销毁交换链之前，实现不会撤消对显示资源所做的任何修改，而是保留这些资源的当前状态。这样做的目的是避免在销毁交换链时引入额外的开销，并且假设显示资源的当前状态对于显示表面的后续操作是合适的。
具体选择由实现决定，但在任何情况下，实现必须确保在销毁与显示表面相关联的交换链后，显示表面的状态与应用程序预期的状态一致。

##### Valid Usage
- [vkDestroySwapchainKHR vaild usage][vkDestroySwapchainKHR_vaild_usage]

##### Host Synchronization
主机对交换链的访问必须进行外部同步


loading ......

[VkSwapchainCreateInfoKHR_vaild_usage]: https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap30.html#VUID-VkSwapchainCreateInfoKHR-surface-01270 "VkSwapchainCreateInfoKHR_vaild_usage"

[VkSwapchainCreateInfoKHR_vaild_usage_Implicit]: https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap30.html#VUID-VkSwapchainCreateInfoKHR-sType-sType "VkSwapchainCreateInfoKHR_vaild_usage_Implicit"

[VkStructureType]: https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap3.html#VkStructureType "VkStructureType"

[VkDeviceGroupSwapchainCreateInfoKHR_Valid_Usage_Implicit]: https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap30.html#VUID-VkDeviceGroupSwapchainCreateInfoKHR-sType-sType "VkDeviceGroupSwapchainCreateInfoKHR Valid Usage Implicit"

[vkDestroySwapchainKHR_vaild_usage]: https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap30.html#VUID-vkDestroySwapchainKHR-swapchain-01282 "vkDestroySwapchainKHR vaild usage"
