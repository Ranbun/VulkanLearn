#ifndef  VULKANTRIANGLE_H_
#define  VULKANTRIANGLE_H_

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN

const int WIDTH = 800;
const int HEIGHT = 600;

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

    bool isComplete()
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

    void initWindow();

    void initVulKan();

    void setupDebugCallback();

    void mainLoop() const;

    void cleanup();

    void createInstance();

    /// @brief 检测所有的检验层都能从列表中找到
    static bool checkValidationLayerSupport();

    /// @brief message callback
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
                                          VkDebugUtilsMessengerEXT * pCallback);


    /**
     * @brief 删除debugmessengerEXT对象
     * @param instance 
     * @param callback 
     * @param pAllocator 
     */
    void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT callback,
                                       const VkAllocationCallbacks* pAllocator);


    /**
     * @brief 选择物理设备
     */
    void pickPhysicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);

    int rateDeviceSuitability(VkPhysicalDevice device);

    /**
     * @brief 查找满足要求的队列族
     * @param device 传入的设备
     * @return 队列族的索引
     */
    QueueFamilyIndices findQueueFamily(VkPhysicalDevice device);


private:

    /**
     * @brief 当前的绘制的窗口 
     */
    GLFWwindow* m_window{nullptr};

    /// VulKan instance 
    VkInstance m_vkInstance = nullptr;

    VkDebugUtilsMessengerEXT m_callBack = nullptr;

    /**
     * @brief 存储选择使用的显卡信息
     */
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;




};

#endif 