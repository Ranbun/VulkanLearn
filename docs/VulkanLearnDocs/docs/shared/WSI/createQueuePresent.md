## 创建呈现队列

呈现队列的创建是随着逻辑设备的创建时完成的。

在前面选择物理设备的过程中，对于我们选择的物理设备，我们进行了对于呈现图像以及是否具备渲染条件的判断，并且记录了可用的队列族索引
-  呈现队列
-  渲染队列

创建逻辑设备的时候还会创建相应的队列，队列的创建由`VkDeviceQueueCreateInfo`结构体设置，我们需要填写我们创建队列个数的结构体数目，并以此创建我们需要的队列。

##### `VkDeviceQueueCreateInfo` defined as

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
- `pNext` 用于扩展结构体
- `queueFamilyIndex` 队列族索引，前面在选择物理设备的时候记录的结果
- `queueCount` 创建的队列的个数
    - `queueCount`必须小于或等于`VkQueueFamilyProperties`结构的`queueCount`成员，该成员由`vkGetPhysicalDeviceQueueFamilyProperties`在`pQueueFamilyProperties`[`queueFamilyIndex`]中返回。
    - 实际情况 一般不需要那么多的队列

### 获取创建的队列族
call`vkGetDeviceQueue`:

```C++
// Provided by VK_VERSION_1_0
void vkGetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue);

```

- `device`创建的物理设备
- `queueFamilyIndex` 用于创建队列的队列族索引
- `queueIndex`  队列的索引
    - 从`0`开始
- `pQueue` 返回创建的队列的句柄




