#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    VulkanApplication app("c06_submitting", 1024, 768);

    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    /// create command pool
    VkCommandPoolCreateInfo poolInfo{
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        0 // 队列族的号
    };

    /// 创建指令池
    VkCommandPool commandPool = nullptr;
    auto res = vkCreateCommandPool(app.getLogicDevice(), &poolInfo, nullptr, &commandPool);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed create command pool." << std::endl;
        return 1;
    }

    // 创建指令缓存  -- 缓存发送的指令
    VkCommandBufferAllocateInfo allocInfo{
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO , nullptr,
        commandPool,
        VK_COMMAND_BUFFER_LEVEL_PRIMARY, // 主指令缓存
        1 // 分配一个指令缓存
    };

    VkCommandBuffer commandBuffer = nullptr;
    res = vkAllocateCommandBuffers(app.getLogicDevice(), &allocInfo, &commandBuffer);
    if(res != VK_SUCCESS)
    {
        std::cout << "Failed allocate command buffer." << std::endl;
        return 1;
    }

    auto waitFence = app.getOrCreateFence("WaitFence");
    auto waitNextImage = app.getOrCreateSemaphore("WaitNextImage");
    auto waitSubmission = app.getOrCreateFence("WaitSubmission");


    while (!app.shouldClose())
    {
        // 从交换链获取渲染可用的图
        uint32_t imageIndex = 0;
        auto result = vkAcquireNextImageKHR(app.getLogicDevice(), app.getSwapChain(), UINT64_MAX, waitNextImage, VK_NULL_HANDLE, &imageIndex);


        switch (result)
        {
            case VK_SUCCESS:
                std::cout << "Next Image: " << imageIndex;
                break;
            default:
                 //app.setClose();
                break;

        }

        /// 每一帧 往buffer添加指令
        VkCommandBufferBeginInfo beginInfo{
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,
            0, nullptr
        };

        VkResetCommandBuffer(commandBuffer,0);
        VkBenginCommandBuffer(commandBuffer, &beginInfo);
        // TODO: 添加渲染指令
        VkEndCommandBuffer(commandBuffer);

        /// 提交指令到GPU - VULKAN

        VkSubmitInfo submitInfo {
            VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr,
            1, &waitNextImage,    // 等待信号量  -- 照片是不是执行获取完成
            0, nullptr,            // 不需要等待的信号量
            1, &commandBuffer      // 提交的指令缓存
        };


        glfwPollEvents();
    }

    return 0;
}
