#ifndef VULKANLEARN_BUFFERMANAGER_H
#define VULKANLEARN_BUFFERMANAGER_H

#include "VulkanApplication.h"

#include <vulkan/vulkan.h>

namespace VKL
{
    struct Vertex
    {
        float x, y, z;/// position
        float r, g, b;/// color
    };

    class BufferManager
    {
    public:
        BufferManager(VulkanApplication *app);
        ~BufferManager();

        bool prepare(const std::string &vert_file, const std::string &frag_file, VkDescriptorSetLayout desc_set) const;
        void cleanup();

        VkBuffer CreateVertexBuffer(const std::string &name, const std::vector<Vertex> &vertices);
        VkBuffer CreateIndexBuffer(const std::string &name, const std::vector<uint32_t> &indices);

        bool CreateBuffer(VkBuffer &buffer, size_t data_size, VkBufferUsageFlags usage) const;
        bool CreateBufferMemory(VkDeviceMemory &buffer_memory, const VkMemoryRequirements &memory_requirements, uint32_t property_bits) const;

        [[nodiscard]] VkBuffer getBuffer(const std::string &name) const;

    private:
        std::map<std::string, VkBuffer> buffer_maps;
        std::map<std::string, VkDeviceMemory> buffer_memory_map;
        VulkanApplication * application;

    };
} // VKL

#endif //VULKANLEARN_BUFFERMANAGER_H
