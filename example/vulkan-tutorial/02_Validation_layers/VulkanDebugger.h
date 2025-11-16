#ifndef VULKANDEBUGGER_H
#define VULKANDEBUGGER_H

#include <vector>
#include <vulkan/vulkan.h>


class VulkanDebugger
{
public:
    VulkanDebugger(VkInstance instance);
    ~VulkanDebugger();

    VulkanDebugger(const VulkanDebugger &) = delete;
    VulkanDebugger &operator=(const VulkanDebugger &) = delete;

    static bool checkValidationLayerSupport();

    static const std::vector<const char *> &getRequiredLayers();

    static std::vector<const char *> getRequiredExtensionName();

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    static constexpr bool enableValidationLayers()
    {
#ifdef NDEBUG
        return false;
#else
        return true;
#endif
    }

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
    static std::vector<const char *> s_validationLayers;

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
};

#endif
