#ifndef __VULKANAPPLICATION_H__
#define __VULKANAPPLICATION_H__

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <map>

/**
 * @brief Vulkan Application
 *
 */
class VulkanApplication
{
public:
    explicit VulkanApplication(const char *appName, int width = 1000, int height = 800);
    ~VulkanApplication();

    VkInstance getInstance() const { return _instance; }
    VkPhysicalDevice getPhysicalDevice() const { return _physicalDevice; }
    VkSurfaceKHR getSurface() const { return _surface; }
    VkDevice getLogicDevice() const  { return _logicDevice;}

    VkSurfaceFormatKHR getSurfaceFormat() const { return _imageFormat; }
    std::vector<VkImage> getSwapChainImage() const {  return _swapChainImages; }
    VkPresentModeKHR getPresentMode() const { return _presentMode; }
    VkSwapchainKHR getSwapChain() const { return _swapChain; }
    VkCommandBuffer getCommandBuffer() const { return _commandBuffer;}
    std::vector<VkImageView> & getSwapChainImageView(){return _swapChainImageViews;}
    VkRenderPass & getRenderPass(){return _renderPass;}
    std::vector<VkFramebuffer> & getFramebuffer(){return _framebuffers;}

    VkFence getOrCreateFence(const std::string &name) ;
    VkSemaphore getOrCreateSemaphore(const std::string &name);

    void submitAndPresent(VkSemaphore waitImage, VkSemaphore waitSubmission, VkFence fenceSubmission, uint32_t imagIndex);

    [[nodiscard]] bool getInitialized() const { return _initialized; }
    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(_window); }
    [[maybe_unused]] void setClose() const { glfwSetWindowShouldClose(_window, 1); }
    uint32_t reCreateSwapChain(int &w, int &h, VkSemaphore &waitImage);


    void mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods);
    void keyPressCallBack(GLFWwindow *window, int key, int scancode, int action, int mods);
    void resizeCallBack(GLFWwindow *window, int w, int h);

private:
    bool createInstance(const char *appName);
    bool obtainPhysicalDevice();
    bool createLogicDevice();
    bool createWindowSurface(const char *name, int width, int height);
    bool createSwapChain();
    bool createSwapChainImageView();
    bool createCommandBuffer();
    bool createRenderPass();
    bool createFramebuffer(int w, int h);
    void cleanUpSwapChain();

    VkInstance _instance{};
    VkPhysicalDevice _physicalDevice{};
    VkDevice _logicDevice{};
    VkSurfaceKHR _surface{};
    GLFWwindow * _window{};

    VkCommandPool _commandPool{};
    VkCommandBuffer _commandBuffer{};

    std::vector<VkFramebuffer> _framebuffers;
    VkRenderPass _renderPass{};

    /// swap chain
    VkSwapchainKHR _swapChain{};
    std::vector<VkImage> _swapChainImages{};
    std::vector<VkImageView> _swapChainImageViews{};
    VkPresentModeKHR _presentMode{};
    VkSurfaceFormatKHR _imageFormat{};

    std::map<std::string, VkFence> _fences;
    std::map<std::string, VkSemaphore> _semaphores;

    bool _initialized{false};
};
#endif// __VULKANAPPLICATION_H__
