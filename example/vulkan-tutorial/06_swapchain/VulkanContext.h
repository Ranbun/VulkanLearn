#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <functional>
#include <memory>
#include <optional>
#include <vulkan/vulkan.h>
#include "VulkanDebugger.h"
#include "VulkanFeatureManager.h"

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class VulkanContext
{
public:
    VulkanContext(const VulkanFeatureManager &feature);
    ~VulkanContext();
    VulkanContext(const VulkanContext &) = delete;
    VulkanContext &operator=(const VulkanContext &) = delete;

    VkInstance getInstance() { return m_instance; }
    VkDevice getDevice() { return m_logicDevice; }

private:
    /**
     * @brief init
     *
     */
    void init();

    /**
     * @brief cleanup resource
     *
     */
    void cleanup();

    /**
     * @brief Create a Instance object
     *
     */
    void createInstance();

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

private:
    const VulkanFeatureManager &m_featureManager;
    std::unique_ptr<VulkanDebugger> m_vulkanDebugger{nullptr};

    VkInstance m_instance{VK_NULL_HANDLE};
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};
    VkDevice m_logicDevice{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

    VkQueue m_presentQueue{VK_NULL_HANDLE};

    std::function<QueueFamilyIndices(VkPhysicalDevice &)> findQueueFamiliesFunc;
};

#endif
