#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    // std::filesystem::path filePath(__FILE__);

    VulkanApplication app("c03_surface");

    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    /// vulkan 的调试层
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    for (auto &layer: layers)
    {
        std::cout << "Layers Name: " << layer.layerName << std::endl;
        // std::cout << "Layers Des: " << layer.description << std::endl;
    }

    /// create swap chain

    return 0;
}
