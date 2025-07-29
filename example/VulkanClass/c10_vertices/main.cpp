#include <iostream>
#include <vector>
#include "Vertex.h"

#include "VulkanApplication.h"

int main(int argc, char **args)
{
    int width = 1024;
    int height = 768;

    VulkanApplication app("c10_vertices", 1024, 768);

    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    /// 输入顶点的绑定信息
    VkVertexInputBindingDescription binding{
            0, sizeof(VKL::Vertex), VK_VERTEX_INPUT_RATE_VERTEX
    };

    /// 输入信息的属性信息
    std::vector<VkVertexInputAttributeDescription> attr_attr_list;
    attr_attr_list.emplace_back(0,0, VK_FORMAT_R32G32B32_SFLOAT,0);
    attr_attr_list.emplace_back(1,0, VK_FORMAT_R32G32B32_SFLOAT,sizeof(float) * 3);

    /// 固定功能阶段
    /// 1. 顶点输入
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    /// 输入装配
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    if(!app.setUpGraphicsPipeline("./shaders/sample_vert.spv","./shaders/sample_frag.spv",vertexInputInfo,inputAssembly))
    {
        throw std::runtime_error("Create graphics pipeline error!");
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
                /// 重建交换链的时候 重置栅栏对象
                break;
            default:
                 //app.setClose();s
                break;
        }

        /// 每一帧 往buffer添加指令
        VkCommandBufferBeginInfo beginInfo{
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,
            VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr
        };

        vkResetCommandBuffer(commandBuffer,0);  /// reset 重置命令缓冲
        vkBeginCommandBuffer(commandBuffer, &beginInfo);  /// 开始记录指令缓冲
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
            {
                /// set viewport
                VkViewport viewport = {0.0f,0.0f,static_cast<float>(width),static_cast<float>(height),0.0f,1.0f};
                /// set scissor
                VkRect2D scissor = {0, 0, static_cast<uint32_t >(width), static_cast<uint32_t>(height)};

                /// 绑定图形管线
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app.getGraphicsPipeline());
                vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
                vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
            }
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);
            vkCmdEndRenderPass(commandBuffer);
        }
        const auto command_res = vkEndCommandBuffer(commandBuffer);
        if(command_res != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }

        // 提交指令
        app.submitAndPresent(waitNextImage, waitSubmission,waitFence, imageIndex);

        glfwPollEvents();
    }

    vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT64_MAX);

    return 0;
}
