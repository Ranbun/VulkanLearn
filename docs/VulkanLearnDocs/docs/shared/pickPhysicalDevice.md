## 选择物理设备
列出所有物理设备： call `vkEnumeratePhysicalDevices`:


```C++

// Provided by VK_VERSION_1_0
VkResult vkEnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices);


```

vkEnumeratePhysicalDevices


### 获取设备属性

```C++
// Provided by VK_VERSION_1_0
void vkGetPhysicalDeviceProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties*                 pProperties);

VkPhysicalDeviceProperties deviceProperties ;
vkGetPhysicalDeviceProperties ( device , &deviceProperties);

```

<font color=red>注：</font> 设备属性可以作为物理设备的筛选条件


### 队列族

`Vulkan` 有多种不同类型的队列，它们属于不同的队列族，每个队列族的队列只允许执行特定的一部分指令。
我们需要检测设备支持的队列族，以及它们中哪些支持我们需要使用的指令。
- 检测设备支持的队列族，以及它们中哪些支持我们需要使用的指令,获取满足需求的队列族索引
    - eg： 支持图形命令

查询队列族信息：
```C++

// Provided by VK_VERSION_1_0
void vkGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties);

/// eg:
uint32_t queueFamilyCount = 0;
vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount ,nullptr);

std::vector<VkQueueFamilyProperties> queueFamilies (queueFamilyCount) ;
vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,queueFamilies.data());

```

<font color=red>注：</font> 队列族对于命令的支持可以作为物理设备的筛选条件

### 设备级扩展的支持

<font color=red>注：</font> 扩展的支持可以作为物理设备的筛选条件

