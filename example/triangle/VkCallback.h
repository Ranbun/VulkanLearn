#ifndef VKCALLBACK_H_
#define VKCALLBACK_H_

#include <iostream>
#include <vulkan/vulkan.h>

/**
 * @brief 接收调试信息的回调函数 - 被VulKan调用
 * @param messageServerity 消息的级别
 * @param messageType 消息的类型
 * @param pCallbackData 一个VkDebugUtilsMessengerCallbackDataEXT的指针(包含调试信息......)
 * @param pUserData 指向设置回调函数时候设置的数据的指针
 * @return 表示引发检验层处理的VK_API是否被中断 通常只有在测试检验层本身的时候才会返回true
 */

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}


#endif // !VKCALLBACK_H_