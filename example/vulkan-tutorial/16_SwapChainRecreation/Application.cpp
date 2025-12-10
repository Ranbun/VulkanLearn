#include "Application.h"

#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

Application::Application()
{ Init(); }

Application::~Application() { cleanup(); }

void Application::init()
{
    m_window = std::make_unique<AppWindow>(800, 640, std::string("SwapChainRecreation"));
    const auto & requirementExtensions = m_window->getRequirementVulkanExtensions();
    featureManager.enableValidationLayers(true);
    featureManager.requestFeature(EngineFeature::SwapChain);
    featureManager.requestFeature(EngineFeature::DebugUtils);
    for (auto & extension: requirementExtensions)
    {
        featureManager.requestInstanceExtension(extension);
    }

    m_window->setEventCallBack([this](auto & event) {
        OnEvent(event);
    });

    m_vulkanLayer = std::make_unique<VulkanLayer>("Vulkan Layer", featureManager,[this]()
    {
        return m_window->getNativeWindow();
    });

    std::cout << "Application initialized successfully." << std::endl;
}

void Application::run() const { mainLoop(); }

void Application::mainLoop() const
{
    while (!m_window->shouldClose())
    {
        m_window->pollEvent();
        m_vulkanLayer->OnUpdate(0.0);
    };
}

void Application::cleanup()
{
    m_vulkanLayer.reset();
    m_window.reset();

    std::cout << "Application cleanup complete." << std::endl;
}

AppWindow *Application::RenderWindow() const { return m_window.get(); }

void Application::OnEvent(Event &event) const {
    m_vulkanLayer->OnEvent(event);
}

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
