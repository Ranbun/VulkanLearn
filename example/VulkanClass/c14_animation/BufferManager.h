#ifndef VULKANLEARN_BUFFERMANAGER_H
#define VULKANLEARN_BUFFERMANAGER_H

#include <vulkan/vulkan.h>

#include "Vertex.h"
#include "VulkanApplication.h"

namespace VKL
{
    class BufferManager
    {
    public:
        explicit BufferManager(VulkanApplication *app);
        ~BufferManager();

        bool createDescriptorSet();

        bool prepare(const std::string &vert_file, const std::string &frag_file, VkDescriptorSetLayout desc_set) const;
        void cleanup();

        VkBuffer CreateVertexBuffer(const std::string &name, const std::vector<Vertex> &vertices);
        VkBuffer CreateIndexBuffer(const std::string &name, const std::vector<uint32_t> &indices);
        VkBuffer CreateUniformBuffer(const std::string &name, size_t ubo_size);
        bool CreateBuffer(VkBuffer &buffer, size_t data_size, VkBufferUsageFlags usage) const;
        bool CreateBufferMemory(VkDeviceMemory &buffer_memory, const VkMemoryRequirements &memory_requirements,
                                uint32_t property_bits) const;

        [[nodiscard]] VkBuffer getBuffer(const std::string &name) const;

        [[nodiscard]] void *mapBufferMemory(const std::string &name, size_t data_size) const;
        void unmapBufferMemory(const std::string &name) const;

        [[nodiscard]] VkDescriptorSet getDescriptorSet() const { return _descriptor_set; }
        [[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayout() const { return _descriptor_set_layout; }

        [[nodiscard]] VkImage createTextureImage(const std::string &name, const std::string &file);

    private:
        std::map<std::string, VkBuffer> _buffer_maps{};
        std::map<std::string, VkDeviceMemory> _buffer_memory_map{};
        std::map<std::string, VkImage> _image_map;
        VulkanApplication *_application{nullptr};

        VkDescriptorSet _descriptor_set{nullptr};
        VkDescriptorSetLayout _descriptor_set_layout{nullptr};
        VkDescriptorPool _descriptor_pool{nullptr};
    };

} // namespace VKL

#endif // VULKANLEARN_BUFFERMANAGER_H
