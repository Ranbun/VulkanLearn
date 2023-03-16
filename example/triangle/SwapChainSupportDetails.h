#ifndef SWAPCHAINSUPPORTDETAILS_H_
#define SWAPCHAINSUPPORTDETAILS_H_

#include <vector>
#include <vulkan/vulkan.h>

struct SwapChainSupportDetails
{
    /**
     * @brief 交换链表面基础特性
     */
    VkSurfaceCapabilitiesKHR m_capabilities{};

    /**
     * @brief 表面格式
     */
    std::vector<VkSurfaceFormatKHR> m_format;

    /**
     * @brief 可用的呈现模式
     */
    std::vector<VkPresentModeKHR> m_presentModes;
};


#endif
