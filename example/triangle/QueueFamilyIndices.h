#ifndef QUEUEFAMILYINDICES_H_
#define QUEUEFAMILYINDICES_H_

#include <algorithm>
#include <iostream>
#include <optional>
#include <vulkan/vulkan.h>

/**
* @brief 满足需求队列族的索引
*/

struct QueueFamilyIndices
{
    std::optional<uint32_t> m_graphicsFamily = -1;
    std::optional<uint32_t> m_presentFamily = -1;


    [[nodiscard]] auto isComplete() const -> bool
    {
        return m_graphicsFamily.has_value() && m_presentFamily.has_value();
    }
};



#endif 
