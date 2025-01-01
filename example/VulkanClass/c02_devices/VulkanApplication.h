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
     * @brief Get the Vulkan Initialized result
     *
     * @return true
     * @return false
     */
    bool getInitialized() { return _initiallized; }

private:
    bool createInstance(const char *appName);

    VkInstance _instance;
    bool _initiallized;
};


#endif // __VULKANAPPLICATION_H__
