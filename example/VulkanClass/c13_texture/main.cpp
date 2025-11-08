#include "BufferManager.h"
#include "VulkanApplication.h"

#include <cstring>
#include <filesystem>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <vector>

int main(int argc, char **args)
{
    int width = 1024;
    int height = 768;

    VulkanApplication app("c13 texture", width, height);
    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    VKL::BufferManager manager(&app);
    manager.createDescriptorSet();
    auto descriptor_set = manager.getDescriptorSet();
    if (auto descriptor_set_layout = manager.getDescriptorSetLayout();
        !manager.prepare("./shaders/sample_vert.spv", "./shaders/sample_frag.spv", descriptor_set_layout))
    {
        throw std::runtime_error("Failed to Create pipeline");
    }

    /// create data
    std::vector<VKL::Vertex> vertices;
    vertices.emplace_back(0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f);
    vertices.emplace_back(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f);
    vertices.emplace_back(0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f);
    vertices.emplace_back(1.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f);

    std::vector<uint32_t> indices;
    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
    indices.emplace_back(0);
    indices.emplace_back(2);
    indices.emplace_back(3);

    /// 创建顶点缓存
    auto vk_vertex_buffer = manager.CreateVertexBuffer("test_vertex_buffer", vertices);
    auto vk_index_buffer = manager.CreateIndexBuffer("test_index_buffer", indices);

    // create uniform buffer object
    struct UniformBufferObject
    {
        glm::mat4 model_view;
        glm::mat4 project;
    };

    auto aspecRatio = 1024.0f / 768.0f;
    auto rotation = glm::rotate<float>(glm::mat4(1.0), glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    auto up = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    UniformBufferObject ubo;
    ubo.model_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), up);
    ubo.project = glm::perspective(45.0f, aspecRatio, 0.1f, 100.0f);

    [[maybe_unused]] auto ubo_buffer = manager.CreateUniformBuffer("ubo_1", sizeof(UniformBufferObject));
    auto ubo_data = manager.mapBufferMemory("ubo_1", sizeof(UniformBufferObject));
    memcpy(ubo_data, &ubo, sizeof(UniformBufferObject));
    manager.unmapBufferMemory("ubo_1");

    /// create texture object
    VkImage texture_image = manager.createTextureImage("mountain", "./sources/mountain.JPG");

    auto waitFence = app.getOrCreateFence("WaitFence");
    auto waitNextImage = app.getOrCreateSemaphore("WaitNextImage");
    auto waitSubmission = app.getOrCreateSemaphore("WaitSubmission");

    const auto &commandBuffer = app.getCommandBuffer();
    while (!app.shouldClose())
    {
        /// VKFence 第一次创建之后是signaled的状态
        vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT64_MAX);

        /// VkFence 是一次性的同步标记 使用完成之后需要重置 才可以再次使用
        vkResetFences(app.getLogicDevice(), 1, &waitFence);

        // 从交换链获取渲染可用的图
        uint32_t imageIndex = 0;
        const auto result = vkAcquireNextImageKHR(app.getLogicDevice(), app.getSwapChain(), UINT64_MAX, waitNextImage,
                                                  VK_NULL_HANDLE, &imageIndex);
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
                break;
        }

        /// 每一帧 往buffer添加指令
        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,
                                           VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr};

        vkResetCommandBuffer(commandBuffer, 0); /// reset 重置命令缓冲
        vkBeginCommandBuffer(commandBuffer, &beginInfo); /// 开始记录指令缓冲
        {
            // TODO: 添加渲染指令
            VkClearValue color = {{0.2, 0.2, 0.4, 1.0}};
            VkRenderPassBeginInfo renderPassBegin{
                    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                    nullptr,
                    app.getRenderPass(),
                    app.getFrameBuffer()[imageIndex],
                    {{0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}},
                    1,
                    &color};

            vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
            {
                /// set viewport
                VkViewport viewport = {0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f};
                /// set scissor
                VkRect2D scissor = {0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

                /// 绑定图形管线
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app.getGraphicsPipeline());
                vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
                vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

                /// bind uniform
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app.GetPipelineLayout(), 0, 1,
                                        &descriptor_set, 0, nullptr);

                /// bind buffer
                VkBuffer vertex_buffers[] = {vk_vertex_buffer};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffers, offsets);
                vkCmdBindIndexBuffer(commandBuffer, vk_index_buffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
            }
            vkCmdEndRenderPass(commandBuffer);
        }
        const auto command_res = vkEndCommandBuffer(commandBuffer);
        if (command_res != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }

        // 提交指令
        app.submitAndPresent(waitNextImage, waitSubmission, waitFence, imageIndex);

        glfwPollEvents();
    }

    vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT64_MAX);
    vkDeviceWaitIdle(app.getLogicDevice());

    vkDestroyImage(app.getLogicDevice(), texture_image, nullptr);
    manager.cleanup();

    return 0;
}
