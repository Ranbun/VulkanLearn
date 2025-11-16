#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include "Window.h"

#include "VulkanContext.h"


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
    void init();
    /**
     * @brief Main application loop
     */
    void mainLoop();

    /**
     * @brief Cleanup Vulkan resources
     */
    void cleanup();

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<VulkanContext> m_vkContext;
};

#endif
