#include "BufferManager.h"

#include <ranges>
#include <utility>
#include <cmath>
#include <cstring>

namespace VKL
{
    BufferManager::BufferManager(VulkanApplication *app)
        : application(app)
    {

    }

    BufferManager::~BufferManager() = default;

    bool BufferManager::prepare(const std::string &vert_file, const std::string &frag_file, const VkDescriptorSetLayout desc_set) const
    {
        VkVertexInputBindingDescription binding_description{
                0, sizeof(Vertex),
                VK_VERTEX_INPUT_RATE_VERTEX,
        };

        std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
        attribute_descriptions.emplace_back(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, x));
        attribute_descriptions.emplace_back(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, r));

        VkPipelineVertexInputStateCreateInfo input_state_create_info{
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr, 0,
            1, &binding_description,
                static_cast<uint32_t>(attribute_descriptions.size()), attribute_descriptions.data()
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly{
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            nullptr, 0,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
        };

        return application->setUpGraphicsPipeline(vert_file, frag_file, input_state_create_info, input_assembly, desc_set);
    }

    void BufferManager::cleanup()
    {
        for (const auto & buffer: buffer_maps | std::views::values)
        {
            vkDestroyBuffer(application->getLogicDevice(), buffer, nullptr);
        }

        for (const auto & buffer_memory: buffer_memory_map | std::views::values)
        {
            vkFreeMemory(application->getLogicDevice(), buffer_memory, nullptr);
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
        vkGetBufferMemoryRequirements(application->getLogicDevice(), vertex_buffer, &memory_requirements);
        if (!CreateBufferMemory(vertex_buffer_memory, memory_requirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            return nullptr;
        }

        vkBindBufferMemory(application->getLogicDevice(), vertex_buffer, vertex_buffer_memory, 0);
        buffer_maps.emplace(name, vertex_buffer);
        buffer_memory_map.emplace(name, vertex_buffer_memory);

        void *data = nullptr;
        vkMapMemory(application->getLogicDevice(), vertex_buffer_memory, 0, data_size, 0, &data);
        memcpy(data, vertices.data(), data_size);
        vkUnmapMemory(application->getLogicDevice(), vertex_buffer_memory);

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
        vkGetBufferMemoryRequirements(application->getLogicDevice(), index_buffer, &buffer_memory_requirements);
        if (!CreateBufferMemory(index_buffer_memory, buffer_memory_requirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            return nullptr;
        }

        vkBindBufferMemory(application->getLogicDevice(), index_buffer, index_buffer_memory, 0);
        buffer_maps.emplace(name, index_buffer);
        buffer_memory_map.emplace(name, index_buffer_memory);

        void *data = nullptr;
        vkMapMemory(application->getLogicDevice(), index_buffer_memory, 0, data_size, 0, &data);
        memcpy(data, indices.data(), data_size);
        vkUnmapMemory(application->getLogicDevice(), index_buffer_memory);
        return index_buffer;
    }

    bool BufferManager::CreateBuffer(VkBuffer&buffer, const size_t data_size, const VkBufferUsageFlags usage) const
    {
        const VkBufferCreateInfo buffer_create_info{
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            nullptr, 0,
                data_size,
                usage,
                VK_SHARING_MODE_EXCLUSIVE,
                0, nullptr
        };

        if (const auto result = vkCreateBuffer(application->getLogicDevice(), &buffer_create_info, nullptr, &buffer);
            result != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    bool BufferManager::CreateBufferMemory(VkDeviceMemory&buffer_memory, const VkMemoryRequirements &memory_requirements, uint32_t property_bits) const
    {
        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(application->getPhysicalDevice(), &memory_properties);

        uint32_t memory_type_index = 0;
        for (auto i = 0 ; std::cmp_less(i, memory_properties.memoryTypeCount); i++)
        {
            if ((memory_requirements.memoryTypeBits & (1 << i)) &&
                (memory_properties.memoryTypes[i].propertyFlags & property_bits) == property_bits)
            {
                memory_type_index = i;
            }
        }

        const VkMemoryAllocateInfo allocate_info{
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            nullptr,
            memory_requirements.size,
            memory_type_index};

        if (const auto result = vkAllocateMemory(application->getLogicDevice(), &allocate_info, nullptr, &buffer_memory);
            result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Create buffer memory.");
            // return false;
        }
        return true;
    }

    VkBuffer BufferManager::getBuffer(const std::string &name) const
    {
        if (buffer_maps.contains(name))
        {
            return buffer_maps.at(name);
        }

        return nullptr;
    }
} // VKL
