#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include "Window.h"

#include "VulkanContext.h"
#include "VulkanLayer.h"

class Application
{
public:
    Application();
    ~Application();

    /**
     * @brief Run the application main loop
     */
    void run() const;

    void Init();

    AppWindow* RenderWindow() const;

    void OnEvent(Event& event) const;

private:
    void init();
    /**
     * @brief Main application loop
     */
    void mainLoop() const;

    /**
     * @brief Cleanup Vulkan resources
     */
    void cleanup();

    void drawFrame() const;

private:
    VulkanFeatureManager featureManager;
    std::unique_ptr<AppWindow> m_window;
    // std::unique_ptr<VulkanContext> m_vkContext;

    std::unique_ptr<VulkanLayer> m_vulkanLayer;

};


#endif
