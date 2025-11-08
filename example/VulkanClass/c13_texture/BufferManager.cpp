#include "BufferManager.h"

#include <cstring>
#include <filesystem>
#include <ranges>
#include <stb_image.h>
#include <utility>

namespace VKL
{
    BufferManager::BufferManager(VulkanApplication *app) : _application(app) {}

    BufferManager::~BufferManager() = default;
    bool BufferManager::createDescriptorSet()
    {
        VkDescriptorSetLayoutBinding ubo_layout_binding{.binding = 0,
                                                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                        .descriptorCount = 1,
                                                        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                                        .pImmutableSamplers = nullptr};

        VkDescriptorSetLayoutBinding sampler_layout_binding{.binding = 1,
                                                            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                            .descriptorCount = 1,
                                                            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                                                            .pImmutableSamplers = nullptr};


        std::vector<VkDescriptorSetLayoutBinding> bindings{ubo_layout_binding, sampler_layout_binding};

        const VkDescriptorSetLayoutCreateInfo layout_create_info{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .bindingCount = static_cast<uint32_t>(bindings.size()),
                .pBindings = bindings.data()};

        VkResult result = vkCreateDescriptorSetLayout(_application->getLogicDevice(), &layout_create_info, nullptr,
                                                      &_descriptor_set_layout);
        if (result != VK_SUCCESS)
        {
            // throw std::runtime_error("Failed to create descriptor-set layout!");
            return false;
        }

        /// create uniform description pool
        VkDescriptorPoolSize pool_size[2] = {{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1},
                                             {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 1}};

        VkDescriptorPoolCreateInfo pool_create_info{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                                                    .pNext = nullptr,
                                                    .flags = 0,
                                                    .maxSets = 1,
                                                    .poolSizeCount = 2,
                                                    .pPoolSizes = pool_size};
        result = vkCreateDescriptorPool(_application->getLogicDevice(), &pool_create_info, nullptr, &_descriptor_pool);
        if (result != VK_SUCCESS)
        {
            // throw std::runtime_error("Failed create descriptor pool!");
            return false;
        }

        VkDescriptorSetAllocateInfo set_allocate_info{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                                      .pNext = nullptr,
                                                      .descriptorPool = _descriptor_pool,
                                                      .descriptorSetCount = 1,
                                                      .pSetLayouts = &_descriptor_set_layout};

        result = vkAllocateDescriptorSets(_application->getLogicDevice(), &set_allocate_info, &_descriptor_set);
        if (result != VK_SUCCESS)
        {
            // throw std::runtime_error("Failed to create descriptor set.");
            return false;
        }
        return true;
    }

