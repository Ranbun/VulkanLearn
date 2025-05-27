#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    VulkanApplication app("c05_render loop", 1024, 768);

    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    /// create swapchain image view
    VkImageViewCreateInfo imageViewCreateInfo {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        nullptr, 0,
        nullptr, VK_IMAGE_VIEW_TYPE_2D,
        app.getSurfaceFormat().format,
        {
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY ,
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
        },
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    };

    const size_t imageSize = app.getSwapChainImage().size();
    std::vector<VkImageView> swapChainImageViews(imageSize);
    for (size_t i = 0; i < imageSize; i++)
    {
        imageViewCreateInfo.image = app.getSwapChainImage()[i];
        auto res = vkCreateImageView(app.getLogicDevice(), &imageViewCreateInfo, nullptr, &swapChainImageViews[i]);
        if (res != VK_SUCCESS)
        {
            throw std::runtime_error("vkCreateImageView Failed!");
            return 1;
        }
    }


    /// 创建渲染循环
    // render loop
    VkFence waitFence = nullptr;
    VkSemaphore semaphore = nullptr;

    VkFenceCreateInfo fenceCreateInfo = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr,
        VK_FENCE_CREATE_SIGNALED_BIT 

    };

    /// 栅栏 用于CPU & GPU之间的同步 
    auto res = vkCreateFence(app.getLogicDevice(), &fenceCreateInfo, nullptr, &waitFence);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("vkCreateFence Failed!");
        return 1;
    }

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,  nullptr, 0
    };

    // 信号量用于GPU之间
    res = vkCreateSemaphore(app.getLogicDevice(), &semaphoreCreateInfo, nullptr, &semaphore);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("vkCreateSemaphore Failed!");
        return 1;
    }

    while (!app.shouldClose())
    {
        /// 等待 Fence 
        // vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT64_MAX);
        /// 重置 fence 设置为不通过通过 
        // vkResetFences(app.getLogicDevice(), 1, &waitFence);

        // 从交换链获取渲染可用的图
        uint32_t imageIndex = 0;
        auto result = vkAcquireNextImageKHR(app.getLogicDevice(), app.getSwapChain(), UINT64_MAX, semaphore, VK_NULL_HANDLE, &imageIndex);


        switch (result)
        {
            case VK_SUCCESS:
                std::cout << "Next Image: " << imageIndex;
                break;
            // default:
                // app.setClose();
                // break;
                    
        }


        glfwPollEvents();
    }

    for (auto i = 0; i < imageSize; i++)
    {
        vkDestroyImageView(app.getLogicDevice(), swapChainImageViews[i], nullptr);
    }

    vkDestroyFence(app.getLogicDevice(), waitFence, nullptr);
    vkDestroySemaphore(app.getLogicDevice(), semaphore, nullptr);

    return 0;
}
