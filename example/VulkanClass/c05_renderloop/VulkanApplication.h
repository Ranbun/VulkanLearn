#ifndef __VULKANAPPLICATION_H__
#define __VULKANAPPLICATION_H__

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

/**
 * @brief Vulkan Application
 *
 */
class VulkanApplication
{
public:
    explicit VulkanApplication(const char *appName, int width = 1000, int height = 800);
    ~VulkanApplication();

    /**
     * @brief Get the Instance object
     *
     * @return VkInstance
     */
    VkInstance getInstance() { return _instance; }

    /**
     * @brief Get the Physical Device object
     *
     * @return VkPhysicalDevice
     */
    VkPhysicalDevice getPhysicalDevice() { return _physicalDevice; }

    /**
     * @brief 获取surface
     * @return VkSurface
     */
    VkSurfaceKHR getSurface() const { return _surface; }

    /**
     * @brief Get the Logic Device object
     *
     * @return VkDevice
     */
    VkDevice getLogicDevice() const
    {
        return _logicDevice;
    }

    VkFormat getSurfaceFormat() const
    {
        return _imageFormat.format;
    }

    std::vector<VkImage> getImage() const
    {
        return _images;
    }

    /**
     * @brief Get the Vulkan Initialized result
     *
     * @return true 初始化完成
     * @return false 初始化未完成
     */
    [[nodiscard]] bool getInitialized() const { return _initialized; }

    /**
     * @brief 是否需要关闭窗口
     * @return
     */
    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(_window); }

    void mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods);
    void keyPressCallBack(GLFWwindow *window, int key, int scancode, int action, int mods);
    void resizeCallBack(GLFWwindow *window, int w, int h);

private:
    bool createInstance(const char *appName);
    bool obtainPhysicalDevice();

    bool createLogicDevice();

    bool createWindowSurface(const char *name, int width, int height);
    bool createSwapChain();

    VkInstance _instance{};
    VkPhysicalDevice _physicalDevice{};
    VkDevice _logicDevice{};
    VkSurfaceKHR _surface{};
    GLFWwindow * _window{};

    /// swap chain
    VkSwapchainKHR _swapChain{};
    std::vector<VkImage> _images{};
    VkPresentModeKHR _presentMode{};
    VkSurfaceFormatKHR _imageFormat{};

    bool _initialized{false};
};

#endif// __VULKANAPPLICATION_H__