    bool BufferManager::prepare(const std::string &vert_file, const std::string &frag_file,
                                const VkDescriptorSetLayout desc_set) const
    {
        VkVertexInputBindingDescription binding_description{
                0,
                sizeof(Vertex),
                VK_VERTEX_INPUT_RATE_VERTEX,
        };

        std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
        attribute_descriptions.emplace_back(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, x));
        attribute_descriptions.emplace_back(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, r));

        VkPipelineVertexInputStateCreateInfo input_state_create_info{
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                nullptr,
                0,
                1,
                &binding_description,
                static_cast<uint32_t>(attribute_descriptions.size()),
                attribute_descriptions.data()};

        VkPipelineInputAssemblyStateCreateInfo input_assembly{
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, nullptr, 0,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE};

        return _application->setUpGraphicsPipeline(vert_file, frag_file, input_state_create_info, input_assembly,
                                                   desc_set);
    }

    void BufferManager::cleanup()
    {
        for (const auto &buffer: _buffer_maps | std::views::values)
        {
            vkDestroyBuffer(_application->getLogicDevice(), buffer, nullptr);
        }

        for (const auto &buffer_memory: _buffer_memory_map | std::views::values)
        {
            vkFreeMemory(_application->getLogicDevice(), buffer_memory, nullptr);
        }

        if (_descriptor_pool)
        {
            vkDestroyDescriptorPool(_application->getLogicDevice(), _descriptor_pool, nullptr);
        }

        if (_descriptor_set_layout)
        {
            vkDestroyDescriptorSetLayout(_application->getLogicDevice(), _descriptor_set_layout, nullptr);
        }
    }

    VkBuffer BufferManager::CreateVertexBuffer(const std::string &name, const std::vector<Vertex> &vertices)
    {
        const size_t data_size = vertices.size() * sizeof(Vertex);
        VkBuffer vertex_buffer = nullptr;
        VkDeviceMemory vertex_buffer_memory = nullptr;

        if (!CreateBuffer(vertex_buffer, data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT))
        {
            return nullptr;
        }

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(_application->getLogicDevice(), vertex_buffer, &memory_requirements);
        if (!CreateBufferMemory(vertex_buffer_memory, memory_requirements,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            return nullptr;
        }

        vkBindBufferMemory(_application->getLogicDevice(), vertex_buffer, vertex_buffer_memory, 0);
        _buffer_maps.emplace(name, vertex_buffer);
        _buffer_memory_map.emplace(name, vertex_buffer_memory);

        void *data = nullptr;
        vkMapMemory(_application->getLogicDevice(), vertex_buffer_memory, 0, data_size, 0, &data);
        memcpy(data, vertices.data(), data_size);
        vkUnmapMemory(_application->getLogicDevice(), vertex_buffer_memory);

        return vertex_buffer;
    }

    VkBuffer BufferManager::CreateIndexBuffer(const std::string &name, const std::vector<uint32_t> &indices)
    {
        const size_t data_size = indices.size() * sizeof(uint32_t);
        VkBuffer index_buffer = nullptr;
        VkDeviceMemory index_buffer_memory = nullptr;

        if (!CreateBuffer(index_buffer, data_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
        {
            return nullptr;
        }

        VkMemoryRequirements buffer_memory_requirements;
        vkGetBufferMemoryRequirements(_application->getLogicDevice(), index_buffer, &buffer_memory_requirements);
        if (!CreateBufferMemory(index_buffer_memory, buffer_memory_requirements,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            return nullptr;
        }

        vkBindBufferMemory(_application->getLogicDevice(), index_buffer, index_buffer_memory, 0);
        _buffer_maps.emplace(name, index_buffer);
        _buffer_memory_map.emplace(name, index_buffer_memory);

        void *data = nullptr;
        vkMapMemory(_application->getLogicDevice(), index_buffer_memory, 0, data_size, 0, &data);
        memcpy(data, indices.data(), data_size);
        vkUnmapMemory(_application->getLogicDevice(), index_buffer_memory);
        return index_buffer;
    }

    VkBuffer BufferManager::CreateUniformBuffer(const std::string &name, size_t ubo_size)
    {
        VkBuffer uniform_buffer = nullptr;
        VkDeviceMemory uniform_buffer_memory = nullptr;

        if (!CreateBuffer(uniform_buffer, ubo_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
        {
            // throw std::runtime_error("create uniform buffer error!");
            return nullptr;
        }
        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(_application->getLogicDevice(), uniform_buffer, &memory_requirements);
        if (!CreateBufferMemory(uniform_buffer_memory, memory_requirements,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            // throw std::runtime_error("Create uniform buffer memory error!");
            return nullptr;
        }

        vkBindBufferMemory(_application->getLogicDevice(), uniform_buffer, uniform_buffer_memory, 0);
        _buffer_maps[name] = uniform_buffer;
        _buffer_memory_map[name] = uniform_buffer_memory;

        /// update description set
        VkDescriptorBufferInfo buffer_info{.buffer = uniform_buffer, .offset = 0, .range = ubo_size};

        VkWriteDescriptorSet write_descriptor_set{
                VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, _descriptor_set, 0,      0, 1,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,      nullptr, &buffer_info,    nullptr};

        vkUpdateDescriptorSets(_application->getLogicDevice(), 1, &write_descriptor_set, 0, nullptr);

        return uniform_buffer;
    }

    bool BufferManager::CreateBuffer(VkBuffer &buffer, const size_t data_size, const VkBufferUsageFlags usage) const
    {
        const VkBufferCreateInfo buffer_create_info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                                    nullptr,
                                                    0,
                                                    data_size,
                                                    usage,
                                                    VK_SHARING_MODE_EXCLUSIVE,
                                                    0,
                                                    nullptr};

        if (const auto result = vkCreateBuffer(_application->getLogicDevice(), &buffer_create_info, nullptr, &buffer);
            result != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    bool BufferManager::CreateBufferMemory(VkDeviceMemory &buffer_memory,
                                           const VkMemoryRequirements &memory_requirements,
                                           uint32_t property_bits) const
    {
        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(_application->getPhysicalDevice(), &memory_properties);

        uint32_t memory_type_index = 0;
        for (auto i = 0; std::cmp_less(i, memory_properties.memoryTypeCount); i++)
        {
            if ((memory_requirements.memoryTypeBits & (1 << i)) &&
                (memory_properties.memoryTypes[i].propertyFlags & property_bits) == property_bits)
            {
                memory_type_index = i;
            }
        }

        const VkMemoryAllocateInfo allocate_info{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr,
                                                 memory_requirements.size, memory_type_index};

        if (const auto result =
                    vkAllocateMemory(_application->getLogicDevice(), &allocate_info, nullptr, &buffer_memory);
            result != VK_SUCCESS)
        {
            // throw std::runtime_error("Failed to Create buffer memory.");
            return false;
        }
        return true;
    }

    VkBuffer BufferManager::getBuffer(const std::string &name) const
    {
        if (_buffer_maps.contains(name))
        {
            return _buffer_maps.at(name);
        }

        return nullptr;
    }

    void *BufferManager::mapBufferMemory(const std::string &name, size_t data_size) const
    {
        void *buffer_data_ptr = nullptr;
        if (_buffer_memory_map.contains(name))
        {
            const auto &buffer_memory = _buffer_memory_map.at(name);
            vkMapMemory(_application->getLogicDevice(), buffer_memory, 0, data_size, 0, &buffer_data_ptr);
        }
        return buffer_data_ptr;
    }

    void BufferManager::unmapBufferMemory(const std::string &name) const
    {
        if (_buffer_memory_map.contains(name))
        {
            const auto &buffer_memory = _buffer_memory_map.at(name);
            vkUnmapMemory(_application->getLogicDevice(), buffer_memory);
        }
    }

    VkImage BufferManager::createTextureImage(const std::string &name, const std::string &file)
    {
        /// load image by stb_image
        int w = 0, h = 0, ch = 4;
        /// STBI_rgb_alpha 使用这个flag, 强制把图像加载成RGBA格式
        stbi_uc *pixels = stbi_load(file.c_str(), &w, &h, &ch, STBI_rgb_alpha);
        std::cout << std::filesystem::current_path() << std::endl;
        if (pixels == nullptr)
        {
            throw std::runtime_error("Failed to load image file!");
        }

        /// TODO: 图像通道没有做处理 记得做这个事情

        /// create staging buffer
        /// use loaded image pixels data create image buffer
        ch = 4; /// 前面强制使用了四通道，这个地方也需要修改， 保证创建的buffer的大小是正确的
        VkDeviceSize image_size =
                static_cast<VkDeviceSize>(w) * static_cast<VkDeviceSize>(h) * static_cast<uint64_t>(ch);
        VkBuffer image_buffer = nullptr;
        VkDeviceMemory image_buffer_memory = nullptr;
        if (!CreateBuffer(image_buffer, image_size,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) /// 创建这个bufer,作为传输源使用
        {
            return VK_NULL_HANDLE;
        }

        /// create buffer memory
        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(_application->getLogicDevice(), image_buffer, &memory_requirements);
        /// 必须是 cpu 可见内存，并且cpu写了之后可以在gpu读取时立即在可见
        if (!CreateBufferMemory(image_buffer_memory, memory_requirements,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            return VK_NULL_HANDLE;
        }

        void *image_data = nullptr;
        vkBindBufferMemory(_application->getLogicDevice(), image_buffer, image_buffer_memory, 0);
        vkMapMemory(_application->getLogicDevice(), image_buffer_memory, 0, image_size, 0, &image_data);
        memcpy(image_data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(_application->getLogicDevice(), image_buffer_memory);

        /// 数据被拷贝到staging buffer之后 可以释放像素数据 之后创建VkImage从staging buffer拷贝数据， 不在需要元数据
        stbi_image_free(pixels);


        /// creat image object
        VkImageCreateInfo image_create_info{.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                            .pNext = nullptr,
                                            .flags = 0,
                                            .imageType = VK_IMAGE_TYPE_2D,
                                            .format = VK_FORMAT_R8G8B8A8_SRGB,
                                            .extent = {static_cast<uint32_t>(w), static_cast<uint32_t>(h), 1},
                                            .mipLevels = 1,
                                            .arrayLayers = 1,
                                            .samples = VK_SAMPLE_COUNT_1_BIT,
                                            .tiling = VK_IMAGE_TILING_OPTIMAL,
                                            .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                            .queueFamilyIndexCount = 0,
                                            .pQueueFamilyIndices = nullptr,
                                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

        VkImage image = VK_NULL_HANDLE;

        VkDeviceMemory result_buffer_memory = nullptr;
        auto result = vkCreateImage(_application->getLogicDevice(), &image_create_info, nullptr, &image);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed create image!");
        }

        VkMemoryRequirements image_memory_requirements;
        vkGetImageMemoryRequirements(_application->getLogicDevice(), image, &image_memory_requirements);
        if (!CreateBufferMemory(result_buffer_memory, image_memory_requirements,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            return VK_NULL_HANDLE;
        }

        /// 绑定内存和VkImage对象
        vkBindImageMemory(_application->getLogicDevice(), image, result_buffer_memory, 0);

        /// 创建一个临时的command buffer 来执行我们的拷贝命令
        /// copy from src to dst
        VkCommandBufferAllocateInfo command_buffer_allocate_info{.sType =
                                                                         VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                                 .pNext = nullptr,
                                                                 .commandPool = _application->getCommandPool(),
                                                                 .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                 .commandBufferCount = 1};

        VkCommandBuffer tempCommand = nullptr;
        vkAllocateCommandBuffers(_application->getLogicDevice(), &command_buffer_allocate_info, &tempCommand);

        /// starting copy
        constexpr VkCommandBufferBeginInfo begin_info{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                      .pNext = nullptr,
                                                      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                                      .pInheritanceInfo = nullptr};

        vkBeginCommandBuffer(tempCommand, &begin_info);

        /// 还需要提交-- 用于转换创建的image的layout，不能向一个 UNDEFINED的图像写入数据
        VkImageMemoryBarrier per_barrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                         .pNext = nullptr,
                                         .srcAccessMask = 0,
                                         .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                         .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                         .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                         .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .image = image,
                                         .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
        vkCmdPipelineBarrier(tempCommand, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &per_barrier);

        /// 从 vkbuffer 把数据拷贝到image中
        VkBufferImageCopy region{.bufferOffset = 0,
                                 .bufferRowLength = 0,
                                 .bufferImageHeight = 0,
                                 .imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                                 .imageOffset = {0, 0, 0},
                                 .imageExtent = {static_cast<uint32_t>(w), static_cast<uint32_t>(h), 1}};
        vkCmdCopyBufferToImage(tempCommand, image_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


        /// 图片拷贝完成之后，需要把格式转换为着色器可以识别的格式
        VkImageMemoryBarrier post_barrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                          .pNext = nullptr,
                                          .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                          .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                          .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                          .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                          .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                          .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                          .image = image,
                                          .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

        vkCmdPipelineBarrier(tempCommand, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &post_barrier);

        /// finish copying image
        VkQueue queue = nullptr;
        vkGetDeviceQueue(_application->getLogicDevice(), 0, 0, &queue);

        vkEndCommandBuffer(tempCommand);

        /// 创建新的提交对象
        VkSubmitInfo submit_info{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                 .pNext = nullptr,
                                 .waitSemaphoreCount = 0,
                                 .pWaitSemaphores = nullptr,
                                 .pWaitDstStageMask = nullptr,
                                 .commandBufferCount = 1,
                                 .pCommandBuffers = &tempCommand,
                                 .signalSemaphoreCount = 0,
                                 .pSignalSemaphores = nullptr};

        vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        vkFreeCommandBuffers(_application->getLogicDevice(), _application->getCommandPool(), 1, &tempCommand);
        vkFreeMemory(_application->getLogicDevice(), image_buffer_memory, nullptr);
        vkDestroyBuffer(_application->getLogicDevice(), image_buffer, nullptr);

        _image_map[name] = image;
        _buffer_memory_map[name] = result_buffer_memory;

        return image;
    }

} // namespace VKL
