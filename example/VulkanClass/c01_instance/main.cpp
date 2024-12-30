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

    /// shutdown
    glfwTerminate();

    return 0;
}