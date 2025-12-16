#include "VulkanFeatureManager.h"

#include <iostream>


VulkanFeatureManager::VulkanFeatureManager()
{
}

void VulkanFeatureManager::requestFeature(EngineFeature feature)
{
    switch (feature)
    {
    case EngineFeature::SwapChain:
        {
            requestDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            break;
        }
    case EngineFeature::DebugUtils:
        {
            requestInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            break;
        }
    }
}

void VulkanFeatureManager::requestInstanceExtension(const char* extName)
{
    if (!m_uniqueInstanceExtensions.contains(extName))
    {
        m_instanceExtensions.push_back(extName);
        m_uniqueInstanceExtensions.insert(extName);
    }
}

void VulkanFeatureManager::requestDeviceExtension(const char* extName)
{
    if (!m_uniqueDeviceExtensions.contains(extName))
    {
        m_deviceExtensions.emplace_back(extName);
        m_uniqueDeviceExtensions.insert(extName);
    }
}

const std::vector<const char*>& VulkanFeatureManager::getEnabledInstanceExtensions() const
{
    return m_instanceExtensions;
}

const std::vector<const char*>& VulkanFeatureManager::getEnabledDeviceExtensions() const { return m_deviceExtensions; }

const std::vector<const char*>& VulkanFeatureManager::getValidationLayers() const { return m_validationLayers; }

void VulkanFeatureManager::enableValidationLayers(bool enable)
{
    m_validationEnabled = enable;
    if (enable)
    {
        m_validationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
        requestFeature(EngineFeature::DebugUtils);
    }
    else
    {
        m_validationLayers.clear();
    }
}

bool VulkanFeatureManager::validateDeviceSupport(VkPhysicalDevice device) const
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_uniqueDeviceExtensions.begin(), m_uniqueDeviceExtensions.end());
    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    if (!requiredExtensions.empty())
    {
        std::cout << "Device Missing Extensions:" << std::endl;
        for (auto& extension : requiredExtensions)
        {
            std::cerr << "\t" << extension << std::endl;
        }
        return false;
    }

    return true;
}

bool VulkanFeatureManager::checkValidationLayerSupport() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::set<std::string> requirementLayers{m_validationLayers.begin(), m_validationLayers.end()};

    /// check validations
    for (auto& layer : availableLayers)
    {
        requirementLayers.erase(std::string(layer.layerName));
    }

    if (!requirementLayers.empty())
    {
        std::cout << "Some Layers is not supported!" << std::endl;
        for (auto& layer : requirementLayers)
        {
            std::cout << "\t" << layer << std::endl;
        }
        return false;
    }

    return true;
}

bool VulkanFeatureManager::checkInstanceExtensionSupport() const
{
    /// Vulkan Instance Extensions
    uint32_t availableExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

    std::set<std::string> requirementInstanceExtensions{
        m_uniqueInstanceExtensions.begin(),
        m_uniqueInstanceExtensions.end()
    };
    for (auto& extension : availableExtensions)
    {
        requirementInstanceExtensions.erase(extension.extensionName);
    }

    if (!requirementInstanceExtensions.empty())
    {
        std::cout << "Some Extensions is Not supported: " << std::endl;
        for (auto& extension : requirementInstanceExtensions)
        {
            std::cerr << "\t" << extension << std::endl;
        }
        return false;
    }


    return true;
}
