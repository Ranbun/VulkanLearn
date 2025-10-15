#include "BufferManager.h"
#include "Matrix.hpp"
#include "VulkanApplication.h"

#include <iostream>
#include <vector>
#include <cstring>

int main(int argc, char **args)
{
    int width = 1024;
    int height = 768;
    VulkanApplication app("c12_uniform_buffer", 1024, 768);
    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

    // create uniform description layout
    VkDescriptorSetLayoutBinding descriptor_set_layout_binding
    {
        0, ///binding point
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        1,  ///  可以是多个 -- 看shader里面资源的个数 如果资源是个数组 则是数组的大小
        VK_SHADER_STAGE_VERTEX_BIT,   /// 哪个阶段可用
        nullptr
    };

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info
    {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr, 0,
        1, &descriptor_set_layout_binding
    };

    VkDescriptorSetLayout descriptor_set_layout = nullptr;
    if (auto res = vkCreateDescriptorSetLayout(app.getLogicDevice(), &descriptor_set_layout_create_info, nullptr, &descriptor_set_layout);
        res != VK_SUCCESS)
    {
        throw std::runtime_error("Failed: Create VkDescriptorSetLayout");
    }

    VkDescriptorPoolSize pool_size{
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info{
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        nullptr, 0,
        1, 1, &pool_size
    };

    VkDescriptorPool descriptor_pool = nullptr;
    if (auto res= vkCreateDescriptorPool(app.getLogicDevice(), &descriptor_pool_create_info, nullptr, &descriptor_pool);
        res != VK_SUCCESS)
    {
        throw std::runtime_error("Failed: Create DescriptorPool");
    }

    /// create uniform descriptor set
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info{
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr, descriptor_pool, 1, &descriptor_set_layout
    };

    VkDescriptorSet descriptor_set  = nullptr;
    if (auto res = vkAllocateDescriptorSets(app.getLogicDevice(), &descriptor_set_allocate_info, &descriptor_set);
        res != VK_SUCCESS)
    {
        throw std::runtime_error("Failed: Allocate DescriptorSets");
    }

    VKL::BufferManager manager(&app);
    if (!manager.prepare("./shaders/sample_vert.spv", "./shaders/sample_frag.spv", descriptor_set_layout))
    {
        throw std::runtime_error("Failed to Create pipeline");
    }

    /// create data
    std::vector<VKL::Vertex> vertices;
    vertices.emplace_back(0.0f, 0.5f,0.0f, 1.0f,0.0f, 0.0f);
    vertices.emplace_back(-0.5f, -0.5f,0.0f, 0.0f,1.0f, 0.0f);
    vertices.emplace_back(0.5f, -0.5f,0.0f, 0.0f,0.0f, 1.0f);
    vertices.emplace_back(1.0f, 0.5f,0.0f, 1.0f,0.0f, 0.0f);

    std::vector<uint32_t> indices;
    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
    indices.emplace_back(0);
    indices.emplace_back(2);
    indices.emplace_back(3);

    /// 创建顶点缓存
    auto vk_vertex_buffer = manager.CreateVertexBuffer("test_vertex_buffer",vertices);
    auto vk_index_buffer = manager.CreateIndexBuffer("test_index_buffer", indices);

    // create uniform buffer object
    struct UniformBufferObject
    {
        VKL::Matrix model_view;
        VKL::Matrix project;
    };

    UniformBufferObject ubo;
    ubo.model_view.makeIdentify();
    ubo.model_view.makeRotate(30.0f, 0.0,0.0, 1.0);
    ubo.project.makeIdentify();
    ubo.project.makeOrtho(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);

    auto ubo_data_size = sizeof(UniformBufferObject);
    VkBuffer ubo_buffer = nullptr;
    VkDeviceMemory ubo_buffer_memory = nullptr;
    if (!manager.CreateBuffer(ubo_buffer, ubo_data_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
    {
        throw std::runtime_error("Failed to Create Uniform buffer object");
    }

    VkMemoryRequirements ubo_buffer_requirements;
    vkGetBufferMemoryRequirements(app.getLogicDevice(), ubo_buffer, &ubo_buffer_requirements);
    manager.CreateBufferMemory(ubo_buffer_memory, ubo_buffer_requirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void * ubo_data = nullptr;
    vkBindBufferMemory(app.getLogicDevice(), ubo_buffer, ubo_buffer_memory, 0);
    vkMapMemory(app.getLogicDevice(), ubo_buffer_memory, 0, ubo_data_size, 0, &ubo_data);
    memcpy(ubo_data, &ubo, ubo_data_size);
    vkUnmapMemory(app.getLogicDevice(), ubo_buffer_memory);

    /// update 描述符集的数据
    VkDescriptorBufferInfo descriptor_buffer_info
    {
        ubo_buffer, 0, ubo_data_size
    };

    VkWriteDescriptorSet write_descriptor_set{
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        nullptr,
        descriptor_set, 0,
        0,1,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        nullptr, &descriptor_buffer_info, nullptr
    };

    vkUpdateDescriptorSets(app.getLogicDevice(), 1, &write_descriptor_set, 0, nullptr);


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
                app.getFrameBuffer()[imageIndex],
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

                /// bind uniform
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app.GetPipelineLayout(), 0, 1, &descriptor_set, 0, nullptr);

                /// bind buffer
                VkBuffer vertex_buffers[] = {vk_vertex_buffer};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffer, 0,1, vertex_buffers, offsets);
                vkCmdBindIndexBuffer(commandBuffer, vk_index_buffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(commandBuffer, indices.size(),1,0,0,0);
            }
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
    vkDeviceWaitIdle(app.getLogicDevice());
    vkDestroyDescriptorPool(app.getLogicDevice(), descriptor_pool, nullptr);
    vkDestroyDescriptorSetLayout(app.getLogicDevice(), descriptor_set_layout, nullptr);
    vkDestroyBuffer(app.getLogicDevice(), ubo_buffer, nullptr);
    vkFreeMemory(app.getLogicDevice(), ubo_buffer_memory, nullptr);
    return 0;
}
