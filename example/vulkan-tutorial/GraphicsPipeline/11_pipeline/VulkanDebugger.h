#ifndef VULKANDEBUGGER_H
#define VULKANDEBUGGER_H

#include <vulkan/vulkan.h>

class VulkanDebugger
{
public:
    VulkanDebugger(VkInstance instance);
    ~VulkanDebugger();

    VulkanDebugger(const VulkanDebugger &) = delete;
    VulkanDebugger &operator=(const VulkanDebugger &) = delete;

    static bool checkValidationLayerSupport();

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator);

    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                 const VkAllocationCallbacks *pAllocator,
                                                 VkDebugUtilsMessengerEXT *debugMessenger);

private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
};

#endif
