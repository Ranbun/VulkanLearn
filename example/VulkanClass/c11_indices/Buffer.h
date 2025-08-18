#ifndef BUFFER_H_
#define BUFFER_H_

#include <vulkan/vulkan.h>
#include "Vertex.h"
#include <vector>

class VulkanApplication;

namespace VKL
{
    class BufferTool
    {
    public:
        static VkBuffer createVertexBuffer(VulkanApplication &app, const std::vector<Vertex> &vertices);
    };
};

#endif //! BUFFER_H_
