#include <iostream>
#include <vector>

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    int width = 1024;
    int height = 768;

    VulkanApplication app("c09_pipeLine", 1024, 768);

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
        /// VKFence 第一次创建之后是signaled的状态
        vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT64_MAX);

        /// VkFence 是一次性的同步标记 使用完成之后需要重置 才可以再次使用
        vkResetFences(app.getLogicDevice(), 1, &waitFence);

        // 从交换链获取渲染可用的图
        uint32_t imageIndex = 0;
        const auto result = vkAcquireNextImageKHR(app.getLogicDevice(), app.getSwapChain(), UINT64_MAX, waitNextImage, VK_NULL_HANDLE, &imageIndex);
        switch (result)
        {
            case VK_SUCCESS:
                std::cout << "Next Image: " << imageIndex << std::endl;
                break;
            case VK_SUBOPTIMAL_KHR:
                /// 交换链部分失效
            case VK_ERROR_OUT_OF_DATE_KHR:
                /// 交换链完全失效
                imageIndex = app.reCreateSwapChain(width, height, waitNextImage);
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
        {
            // TODO: 添加渲染指令
            VkClearValue color = {{0.2,0.2,0.4,1.0}};
            VkRenderPassBeginInfo renderPassBegin{
                VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                nullptr, app.getRenderPass(),
                app.getFramebuffer()[imageIndex],
                {{0,0},{static_cast<uint32_t>(width),static_cast<uint32_t>(height)}},
                1, &color
            };

            vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdEndRenderPass(commandBuffer);
        }
        vkEndCommandBuffer(commandBuffer);

        // 提交指令
        app.submitAndPresent(waitNextImage,waitSubmission,waitFence, imageIndex);

        glfwPollEvents();
    }

    vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT8_MAX);

    return 0;
}
