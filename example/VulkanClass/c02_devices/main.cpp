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

    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    for(auto & layer: layers)
    {
        std::cout << "Layers Name: " << layer.layerName << std::endl;
        std::cout << "Layers Des: " << layer.description << std::endl;
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
    std::cout << "Geomtry: " << bool(physicalDeviceFeatures.geometryShader) << std::endl;
    std::cout << "Tessellation: " << bool(physicalDeviceFeatures.tessellationShader) << std::endl;

    ///  create logsical device
    VkDeviceQueueCreateInfo queueCreateInfo
    {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,0,
        0, 1, /// queueFamily, queueCount
        nullptr
    };

    VkDeviceCreateInfo logicDeviceCreateInfo
    {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr, 0,
        1, &queueCreateInfo,
        0, nullptr,
        0, nullptr,
        &physicalDeviceFeatures
    };

    VkDevice logicDevice = nullptr;
    reslut = vkCreateDevice(masterPhysicalDevice, &logicDeviceCreateInfo, nullptr, &logicDevice);
    if(reslut != VK_SUCCESS)
    {
        std::cout<<"Failed to create logic Device"<<std::endl;
        return 1;
    }

    std::cout<< logicDevice <<std::endl;
    vkDestroyDevice(logicDevice, nullptr);

    /// create swap chain





    return 0;
}
