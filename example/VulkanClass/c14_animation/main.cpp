#include "BufferManager.h"
#include "VulkanApplication.h"

#include <cstring>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <vector>

VkSampler createTextureSampler(VulkanApplication &app)
{
    VkPhysicalDeviceProperties prperties;
    vkGetPhysicalDeviceProperties(app.getPhysicalDevice(), &prperties);
    VkSamplerCreateInfo texture_sampler_create_info{.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                                    .pNext = nullptr,
                                                    .flags = 0,
                                                    .magFilter = VK_FILTER_LINEAR,
                                                    .minFilter = VK_FILTER_NEAREST,
                                                    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                                                    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                    .mipLodBias = 0.0,
                                                    .anisotropyEnable = VK_TRUE,
                                                    .maxAnisotropy = prperties.limits.maxSamplerAnisotropy,
                                                    .compareEnable = VK_FALSE,
                                                    .compareOp = VK_COMPARE_OP_ALWAYS,
                                                    .minLod = 0.0f,
                                                    .maxLod = 0.0f,
                                                    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
                                                    .unnormalizedCoordinates = VK_FALSE};

    VkSampler texture_sampler = VK_NULL_HANDLE;
    vkCreateSampler(app.getLogicDevice(), &texture_sampler_create_info, nullptr, &texture_sampler);

    return texture_sampler;
}


VkImageView createTextureImageView(VulkanApplication &app, VkImage &texture_image)
{
    VkImageViewCreateInfo image_view_create_info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = texture_image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                           VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    VkImageView image_view = VK_NULL_HANDLE;

    if (const auto result = vkCreateImageView(app.getLogicDevice(), &image_view_create_info, nullptr, &image_view);
        result != VK_SUCCESS)
    {
        std::cerr << "Create ImageView Failed!" << std::endl;
        return VK_NULL_HANDLE;
    }
    return image_view;
}

int main(int argc, char **args)
{
    int width = 1024;
    int height = 768;

    VulkanApplication app("c14 animation", width, height);
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
    vertices.emplace_back(0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    vertices.emplace_back(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    vertices.emplace_back(0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    vertices.emplace_back(1.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

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

    UniformBufferObject ubo;

    // manager.unmapBufferMemory("ubo_1");

    /// create texture object
    VkImage texture_image = manager.createTextureImage("mouten", "./sources/mouten.jpg");
    [[maybe_unused]] VkImageView texture_image_view = createTextureImageView(app, texture_image);
    /// 为了使用imageView, 需要创建一个采样器
    [[maybe_unused]] auto texture_sampler = createTextureSampler(app);

    VkDescriptorImageInfo image_info{
            .sampler = texture_sampler,
            .imageView = texture_image_view,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    VkWriteDescriptorSet write_descriptor_set{
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, manager.getDescriptorSet(), 1, 0, 1,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            &image_info,
            nullptr,
            nullptr};

            vkUpdateDescriptorSets(app.getLogicDevice(), 1, &write_descriptor_set, 0, nullptr);

    auto waitFence = app.getOrCreateFence("WaitFence");
    auto waitNextImage = app.getOrCreateSemaphore("WaitNextImage");
    auto waitSubmission = app.getOrCreateSemaphore("WaitSubmission");

    const auto &commandBuffer = app.getCommandBuffer();

    float angleZ = 30.0f;
    auto aspecRatio = 1024.0f / 768.0f;

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
                std::cout << "Next Image: " << imageIndex << '\n';
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

        auto rotation = glm::rotate<float>(glm::mat4(1.0), glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));
        auto up = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

        ubo.model_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), up);
        ubo.project = glm::perspective(45.0f, aspecRatio, 0.1f, 100.0f);

        [[maybe_unused]] auto ubo_buffer = manager.CreateUniformBuffer("ubo_1", sizeof(UniformBufferObject));
        auto ubo_data = manager.mapBufferMemory("ubo_1", sizeof(UniformBufferObject));
        memcpy(ubo_data, &ubo, sizeof(UniformBufferObject));

        angleZ += 0.1f;

        /// 每一帧 往buffer添加指令
        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,
                                           VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr};

        vkResetCommandBuffer(commandBuffer, 0); /// reset 重置命令缓冲
        vkBeginCommandBuffer(commandBuffer, &beginInfo); /// 开始记录指令缓冲
        {
            // TODO: 添加渲染指令
            VkClearValue color = {{0.2f, 0.2f, 0.4f, 1.0f}};
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

    manager.unmapBufferMemory("ubo_1");

    vkWaitForFences(app.getLogicDevice(), 1, &waitFence, VK_TRUE, UINT64_MAX);
    vkDeviceWaitIdle(app.getLogicDevice());

    vkDestroySampler(app.getLogicDevice(), texture_sampler, nullptr);
    vkDestroyImageView(app.getLogicDevice(), texture_image_view, nullptr);
    vkDestroyImage(app.getLogicDevice(), texture_image, nullptr);
    manager.cleanup();

    return 0;
}
