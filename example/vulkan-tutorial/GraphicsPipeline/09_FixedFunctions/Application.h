#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include "Window.h"

#include "VulkanContext.h"

class Application
{
public:
    static Application &Instance();

    /**
     * @brief Run the application main loop
     */
    void run() const;

    void Init();

    AppWindow *RenderWindow() const;

private:
    Application();
    ~Application();

    void init();
    /**
     * @brief Main application loop
     */
    void mainLoop() const;

    /**
     * @brief Cleanup Vulkan resources
     */
    void cleanup();

private:
    VulkanFeatureManager featureManager;
    std::unique_ptr<AppWindow> m_window;
    std::unique_ptr<VulkanContext> m_vkContext;
};

#endif
