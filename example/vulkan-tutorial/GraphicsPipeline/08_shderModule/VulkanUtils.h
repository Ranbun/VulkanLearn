#ifndef VULKANUTILS_H_
#define VULKANUTILS_H_

#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

struct QueueFamilyIndices;
struct SwapChainSupportDetails;

namespace VulkanUtils
{
    VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    std::vector<char> readFile(const std::string& filename);
}



#endif
