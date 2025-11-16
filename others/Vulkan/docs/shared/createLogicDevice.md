## 创建逻辑设备

选择物理设备后，我们还需要一个逻辑设备来作为和物理设备交互的接口。逻辑设备的创建过程类似于我们之前描述的Vulkan 实例的创建过程。我们还需要指定使用的队列所属的队列族。对于同一个物理设备，我们可以根据需求的不同，创建多个逻辑设备。

创建逻辑设备call:

```C++

// Provided by VK_VERSION_1_0
VkResult vkCreateDevice(
    VkPhysicalDevice                            physicalDevice,
    const VkDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice);

```
- `physicalDevice`筛选得到的物理设备
- `pCreateInfo`创建逻辑设备的信息
- `pDevice`逻辑设备指针，接受返回的逻辑设备句柄


##### Defined `VkDeviceCreateInfo` as:

```C++

// Provided by VK_VERSION_1_0
typedef struct VkDeviceCreateInfo {
    VkStructureType                    sType;
    const void*                        pNext;
    VkDeviceCreateFlags                flags;
    uint32_t                           queueCreateInfoCount;
    const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
    uint32_t                           enabledLayerCount;
    const char* const*                 ppEnabledLayerNames;
    uint32_t                           enabledExtensionCount;
    const char* const*                 ppEnabledExtensionNames;
    const VkPhysicalDeviceFeatures*    pEnabledFeatures;
} VkDeviceCreateInfo;

```
- `pQueueCreateInfos`是指向`VkDeviceQueueCreateInfo`结构数组的指针， 该结构描述了请求与逻辑设备一起创建的队列。
- `enabledExtensionCount` 启用扩展的数量
- `ppEnabledExtensionNames`启用扩展的字符串指针，以NULL结尾。

`VkDeviceCreateFlags`是用于设置掩码的位掩码类型，但当前保留以供将来使用。

### 创建队列

逻辑设备创建需要填写`VkDeviceQueueCreateInfo` 结构体。这一结构体描述了针对一个队列族我们所需的队列数量。

##### defined `VkDeviceQueueCreateInfo` as:

```C++
// Provided by VK_VERSION_1_0
typedef struct VkDeviceQueueCreateInfo {
    VkStructureType             sType;
    const void*                 pNext;
    VkDeviceQueueCreateFlags    flags;
    uint32_t                    queueFamilyIndex;
    uint32_t                    queueCount;
    const float*                pQueuePriorities;
} VkDeviceQueueCreateInfo;

```
- `queueFamilyIndex` 队列族索引
- `queueCount`创建队列的个数
- `pQueuePriorities` 指令缓冲的执行优先级 $ [0.0 - 1.0] $ `float`

- 如果有多个队列族就需要填写多个
eg:
    - 图形队列
    - 呈现队列

对于每个队列族，驱动程序只允许创建很少数量的队列，但实际上，对于每一个队列族，我们很少需要一个以上的队列。

### 创建逻辑设备

call `vkCreateDevice`函数以创建逻辑设备。

<font color=red>注：</font> 需要在最后退出的时候将逻辑设备销毁

- call: `vkDestroyDevice`

```C++

// Provided by VK_VERSION_1_0
void vkDestroyDevice(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator);

```

### 获取队列句柄

- call: `vkGetDeviceQueue`

```C++

void vkGetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue);

```

`vkGetDeviceQueue` 函数可以获取指定队列族的队列句柄。它的参数依次是逻辑设备对象，队列族索引，队列索引，来存储返回的队列句柄的内存地址

创建完逻辑设备，我们就可以真正开始使用显卡来完成一些操作
