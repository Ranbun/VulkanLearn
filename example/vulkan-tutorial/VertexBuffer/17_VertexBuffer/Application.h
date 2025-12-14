#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include "Window.h"

#include "VulkanLayer.h"

class Application
{
public:
    Application(int width, int height, const std::string &title);
    ~Application();

    /**
     * @brief Run the application main loop
     */
    void run() const;

    void Init(int width, int height, const std::string &title);

    AppWindow* renderWindow() const;

    void OnEvent(Event& event);

private:
    void init(int width, int height, const std::string &title);
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
    VulkanFeatureManager m_featureManager;
    std::unique_ptr<AppWindow> m_window;
    std::unique_ptr<VulkanLayer> m_vulkanLayer;

    bool m_running {true};

};






#endif
