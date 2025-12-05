#include "Application.h"
#include <iostream>

#include "window_pro.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

Application::Application() = default;

Application::~Application() { cleanup(); }

void Application::init()
{
    m_window = std::make_unique<AppWindow>(WP.width, WP.height, WP.title);
    const auto & requirementExtensions = m_window->getRequirementVulkanExtensions();

    featureManager.enableValidationLayers(true);
    featureManager.requestFeature(EngineFeature::SwapChain);
    featureManager.requestFeature(EngineFeature::DebugUtils);
    for (auto & extension: requirementExtensions)
    {
        featureManager.requestInstanceExtension(extension);
    }
    m_vkContext = std::make_unique<VulkanContext>(featureManager);
    std::cout << "Application initialized successfully." << std::endl;
}

void Application::run() const { mainLoop(); }

void Application::mainLoop() const
{
    while (!m_window->shouldClose())
    {
        m_window->pollEvent();
        drawFrame();
    };
}

void Application::cleanup()
{
    m_vkContext.reset();
    m_window.reset();

    std::cout << "Application cleanup complete." << std::endl;
}

void Application::drawFrame() const
{
    m_vkContext->drawFrame();
}

Application &Application::Instance()
{
    static Application app;
    return app;
}

AppWindow *Application::RenderWindow() const { return m_window.get(); }

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
