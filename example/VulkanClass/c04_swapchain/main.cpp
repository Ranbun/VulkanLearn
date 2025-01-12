#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    VulkanApplication app("c03_surface", 640, 480);

    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    /// create swap chain
    /// 获取物理设备熟悉
    VkSurfaceCapabilitiesKHR caps;
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app.getPhysicalDevice(), app.getSurface(), &caps);

    if(result != VK_SUCCESS)
    {
        std::cout<<"Failed to get capabilities."<< std::endl;
        return 1;
    }

    /// 获取物理设备的图像格式 & 图像的呈现方式
    uint32_t formatCount = 0, presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(app.getPhysicalDevice(), app.getSurface(), &formatCount, nullptr);

    if(result != VK_SUCCESS || formatCount == 0)
    {
        std::cout<<"Failed to get surface formats."<< std::endl;
        return 1;
    }
    std::vector<VkSurfaceFormatKHR> imageFormat(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(app.getPhysicalDevice(), app.getSurface(), &formatCount, imageFormat.data());

    result = vkGetPhysicalDeviceSurfacePresentModesKHR(app.getPhysicalDevice(), app.getSurface(), &presentModeCount, nullptr);

    if(result != VK_SUCCESS || presentModeCount == 0)
    {
        std::cout<<"Failed to get surface present mode."<< std::endl;
        return 1;
    }
    std::vector<VkPresentModeKHR> imagePresentMode(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(app.getPhysicalDevice(), app.getSurface(), &presentModeCount, imagePresentMode.data());

    while (!app.shouldClose())
    {
        glfwPollEvents();
    }

    return 0;
}
