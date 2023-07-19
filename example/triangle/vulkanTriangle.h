#ifndef  VULKANTRIANGLE_H_
#define  VULKANTRIANGLE_H_

#include <vector>
// #include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>


/**
 * @brief 窗口的 size
 */
constexpr int WIDTH = 1000;
constexpr int HEIGHT = 800;

/**
 * @brief 检验层
 * @note 当前请求使用的检验层
 */
const std::vector validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

/// use validation layers
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
 * @brief 每次异步渲染的帧数
 */
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

/**
 * @brief VulKan渲染三角形基类
 */
class HelloTriangleApplication
{
public:
    HelloTriangleApplication();
    virtual ~HelloTriangleApplication() = default;

    HelloTriangleApplication(const HelloTriangleApplication&) = delete;
    HelloTriangleApplication(const HelloTriangleApplication&&) = delete;

    auto operator=(const HelloTriangleApplication&) -> HelloTriangleApplication& = delete;
    auto operator=(const HelloTriangleApplication&&) -> HelloTriangleApplication& = delete;

    /**
     * @brief 运行程序
     */
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
    auto mainLoop() -> void;

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
     * @note 请求所有的检验层
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
     * @note 由于是扩展函数 所以此函数并不会被加载，需要手动加载
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
    [[maybe_unused]] auto rateDeviceSuitability(VkPhysicalDevice device) const -> int;

    /**
     * @brief 查找满足要求的队列族
     * @param device 传入的设备
     * @return 队列族的索引 (队列族索引是绑定到物理设备的从0开始的整数 可以在创建逻辑设备的时候引用它)
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
     * @brief 检查设备支持情况 是否支持交换链
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
    auto createGraphicsPipeline() -> void;

    /**
     * @brief 创建shader module
     * @param code 着色器文件
     * @return 创建的module
     */
    [[nodiscard]] auto createShaderModule(const std::vector<char>& code) const ->VkShaderModule;

    /**
     * @brief 设置帧缓冲附着
     * @return none
     */
    auto createRenderPass()->void;

    /**
     * @brief 创建帧缓冲对象
     */
    void createFramebuffers();

    /**
     * @brief 创建指令池
     */
    void createCommandPool();

    /**
     * @brief 创建指令缓冲对象
     */
    void createCommandBuffers();

    /**
     * @brief 记录指令到缓冲
     */
    void recordCommandBuffer(VkCommandBuffer commandBuffer,uint32_t imageIndex);

    /**
     * @brief 渲染一帧
     */
    void drawFrame();

    /**
     * @brief 创建信号量
     */
    void createSyncObjects();

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
    VkSwapchainKHR m_swapChain{};

    /**
     * @brief 交换链图像句柄 -- 用于渲染
     */
    std::vector<VkImage> m_swapChainImages;

    /**
     * @brief 访问图像创建的图像视图
     */
    std::vector<VkImageView> m_swapChainImagesViews;

    /**
     * @brief 交换链图像格式
     */
    VkFormat m_swapChainImageFormat;

    /**
     * @brief 交换链图像范围
     */
    VkExtent2D m_swapChainExtent{};

    /**
     * @brief  渲染流程
     */
    VkRenderPass m_renderPass{};

    /**
     * @brief  管线布局
     */
    VkPipelineLayout m_pipelineLayout{};

    /**
     * @brief  创建的管线对象
     */
    VkPipeline m_graphicsPipeline{};

    /**
     * @brief 帧缓冲对象
     */
    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    /**
     * @brief 指令池
     */
    VkCommandPool m_commandPool{};

    /**
     * @brief 指令缓冲对象
     */
    std::vector<VkCommandBuffer> m_commandBuffers;

    /**
     * @brief 信号量
     */
    std::vector<VkSemaphore> m_imageAvailableSemaphore;

    /**
     * @brief 信号量
    */
    std::vector<VkSemaphore> m_renderFinishedSemaphore;

    /**
     * @brief 栅栏 同步CPU & GPU
    */
    std::vector<VkFence> m_inFlightFence;

    /**
     * @brief 当前帧ID
    */
    size_t m_currentFrame = 0;

};

#endif
