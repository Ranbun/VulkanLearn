#ifndef  VULKANTRIANGLE_H_
#define  VULKANTRIANGLE_H_

#include <vector>
#include <vulkan/vulkan.h>

constexpr int WIDTH = 1000;
constexpr int HEIGHT = 800;

const std::vector validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

/// use validation layers ?
#ifdef NODEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

/**
 * @brief  前置申明
 */
class GLFWwindow;

/**
 * @brief 交换链的的检测
 */
struct SwapChainSupportDetails;

/**
 * @brief 队列的信息
 */
struct QueueFamilyIndices;



/**
 * @brief VulKan渲染三角形基类
 */
class HelloTriangleApplication
{
public:
    HelloTriangleApplication() = default;
    virtual ~HelloTriangleApplication() = default;

    HelloTriangleApplication(const HelloTriangleApplication&) = delete;
    HelloTriangleApplication(const HelloTriangleApplication&&) = delete;

    auto operator=(const HelloTriangleApplication&) -> HelloTriangleApplication& = delete;
    auto operator=(const HelloTriangleApplication&&) -> HelloTriangleApplication& = delete;

    auto run() -> void;

private:
    /**
     * @brief 初始化窗口
     */
    auto initWindow() -> void;

    /**
     * @brief 初始化vk的环境
     */
    auto initVulKan() -> void;

    /**
     * @brief 设置调试的回调
     */
    auto setupDebugMessenger() -> void;

    /**
     * @brief 渲染循环
     */
    auto mainLoop() const -> void;

    /**
     * @brief 退出时候的清空
     */
    auto cleanup() -> void;

    /**
     * @brief 创建VulKan实例
     */
    auto createInstance() -> void;

    /**
     * @brief 检测所有的检验层都能从列表中找到
     * @return 检测结果
     */
    static auto checkValidationLayerSupport() -> bool;

    /**
    * @brief message callback
    */
    [[nodiscard]] auto getRequireExtensions() const -> std::vector<const char *>;

    /**
     * @brief 创建VkDebugUtilsMessengerEXT对象
     * @param instance vk实例
     * @param pCreateInfo debug 调试创建对象
     * @param pAllocator 分配器
     * @param pCallback 回调函数
     * @return 结果
     */
    auto CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pCallback) const -> VkResult;

    /**
     * @brief 删除 DebugUtilsMessenger 对象
     * @param instance vk实例
     * @param callback 被创建的调试对象
     * @param pAllocator 分配器
     */
    auto DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT callback,
                                       const VkAllocationCallbacks* pAllocator) const -> void;

    /**
     * @brief 选择物理设备
     */
    auto pickPhysicalDevice() -> void;

    /**
     * @brief 设被是否可用
     * @param device 被选择的设备
     * @return 判定结果
     */
    auto isDeviceSuitable(VkPhysicalDevice device) const -> bool;

    /**
     * @brief 为选择的设备打分
     * @param device 被选择的设备
     * @return 分数
     */
    auto rateDeviceSuitability(VkPhysicalDevice device) const -> int;

    /**
     * @brief 查找满足要求的队列族
     * @param device 传入的设备
     * @return 队列族的索引
     */
    auto findQueueFamily(VkPhysicalDevice device) const -> QueueFamilyIndices;

    /**
     * @brief 创建逻辑设备
     */
    auto createLogicDevice() -> void;

    /**
     * @brief 设置结构体的属性
     * @param createInfo 被设置的结构体
     */
    static auto populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) -> void;

    /**
     * @brief 创建显示的表面
     */
    auto createSurface() -> void;

    /**
     * @brief 检查设备支持情况
     */
    auto checkDeviceExtensionSupport(VkPhysicalDevice device) const -> bool;

    /**
     * @brief 获取交换链的属性 
     * @param device 物理设备
     * @return 获取的结果
     */
    auto querySwapChainSupport(VkPhysicalDevice device) const -> SwapChainSupportDetails;

    /**
     * @brief 选择合适的表面格式
     * @param availableFormats 当前物理设备的所有的表面格式 
     * @return 选中的表面格式
     */
    auto chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) -> VkSurfaceFormatKHR;


    /**
     * @brief 查找最佳显示模式
     * @param availablePresentModes 对应物理设备的呈现模式
     * @return 查找的最佳显示模式
     */
    auto chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) -> VkPresentModeKHR;

    /**
     * @brief 选择合适的交换范围  - 图像的分辨率
     * @param capabilities 表面的基础属性
     * @return 选择的结果
     */
    auto chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) -> VkExtent2D;

    /**
     * @brief  创建交换链
     */
    auto createSwapChain() -> void;

    /**
     * @brief 创建图像视图
     */
    auto createImageViews() -> void ;


    /**
     * @brief  创建管线
     */
    auto createGraphicsPipeline() const -> void;


private:
    /**
     * @brief 当前的绘制的窗口 
     */
    GLFWwindow* m_window{nullptr};

    /**
     * @brief VulKan instance 
     */
    VkInstance m_vkInstance = nullptr;

    /**
     * @brief 回调对象
     */
    VkDebugUtilsMessengerEXT m_callBack = nullptr;

    /**
     * @brief 存储选择使用的显卡信息
     */
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    /**
     * @brief 逻辑设备对象
     */
    VkDevice m_logicDevice = nullptr;

    /**
     * @brief 逻辑设备队列
     */
    VkQueue m_graphicsQueue = nullptr;

    /**
     * @brief 窗口表面
     */
    VkSurfaceKHR m_surface = nullptr;

    /**
     * @brief 显示队列
     * @note 确保渲染结果能在我们创建的表面上显示(需要选择的物理设备支持)
     */
    VkQueue m_presentQueue = nullptr;

    /**
     * @brief 被创建的交换链的句柄
     */
    VkSwapchainKHR m_swapChain;


    /**
     * @brief 交换链图像
     */
    std::vector<VkImage> m_swapChainImages;

    /**
     * @brief 访问图像创建的图像试图 
     */                                                                          
    std::vector<VkImageView> m_swapChainImagesViews;


    /**
     * @brief 交换链图像格式
     */
    VkFormat m_swapChainImageFormat;

    /**
     * @brief 交换链图像范围
     */
    VkExtent2D m_swapChainExtent;

};

#endif
