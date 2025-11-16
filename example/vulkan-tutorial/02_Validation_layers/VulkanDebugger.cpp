#include "VulkanDebugger.h"

#include <algorithm>
#include <cstring>
#include <iostream>

std::vector<const char *> VulkanDebugger::s_validationLayers{"VK_LAYER_KHRONOS_validation"};

VulkanDebugger::VulkanDebugger(VkInstance instance) : m_instance(instance), m_debugMessenger(VK_NULL_HANDLE)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo); // 使用静态辅助函数填充

    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to set up debug messenger!");
    }
}

VulkanDebugger::~VulkanDebugger()
{
    if (m_debugMessenger != VK_NULL_HANDLE)
    {
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }
}


VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                             void *pUserData)
{

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
void VulkanDebugger::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                   const VkAllocationCallbacks *pAllocator)
{
    auto func =
            (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}
VkResult VulkanDebugger::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                      const VkAllocationCallbacks *pAllocator,
                                                      VkDebugUtilsMessengerEXT *debugMessenger)
{
    /// load debug messenger function
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func)
    {
        return func(instance, pCreateInfo, pAllocator, debugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
bool VulkanDebugger::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::cout << "\n";
    std::cout << "available Layers: " << std::endl;
    for (auto &pro: availableLayers)
    {
        std::cout << pro.layerName << "\n";
    }

    auto getEnableValidationLayers = []() { return s_validationLayers; };

    /// check validations
    std::cout << "\n";
    std::cout << "Required Layers supported:" << std::endl;
    const auto &enableValidationLayers = getEnableValidationLayers();
    for (auto &validation: enableValidationLayers)
    {
        auto it = std::find_if(availableLayers.begin(), availableLayers.end(), [&validation](VkLayerProperties &proper)
                               { return strcmp(proper.layerName, validation) == 0; });

        std::cout << validation << "\t: ";
        if (it == availableLayers.end())
        {
            std::cout << "false" << std::endl;
            return false;
        }

        std::cout << "true" << std::endl;
    }

    return true;
}
const std::vector<const char *> &VulkanDebugger::getRequiredLayers() { return s_validationLayers; }

void VulkanDebugger::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
}
std::vector<const char *> VulkanDebugger::getRequiredExtensionName()
{
    if (enableValidationLayers())
    {
        return {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    }

    return {};
}
