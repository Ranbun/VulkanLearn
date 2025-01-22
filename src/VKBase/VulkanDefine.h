#ifndef VULKANDDEFINE_H_
#define VULKANDDEFINE_H_

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR   ///< 在包含vulkan.h前定义该宏，会一并包含vulkan_win32.h和windows.h
    #define NOMINMAX                    ///< 定义该宏可避免windows.h中的min和max两个宏与标准库中的函数名冲突
#endif

#include  <vulkan/vulkan.h>

#endif //! VULKANDDEFINE_H_
