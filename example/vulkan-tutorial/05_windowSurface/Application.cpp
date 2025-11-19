#include "Application.h"
#include <iostream>

#include "window_pro.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

Application::Application() {}
Application::~Application() { cleanup(); }

void Application::init()
{
    m_window = std::make_unique<AppWindow>(WP.width, WP.height, WP.title);
    const auto &windowRequirementExtensions = m_window->getRequirementVulkanExtensions();
    m_vkContext = std::make_unique<VulkanContext>(windowRequirementExtensions);

    std::cout << "Application initialized successfully." << std::endl;
}

void Application::run() { mainLoop(); }

void Application::mainLoop()
{
    while (!m_window->shouldClose())
    {
        m_window->pollEvent();
    };
}

void Application::cleanup()
{
    m_vkContext.reset();
    m_window.reset();

    std::cout << "Application cleanup complete." << std::endl;
}
Application &Application::Instance()
{
    static Application app;
    return app;
}
AppWindow *Application::RenderWindow() { return m_window.get(); }
void Application::Init()
{
    try
    {
        init();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Application initialization failed: " << e.what() << std::endl;
        cleanup();
        throw;
    }
};
