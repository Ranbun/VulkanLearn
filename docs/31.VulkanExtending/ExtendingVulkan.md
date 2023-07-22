# Extending Vulkan

### 1. 新功能扩展机制

- 添加新的扩展扩展功能
- 更新新的核心版本(一般来自于扩展)

### 2. 功能分类
- 实例级功能
- 设备级功能

#### 实例级功能
- 可获取实例的属性，或者接受一个Vulkan实例(VkInstance)对象作为参数的命令视作实例级功能

#### 设备级功能
- 从VkDevice对象或VkDevice的子对象分派或将其中任何一个(VkDevice or VkDevice子对象)作为参数的命令被视为设备级功能。设备扩展定义的类型也被视作设备级功能。
- 获取物理设备属性的命令也是设备级功能


### 3. Vulkan 层
- 扩展Vulkan的行为到规范之外
- 启用某个层后，它会将自身插入到该层关注的 `Vulkan` 命令的调用链中。层可用于各种任务，这些任务将 `Vulkan` 的基本行为扩展到规范要求之外 - 例如调用日志记录、跟踪、验证或提供附加扩展。

```C++
// 查询层的信息
// Provided by VK_VERSION_1_0
VkResult vkEnumerateInstanceLayerProperties(
    uint32_t*                                   pPropertyCount,
    VkLayerProperties*                          pProperties);

// pPropertyCount must be a valid pointer to a uint32_t value
// If the value referenced by pPropertyCount is not 0, and pProperties is not NULL, pProperties must be a valid pointer to an array of pPropertyCount VkLayerProperties structures

```

- 一旦创建了实例，层将保持在实例的生命周期内继续启用用，即使其中的某些层对于未来的某些实例不可用(`vkEnumerateInstanceLayerProperties`同样的两次调用可能会得到不同的结果)

#### 启用层
##### 显示启用
- 创建实例时指定层的名称到实例创建的参数中

```C++
/**
 * @brief 创建实例的结构体
*/
VkInstanceCreateInfo createInfo{};
......
/**
 * @brief 设置校验层
*/
createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
createInfo.ppEnabledLayerNames = validationLayers.data();

```

##### 隐式启用 - 加载器
- 隐式启用的层在显式启用的层之前加载，因此隐式启用的层更接近应用程序，而显式启用的层更接近驱动程序。除非另有规定，隐式启用和显式启用的层仅在启用方式和加载顺序上有所区别。显式启用一个已经隐式启用的层将导致该层作为隐式启用的层进行加载，没有额外的影响。
- 在`Vulkan`中，`Loader`是一个关键的组件，它负责加载和管理`Vulkan`的实现（驱动程序）。`Loader`充当了应用程序与底层驱动程序之间的中间层。它提供了一个统一的接口，使得应用程序可以与不同的驱动程序进行交互，而不需要直接与特定的硬件或驱动程序进行耦合。
`Loader`的加载方式可以是隐式的，也可以是显式的。隐式加载是通过系统环境变量或配置文件等机制自动加载适当的驱动程序。这种方式通常由`Vulkan`实现的供应商或操作系统提供。在隐式加载中，`Loader`会自动查找并加载可用的驱动程序，而应用程序无需显式指定。
相比之下，显式加载需要应用程序明确指定要加载的驱动程序库。应用程序需要提供驱动程序库的路径或名称，并通过编程方式加载该库。这种方式通常在需要对驱动程序进行更精确的控制或测试特定驱动程序功能时使用。
无论是隐式加载还是显式加载，`Loader`的主要作用是在运行时将应用程序与适当的驱动程序进行连接，并提供`Vulkan API`的实现。

#### 设备层 - <font color=red>弃用</font>

### 4. 扩展
- 扩展定义的命令无法用于静态链接 - 在这种情况下，应在运行时查询这些命令的函数指针，如在命令函数指针中所述。扩展可以由层和Vulkan实现提供。

#### 启用扩展
##### 实例扩展
-  指定 `VkInstanceCreateInfo`结构体的`ppEnabledExtensionNames`参数在创建`VkInstance`时候.


##### 设备扩展
- 指定 `VkDeviceCreateInfo`结构体的`ppEnabledExtensionNames`参数在创建`VkDevice`时候.

#### 实例扩展
- 在核心API范围之外向Vulkan添加新的实例级功能
- 查询可用的实例级扩展

```C++
// Provided by VK_VERSION_1_0
VkResult vkEnumerateInstanceExtensionProperties(
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties);

/// pLayerName 指定用于搜索扩展的层的名称 - 前面提到可以通过层附加扩展

```

#### 设备扩展
- 设备扩展向API添加了核心规范之外的新的设备级功能。这些扩展可以扩展Vulkan的功能，引入新的命令、结构体、枚举值或其他特定功能，以满足特定应用程序的需求。

```C++
// Provided by VK_VERSION_1_0
VkResult vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice                            physicalDevice,
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties);

// physicalDevice must be a valid VkPhysicalDevice handle

```
- 要使用设备级扩展的功能必须在创建逻辑设备的时候启用相应的扩展

### 5. 扩展的依赖
- &ensp;&ensp;一些扩展的功能依赖于其他扩展或特定的核心API版本。要启用具有依赖关系的扩展，在使用vkCreateInstance创建实例或使用vkCreateDevice创建设备时，必须通过相同的API机制启用所需的扩展。具有这种依赖关系的每个扩展都在总结该扩展的附录中记录了这些依赖关系。
&ensp;&ensp;如果一个扩展被支持（通过vkEnumerateInstanceExtensionProperties或vkEnumerateDeviceExtensionProperties查询），那么该扩展所需的其他扩展也必须在同一实例或物理设备上被支持。
&ensp;&ensp;如果一个设备扩展具有依赖于实例扩展的依赖关系，并且该实例扩展没有在vkCreateInstance中启用，那么该设备扩展被视为不受支持，因此在该实例的任何VkPhysicalDevice子对象中不应返回该设备扩展。实例扩展不依赖于设备扩展。
&ensp;&ensp;如果一个所需的扩展被提升为另一个扩展或核心API版本，那么通常情况下，依赖关系也会被提升的扩展或核心版本所满足。只要原始扩展所需的任何功能也被提升的扩展或核心版本所需或启用，这个规则就成立。然而，在某些情况下，一个扩展在提升为另一个扩展或核心版本时，可能会将其某些功能设为可选。在这种情况下，依赖关系可能无法满足。要确定依赖关系是否满足，唯一的方法是查看图层和扩展附录中原始依赖和提升版本的描述信息。


[Vulkan-1.3]: https://registry.khronos.org/vulkan/specs/1.3/html/chap31.html#extendingvulkan-layers
