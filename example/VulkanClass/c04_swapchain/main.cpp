#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    VulkanApplication app("c04_swapchain", 1000, 800);

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

    VkSwapchainCreateInfoKHR createInfo{
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            nullptr, 0,
            app.getSurface(),
            caps.minImageCount,
            imageFormat[0].format, imageFormat[0].colorSpace,
            caps.maxImageExtent,
            1,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            0, nullptr,
            caps.currentTransform,
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            imagePresentMode[0],
            VK_TRUE, nullptr
    };

    VkSwapchainKHR swapChain = nullptr;

    std::cout<<app.getLogicDevice()<<std::endl;

    result = vkCreateSwapchainKHR(app.getLogicDevice(), &createInfo, nullptr,&swapChain);
    if(result != VK_SUCCESS)
    {
        std::cout<<"Failed  to create swapChain."<< std::endl;
        return 1;
    }

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(app.getLogicDevice(), swapChain, &imageCount, nullptr);

    std::vector<VkImage> images(imageCount);
    vkGetSwapchainImagesKHR(app.getLogicDevice(), swapChain, &imageCount, images.data());

    while (!app.shouldClose())
    {
        glfwPollEvents();
    }

    vkDestroySwapchainKHR(app.getLogicDevice(), swapChain, nullptr);



    return 0;
}
