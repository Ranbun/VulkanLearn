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


    // 从指令池中获取一个指令缓冲
    VkCommandBuffer commandBuffer = nullptr;
    res = vkAllocateCommandBuffers(app.getLogicDevice(), &allocInfo, &commandBuffer);
    if(res != VK_SUCCESS)
    {
        std::cout << "Failed allocate command buffer." << std::endl;
        return 1;
    }

    auto waitFence = app.getOrCreateFence("WaitFence");
    auto waitNextImage = app.getOrCreateSemaphore("WaitNextImage");
    auto waitSubmission = app.getOrCreateSemaphore("WaitSubmission");

    VkQueue queue = nullptr;
    vkGetDeviceQueue(app.getLogicDevice(), 0, 0, &queue);

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

        vkResetCommandBuffer(commandBuffer,0);
        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        // TODO: 添加渲染指令

        vkEndCommandBuffer(commandBuffer);


        // 提交之前判断下一帧图像是否准备好？ 当前的渲染流水线什么时候等待
        VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        /// 提交指令到GPU - VULKAN
        VkSubmitInfo submitInfo {
            VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr,
            1,
            &waitNextImage,     // 等待信号量  -- 照片是不是执行获取完成
            &waitStageMask,
            1, &commandBuffer,
            1, &waitSubmission      // 提交的指令缓存  执行完成之后通知这个信号量  告诉等待这个信号量的地方开始执行
        };

        vkQueueSubmit(queue, 1, &submitInfo, waitFence);

        VkSwapchainKHR swapChains[] = {app.getSwapChain()};
        VkPresentInfoKHR presentInfo{
            VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            nullptr,
            1, &waitSubmission,
            1, swapChains,
            &imageIndex,
            nullptr
        };

        vkQueuePresentKHR(queue, &presentInfo);



        glfwPollEvents();
    }

    vkDestroyCommandPool(app.getLogicDevice(), commandPool, nullptr);

    return 0;
}
