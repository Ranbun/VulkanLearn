#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#define GLFW_INCLUDE_VULKAN

class GLFWwindow;

class HelloTriangleApplication
{
public:
    void run();

private:

    void initWindow();

    void initVulKan();

    void mainLoop() const;

    void cleanup();

    void createInstance();


private:

    GLFWwindow* m_window{nullptr};
    const int WIDTH = 800;
    const int HEIGHT = 600;

    /// VulKan
    VkInstance m_vkInstance = nullptr;



};

