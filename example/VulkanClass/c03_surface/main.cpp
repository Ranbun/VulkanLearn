#include <iostream>
#include <vector>

#include "VulkanApplication.h"


void mouseButtonCallBack(GLFWwindow * window, int button, int action, int mods)
{
    std::cout << "Mouse button: " << button << " " << action << std::endl;
}

void keyPressCallBack(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    std::cout << "Key button: " << key << " " << action << std::endl;
    if(key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, 1);
    }
}

void resizeCallBack(GLFWwindow * window, int w, int h)
{
    std::cout << "new resize: " << "w = " << w << "h = " << h << std::endl;
}


int main(int argc, char **args)
{
    // std::filesystem::path filePath(__FILE__);

    VulkanApplication app("c03_surface");

    if (!app.getInitialized())
    {
        std::cout << "Failed top initialized VulkanApplication!" << std::endl;
        return 1;
    }

#if 0

    /// vulkan 的调试层
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    for (auto &layer: layers)
    {
        std::cout << "Layers Name: " << layer.layerName << std::endl;
        // std::cout << "Layers Des: " << layer.description << std::endl;
    }
#endif
    /// create swap chain

    /// create surface
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(640, 480, "c03_surface", nullptr, nullptr);

    VkSurfaceKHR surface = nullptr;
    VkResult result = glfwCreateWindowSurface(app.getInstance(), window, nullptr, &surface);
    if(result != VK_SUCCESS)
    {
        std::cout << "Failed to initialize vulkan surface." << std::endl;
        return 1;
    }

    std::cout << surface << std::endl;

    glfwSetMouseButtonCallback(window, mouseButtonCallBack);
    glfwSetKeyCallback(window, keyPressCallBack);
    glfwSetWindowSizeCallback(window, resizeCallBack);

    while (!glfwWindowShouldClose(window));
    {
        glfwPollEvents();
    }

    vkDestroySurfaceKHR(app.getInstance(),surface, nullptr);
    glfwDestroyWindow(window);

    return 0;
}
