#include <iostream>
#include <filesystem>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main()
{
    std::filesystem::path filePath(__FILE__);
    /// std::cout<<filePath.filename()<<std::endl;

    if(!glfwInit())
    {
        std::cout << "Failed to initialize GLFW."<<std::endl;
        return 1;
    }

    if(!glfwVulkanSupported())
    {
        std::cout << "Vulkan is not supported."<<std::endl;
        return 1;

    }
    std::cout << "Vulkan is initialize success!"<<std::endl;


    /// check support extensions
    uint32_t glfwExtCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    std::cout << "GLFW initialized. It requires the following etensions." << std::endl;

    for (auto i = 0; i < glfwExtCount; i++)
    {
        std::cout << glfwExtensions[i] << std::endl;
    }

    /// TODO:

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

    VkInstance instance = nullptr;
    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, nullptr, &instance);
    if(result != VK_SUCCESS)
    {
        std::cout << "Vukan instance create failed!" << std::endl;
    }

    std::cout << "Vukan instance create success!" << std::endl;

    if(instance)
    {
        vkDestroyInstance(instance, nullptr);
    }

    /// create instance

    /// select phycis device


    /// shutdown
    glfwTerminate();

    return 0;
}
