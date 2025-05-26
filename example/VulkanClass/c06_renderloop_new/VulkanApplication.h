#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <optional>

class VulkanApplication {
public:
    struct Config {
        std::string appName = "VulkanApp";
        int width = 1280;
        int height = 720;
        bool enableValidation = true;
    };

    explicit VulkanApplication(const Config& config);
    ~VulkanApplication();

    // 禁止拷贝和赋值
    VulkanApplication(const VulkanApplication&) = delete;
    VulkanApplication& operator=(const VulkanApplication&) = delete;

    // 允许移动
    VulkanApplication(VulkanApplication&&) noexcept;
    VulkanApplication& operator=(VulkanApplication&&) noexcept;

    void mainLoop();
    bool shouldClose() const;

    VkInstance instance() const { return m_instance; }
    VkDevice device() const { return m_device; }
    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    VkSurfaceKHR surface() const { return m_surface; }
    GLFWwindow* window() const { return m_window; }
    VkFormat swapchainImageFormat() const { return m_swapchainImageFormat; }
    VkExtent2D swapchainExtent() const { return m_swapchainExtent; }
    const std::vector<VkImage>& swapchainImages() const { return m_swapchainImages; }
    const std::vector<VkImageView>& swapchainImageViews() const { return m_swapchainImageViews; }

private:
    void initWindow();
    void initVulkan();
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapchain();
    void createImageViews();
    void cleanup();

    // 工具函数
    bool checkValidationLayerSupport() const;
    std::vector<const char*> getRequiredExtensions() const;

    // 成员变量
    Config m_config;
    GLFWwindow* m_window = nullptr;

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
    VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_swapchainExtent{};

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

    bool m_initialized = false;
};
