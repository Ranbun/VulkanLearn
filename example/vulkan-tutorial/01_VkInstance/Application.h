#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <vulkan/vulkan.h>

class Application
{
public:
    Application();
    ~Application();

    /**
     * @brief Run the application main loop
     */
    void run();

private:
    /**
     * @brief Initialize the window
     *
     */
    void initWindow();
    /**
     * @brief Initialize Vulkan components
     */
    void initVulkan();

    /**
     * @brief Main application loop
     */
    void mainLoop();

    /**
     * @brief Cleanup Vulkan resources
     */
    void cleanup();

    /**
     * @brief Check system requirements
     *
     */
    void checkRequirementsExtensionSupport(std::vector<const char *> &requiredExtensions);

    void createInstance();

private:
    GLFWwindow *window;
    VkInstance instance;
};

#endif
