一旦Vulkan被初始化，设备(devices)和队列(queues)是与Vulkan实现进行交互的主要对象。<br>
Vulkan将物理设备(physical devices)和逻辑设备(logical devices)的概念分开。一个物理设备通常表示Vulkan的一个完整实现(不包括实例级功能)，可供主机使用，数量有限。逻辑设备表示该实现的一个实例，具有自己的状态和资源，与其他逻辑设备相互独立。

- [Physical Devices](./PhysicalDevices/5.1PhysicalDevices.md)
- [Devices](./Devices/5.2Devices.md)
- [Queues](./Queues/5.3Queues.md)



