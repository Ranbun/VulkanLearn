#include "Buffer.h"
#include "VulkanApplication.h"
#include <cstring>

namespace VKL
{
    VkBuffer BufferTool::createVertexBuffer(const VulkanApplication &app, const std::vector<Vertex> &vertices)
    {
        /// create buffer in cpu
        VkBufferCreateInfo bufferInfo{
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            nullptr, 0,
            sizeof(Vertex) * vertices.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,  /// 作为什么存在
            VK_SHARING_MODE_EXCLUSIVE,  /// 独占模式
            0, nullptr
        };


        /// 创建buffer的句柄
        VkBuffer vertex_buffer = nullptr;
        VkResult result = vkCreateBuffer(app.getLogicDevice(), &bufferInfo, nullptr, &vertex_buffer);
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error("Create Vertex buffer Failed!");
        }

        /// map data to buffer
        /// request memory for buffer
        /// 需要多少显存 如何对齐 什么类型
        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(app.getLogicDevice(), vertex_buffer, &memory_requirements);

        /// 获取物理设备支持的内存属性
        VkPhysicalDeviceMemoryProperties memory_properties;  /// get pros
        vkGetPhysicalDeviceMemoryProperties(app.getPhysicalDevice(), &memory_properties);

        /// 看看当前如理设备的内存属性 能不能匹配我请求的buffer 的内存属性
        /// 获取合适的内存类型
        uint32_t memIndex = 0;
        uint32_t propsBits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
        {
            if (memory_requirements.memoryTypeBits & (1 << i))
            {
                if ((memory_properties.memoryTypes[i].propertyFlags & propsBits) == propsBits)
                {
                    memIndex = i;
                }
            }
        }
        if (memIndex == 0)
        {
            throw std::runtime_error("Failed to find a usable memory type!");
        }

        /// 分配显卡端的内存
        VkMemoryAllocateInfo allocate_info{
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            nullptr,
            memory_requirements.size, /// 分配的内存大小
            memIndex    /// 请求类型的Index
        };

        VkDeviceMemory vertex_buffer_memory = nullptr;
        result = vkAllocateMemory(app.getLogicDevice(), &allocate_info, nullptr, &vertex_buffer_memory);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate vertex buffer memory!");
        }

        /// 绑定buffer 到分配好的内存上
        vkBindBufferMemory(app.getLogicDevice(), vertex_buffer, vertex_buffer_memory, 0);

        /// 映射数据到
        /// 隐射显卡内存到CPU
        void *data = nullptr;
        vkMapMemory(app.getLogicDevice(), vertex_buffer_memory, 0, bufferInfo.size, 0, &data);
        /// 复制顶点数据
        memcpy(data, vertices.data(), bufferInfo.size);
        /// 结束映射
        vkUnmapMemory(app.getLogicDevice(), vertex_buffer_memory);
        /// buffer 创建完成
        return vertex_buffer;
    }

    bool BufferTool::createVertexBuffer(const VulkanApplication &app, VkBuffer & buffer, VkDeviceMemory & bufferMemory, const size_t dataSize, const VkBufferUsageFlags usage, const uint32_t propertyBits)
    {
        /// create buffer in cpu
        const VkBufferCreateInfo buffer_info{
                VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                nullptr, 0,
                dataSize,
                usage,                            /// 作为什么存在 --什么类型的缓冲
                VK_SHARING_MODE_EXCLUSIVE,        /// 独占模式
                0, nullptr};


        /// 创建buffer的句柄
        // VkBuffer vertex_buffer = nullptr;
        VkResult result = vkCreateBuffer(app.getLogicDevice(), &buffer_info, nullptr, &buffer);
        if (result != VK_SUCCESS)
        {
            return false;
            throw std::runtime_error("Create Vertex buffer Failed!");
        }

        /// map data to buffer
        /// request memory for buffer
        /// 需要多少显存 如何对齐 什么类型
        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(app.getLogicDevice(), buffer, &memory_requirements);

        /// 获取物理设备支持的内存属性
        VkPhysicalDeviceMemoryProperties memory_properties;/// get pros
        vkGetPhysicalDeviceMemoryProperties(app.getPhysicalDevice(), &memory_properties);

        /// 看看当前如理设备的内存属性 能不能匹配我请求的buffer 的内存属性
        /// 获取合适的内存类型
        uint32_t mem_index = 0;
        // constexpr uint32_t props_bits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
        {
            if (memory_requirements.memoryTypeBits & (1 << i))
            {
                if ((memory_properties.memoryTypes[i].propertyFlags & propertyBits) == propertyBits)
                {
                    mem_index = i;
                }
            }
        }
        if (mem_index == 0)
        {
            return false;
            throw std::runtime_error("Failed to find a usable memory type!");
        }

        /// 分配显卡端的内存 -- 分配设备端的内存
        const VkMemoryAllocateInfo allocate_info{
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                nullptr,
                memory_requirements.size,/// 分配的内存大小
                mem_index                 /// 请求类型的Index
        };

        // VkDeviceMemory vertex_buffer_memory = nullptr;
        result = vkAllocateMemory(app.getLogicDevice(), &allocate_info, nullptr, &bufferMemory);

        if (result != VK_SUCCESS)
        {
            return false;
            throw std::runtime_error("Failed to allocate vertex buffer memory!");
        }

        /// 绑定buffer 到分配好的内存上
        vkBindBufferMemory(app.getLogicDevice(), buffer, bufferMemory, 0);

        return true;
    }

    VkBuffer BufferTool::createVertexBufferNew(const VulkanApplication &app, const std::vector<Vertex> &vertices)
    {
        const size_t data_size = sizeof(Vertex) * vertices.size();
        VkBuffer vertex_buffer = nullptr;
        VkDeviceMemory vertex_buffer_memory = nullptr;
        if (!createVertexBuffer(app, vertex_buffer, vertex_buffer_memory,data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            return nullptr;
        }

        /// 映射数据到
        /// 隐射显卡内存到CPU
        void *data = nullptr;
        vkMapMemory(app.getLogicDevice(), vertex_buffer_memory, 0, data_size, 0, &data);
        /// 复制顶点数据
        memcpy(data, vertices.data(), data_size);
        /// 结束映射
        vkUnmapMemory(app.getLogicDevice(), vertex_buffer_memory);
        /// buffer 创建完成
        return vertex_buffer;
    }

    VkBuffer BufferTool::createIndexBuffer(const VulkanApplication &app, const std::vector<uint32_t> &indices)
    {
        const size_t data_size = sizeof(uint32_t) * indices.size();
        VkBuffer index_buffer = nullptr;
        VkDeviceMemory index_buffer_memory = nullptr;
        if (!createVertexBuffer(app, index_buffer, index_buffer_memory,data_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            return nullptr;
        }

        /// 映射数据到
        /// 隐射显卡内存到CPU
        void *data = nullptr;
        vkMapMemory(app.getLogicDevice(), index_buffer_memory, 0, data_size, 0, &data);
        /// 复制顶点数据
        memcpy(data, indices.data(), data_size);
        /// 结束映射
        vkUnmapMemory(app.getLogicDevice(), index_buffer_memory);
        /// buffer 创建完成
        return index_buffer;
    }

}
