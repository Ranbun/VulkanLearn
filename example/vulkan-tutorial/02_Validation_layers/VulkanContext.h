#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <memory>
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
    VkInstance m_instance;
    std::unique_ptr<VulkanDebugger> m_vulkanDebugger;
};

#endif
