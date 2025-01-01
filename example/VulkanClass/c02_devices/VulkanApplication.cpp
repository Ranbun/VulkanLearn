#include "VulkanApplication.h"

#include <iostream>

VulkanApplication::VulkanApplication(const char *appName)
: _initiallized(false)
{
    if(!glfwInit())
    {
        std::cout << "Failed to initialize GLFW."<<std::endl;
        return;
    }

    if(!glfwVulkanSupported())
    {
        std::cout << "Vulkan is not supported."<<std::endl;
        return;
    }
    _initiallized = createInstance(appName);
}

VulkanApplication::~VulkanApplication()
{
    if(_initiallized && _instance)
    {
        vkDestroyInstance(_instance, nullptr);
    }

    glfwTerminate();
}

bool VulkanApplication::createInstance(const char *appName)
{
    /// create application
    VkApplicationInfo appinfo
    {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "c01_instance",
        VK_MAKE_VERSION(1, 0, 0),
        "VukanCalss",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_2
    };

    /// check support extensions
    uint32_t glfwExtCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    // std::cout << "GLFW initialized. It requires the following etensions." << std::endl;

    VkInstanceCreateInfo vkInstanceCreateInfo
    {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &appinfo,
        0,
        nullptr,
        glfwExtCount,
        glfwExtensions
    };

    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, nullptr, &_instance);
    if(result != VK_SUCCESS)
    {
        std::cout << "Vukan instance create failed!" << std::endl;
        return false;
    }
    std::cout << "Vukan instance create success!" << std::endl;


    return true;
}
