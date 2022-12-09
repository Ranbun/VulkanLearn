#ifndef  VULKANTRIANGLE_H_
#define  VULKANTRIANGLE_H_

#include <functional>
#include <iostream>
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

/// use validation layers ?
#ifdef NODEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


/**
 * @brief 满足需求队列族的索引
 */
struct  QueueFamilyIndices
{
    int m_graphicsFamily = -1;

    [[nodiscard]] bool isComplete() const
    {
        return m_graphicsFamily >= 0;
    }
};

class GLFWwindow;

class HelloTriangleApplication
{
public:
    void run();

private:
    /**
     * @brief 初始化窗口
     */
    void initWindow();

    /**
     * @brief 初始化vk的环境
     */
    void initVulKan();

    /**
     * @brief 设置调试的回调
     */
    void setupDebugCallback();

    /**
     * @brief 渲染循环
     */
    void mainLoop() const;

    /**
     * @brief 退出时候的清空
     */
    void cleanup();

    /**
     * @brief 创建VulKan实例
     */
    void createInstance();

    /**
     * @brief 检测所有的检验层都能从列表中找到
     * @return 检测结果
     */
    static bool checkValidationLayerSupport();

    /**
    * @brief message callback
    */
    [[nodiscard]] std::vector<const char *> getRequireExtensions() const;

    /**
     * @brief 创建VkDebugUtilsMessengerEXT对象
     * @param instance vk实例
     * @param pCreateInfo debug 调试创建对象
     * @param pAllocator 分配器
     * @param pCallback 回调函数
     * @return 结果
     */
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
                                          const VkAllocationCallbacks * pAllocator,
                                          VkDebugUtilsMessengerEXT * pCallback) const;


    /**
     * @brief 删除 DebugUtilsMessenger 对象
     * @param instance vk实例
     * @param callback 被创建的调试对象
     * @param pAllocator 分配器
     */
    void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT callback,
                                       const VkAllocationCallbacks* pAllocator);


    /**
     * @brief 选择物理设备
     */
    void pickPhysicalDevice();

    /**
     * @brief 设被是否可用
     * @param device 被选择的设备
     * @return 判定结果
     */
    bool isDeviceSuitable(VkPhysicalDevice device) const;

    /**
     * @brief 为选择的设备打分
     * @param device 被选择的设备
     * @return 分数
     */
    int rateDeviceSuitability(VkPhysicalDevice device);

    /**
     * @brief 查找满足要求的队列族
     * @param device 传入的设备
     * @return 队列族的索引
     */
    QueueFamilyIndices findQueueFamily(VkPhysicalDevice device) const;

    /**
     * @brief 创建逻辑设备
     */
    void createLogicDevice();


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
    VkDevice m_device = nullptr;



};

#endif 