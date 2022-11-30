#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include "vulkanTriangle.h"

#if 0
int main()
{
    glfwInit();

    const auto window = glfwCreateWindow(800, 600, "Vulkan Wndow", nullptr, nullptr);
    uint32_t extensionCount = 0;

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << "extension supported" << std::endl;

    glm::mat4 matrix;
    glm::vec4 vec;

    auto test = matrix * vec;

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#endif

int main()
{
    try
    {
        HelloTriangleApplication app;
        app.run();
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
