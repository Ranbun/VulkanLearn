#ifndef APPLICATION_H
#define APPLICATION_H


struct GLFWwindow;

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
    void RequirementsCheck();

private:
    GLFWwindow *window;
};

#endif
