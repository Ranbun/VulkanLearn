#ifndef __VULKANAPPLICATION_H__
#define __VULKANAPPLICATION_H__

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

/**
 * @brief Vulkan Application
 *
 */
class VulkanApplication
{
public:
    VulkanApplication(const char *appName);
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
     * @brief Get the Logic Device object
     *
     * @return VkDevice
     */
    VkDevice getLogicDevice() { return _logicDevice; }

    /**
     * @brief Get the Vulkan Initialized result
     *
     * @return true
     * @return false
     */
    bool getInitialized() { return _initiallized; }

private:
    bool createInstance(const char *appName);
    bool obtainPhysicalDevice();
    bool createLogicDevice();

    VkInstance _instance;
    VkPhysicalDevice _physicalDevice;
    VkDevice _logicDevice;

    bool _initiallized;
};


#endif// __VULKANAPPLICATION_H__
