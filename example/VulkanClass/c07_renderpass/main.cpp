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

    // create render pass
    VkAttachmentDescription colorAttachment {
        0, app.getSurfaceFormat().format,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentRef {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass
    {
        0, VK_PIPELINE_BIND_POINT_GRAPHICS,
        0, nullptr,
        1, & colorAttachmentRef,
        nullptr, nullptr,
        0, nullptr
    };

    VkRenderPassCreateInfo renderPassInfo
    {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        nullptr,  0,
        1, &colorAttachment,
        1,&subpass,
        0,nullptr
    };

    VkRenderPass renderPass;
    auto res = vkCreateRenderPass(app.getLogicDevice(), &renderPassInfo,nullptr, &renderPass);
    if (res != VK_SUCCESS)
    {
        std::cout<<"Failed to create render pass!"<<std::endl;
        return 1;
    }

    std::vector<VkFramebuffer> framebuffers;
    framebuffers.resize(app.getSwapChainImageView().size());
    for (auto i = 0; i < framebuffers.size(); i++)
    {
        VkImageView attachments[] = {app.getSwapChainImageView()[i]};
        VkFramebufferCreateInfo info
        {
            VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            nullptr,0,
            renderPass,
            1, attachments,
            1024,768,1

        };

        vkCreateFramebuffer(app.getLogicDevice(), &info,nullptr, &framebuffers[i]);
    }


    auto waitFence = app.getOrCreateFence("WaitFence");
    auto waitNextImage = app.getOrCreateSemaphore("WaitNextImage");
    auto waitSubmission = app.getOrCreateSemaphore("WaitSubmission");

    const auto & commandBuffer = app.getCommandBuffer();
    while (!app.shouldClose())
    {
        /// VKFence 第一次创建之后是signaled的状态
        vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT64_MAX); ///-> 此后成为unsignaled

        /// VkFence 是一次性的同步标记 使用完成之后需要重置 才可以再次使用
        vkResetFences(app.getLogicDevice(), 1, &waitFence);

        // 从交换链获取渲染可用的图
        uint32_t imageIndex = 0;
        const auto result = vkAcquireNextImageKHR(app.getLogicDevice(), app.getSwapChain(), UINT64_MAX, waitNextImage, VK_NULL_HANDLE, &imageIndex);
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
        VkClearValue color = {{0.2,0.2,0.4,1.0}};
        VkRenderPassBeginInfo renderPassBegin{
            VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            nullptr, renderPass,
            framebuffers[imageIndex],
            {{0,0},{1024,768}},
            1, &color
        };

        vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdEndRenderPass(commandBuffer);

        vkEndCommandBuffer(commandBuffer);

        // 提交指令
        app.submitAndPresent(waitNextImage,waitSubmission,waitFence, imageIndex);

        glfwPollEvents();
    }

    for (auto i = 0; i < framebuffers.size();i++)
    {
        vkDestroyFramebuffer(app.getLogicDevice(), framebuffers[i],nullptr);
    }
    vkDestroyRenderPass(app.getLogicDevice(), renderPass, nullptr);


    return 0;
}
