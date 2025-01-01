#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char ** args)
{
    // std::filesystem::path filePath(__FILE__);

    VulkanApplication app("c02_devices");

    if(!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    /// select physical deveice
    uint32_t deviceCount = 0;
    VkResult reslut = vkEnumeratePhysicalDevices(app.getInstance(), &deviceCount, nullptr);
    if(reslut != VK_SUCCESS || !deviceCount)
    {
        std::cout << "Failed to find physical devices." << std::endl;
        return 1;
    }
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(app.getInstance(), &deviceCount, physicalDevices.data());

    for(auto & device: physicalDevices)
    {
        VkPhysicalDeviceProperties proper;
        vkGetPhysicalDeviceProperties(device, &proper);

        if(proper.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            std::cout << "Device: " << proper.deviceName << "(type = Discrate)" << std::endl;
        }

        if(proper.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            std::cout << "Device: " << proper.deviceName << "(type = Integated)" << std::endl;
        }
    }

    auto masterPhysicalDevice = physicalDevices[0];

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(masterPhysicalDevice, &queueFamilyCount, nullptr);
    if(!deviceCount)
    {
        std::cout << "Failed to find physical devices's queueFamily." << std::endl;
        return 1;
    }

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(masterPhysicalDevice, &queueFamilyCount, queueFamilies.data());
    for(auto & queueFamily: queueFamilies)
    {
        std::cout << "master physical device supports: " << queueFamily.queueFlags << std::endl;
        std::cout << "  Graphics supports: " << bool(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) << std::endl;
        std::cout << "  Compute supports: " << bool(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) << std::endl;
    }

    /// requirement phyiscal device  feature
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(masterPhysicalDevice, &physicalDeviceFeatures);


    ///  create logsical device

    return 0;
}
