#ifndef SWAPCHAINSUPPORTDETAILS_H_
#define SWAPCHAINSUPPORTDETAILS_H_

#include <vector>
#include <vulkan/vulkan.h>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_capabilities{};
    std::vector<VkSurfaceFormatKHR> m_format;
    std::vector<VkPresentModeKHR> m_presentModes;
};


#endif