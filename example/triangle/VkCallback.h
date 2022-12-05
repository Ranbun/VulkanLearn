#ifndef VKCALLBACK_H_
#define VKCALLBACK_H_

#include <vulkan/vulkan.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);




#endif // !VKCALLBACK_H_