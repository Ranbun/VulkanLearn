#ifndef BUFFER_H_
#define BUFFER_H_

#include <vulkan/vulkan.h>
#include "Vertex.h"
#include <vector>

class VulkanApplication;

namespace VKL
{
    struct Vertex;

    class BufferTool
    {
    public:
        static VkBuffer createVertexBuffer(const VulkanApplication &app, const std::vector<Vertex> &vertices);
        static bool createVertexBuffer(const VulkanApplication &app, VkBuffer& buffer,
                                    VkDeviceMemory &bufferMemory, size_t dataSize, VkBufferUsageFlags usage, uint32_t propertyBits);
        static VkBuffer createVertexBufferNew(const VulkanApplication &app, const std::vector<Vertex> &vertices);
        static VkBuffer createIndexBuffer(const VulkanApplication &app, const std::vector<uint32_t> &indices);


    };
};

#endif //! BUFFER_H_
