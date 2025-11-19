#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <functional>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include "VulkanDebugger.h"

class VulkanContext
{
public:
    VulkanContext(const std::vector<const char *> &requiredExtensions);
    ~VulkanContext();
    VulkanContext(const VulkanContext &) = delete;
    VulkanContext &operator=(const VulkanContext &) = delete;

    VkInstance getInstance() { return m_instance; }
    VkDevice getDevice() { return m_logicDevice; }

private:
    /**
     * @brief init
     *
     * @param requiredExtensions
     */
    void init(const std::vector<const char *> &requiredExtensions);

    /**
     * @brief cleanup resource
     *
     */
    void cleanup();

    /**
     * @brief Create a Instance object
     *
     * @param requiredExtensions
     */
    void createInstance(const std::vector<const char *> &requiredExtensions);

    /**
     * @brief select physical device
     *
     */
    void pickPhysicalDevice();


    /**
     * @brief Create a Logic Device object
     *
     */
    void createLogicDevice();

    /**
     * @brief Create a Surface object
     *
     */
    void createSurface();

    /**
     * @brief setup messenger
     *
     */
    void setupDebugMessenger();

    /**
     * @brief check Requirement Extensions Support
     *
     * @param requiredExtensions
     * @return true
     * @return false
     */
    static bool checkRequirementsExtensionSupport(const std::vector<const char *> &requiredExtensions);

    /**
     * @brief Get the Vulkan Layers Extensions object
     * @note get all others extension layers in this function
     * @return std::vector<const char *>
     */
    std::vector<const char *> getVulkanLayersExtensions();

private:
    VkInstance m_instance{VK_NULL_HANDLE};
    std::unique_ptr<VulkanDebugger> m_vulkanDebugger{nullptr};
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};
    VkDevice m_logicDevice{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

    VkQueue m_presentQueue{VK_NULL_HANDLE};

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };

    std::function<QueueFamilyIndices(VkPhysicalDevice &)> findQueueFamiliesFunc;
};

#endif
