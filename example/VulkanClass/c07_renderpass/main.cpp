#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    VulkanApplication app("c07_renderpass", 1024, 768);

    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    auto waitFence = app.getOrCreateFence("WaitFence");
    auto waitNextImage = app.getOrCreateSemaphore("WaitNextImage");
    auto waitSubmission = app.getOrCreateSemaphore("WaitSubmission");

    const auto & commandBuffer = app.getCommandBuffer();
    while (!app.shouldClose())
    {
        vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT64_MAX);
        vkResetFences(app.getLogicDevice(), 1, &waitFence);

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

        // 提交指令
        app.submitAndPresent(waitNextImage,waitSubmission,waitFence, imageIndex);

        glfwPollEvents();
    }

    return 0;
}
