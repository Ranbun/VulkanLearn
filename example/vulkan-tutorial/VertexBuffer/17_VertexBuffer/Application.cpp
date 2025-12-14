#include "Application.h"

#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "Event.h"

Application::Application(int width, int height, const std::string& title)
{
    Init(width, height, title);
}

Application::~Application() { cleanup(); }

void Application::init(int width, int height, const std::string& title)
{
    m_window = std::make_unique<AppWindow>(width, height, title);
    const auto& requirementExtensions = m_window->getRequirementVulkanExtensions();
    m_featureManager.enableValidationLayers(true);
    m_featureManager.requestFeature(EngineFeature::SwapChain);
    m_featureManager.requestFeature(EngineFeature::DebugUtils);
    for (auto& extension : requirementExtensions)
    {
        m_featureManager.requestInstanceExtension(extension);
    }

    m_window->setEventCallBack([this](auto& event)
    {
        OnEvent(event);
    });

    m_vulkanLayer = std::make_unique<VulkanLayer>("Vulkan Layer", m_featureManager, [this]()
    {
        return m_window->getNativeWindow();
    });

    std::cout << "Application initialized successfully." << std::endl;
}

void Application::run() const { mainLoop(); }

void Application::mainLoop() const
{
    while (m_running)
    {
        m_window->pollEvent();
        m_vulkanLayer->OnUpdate(0.0);
    };
}

void Application::cleanup()
{
    m_vulkanLayer.reset();
    m_window.reset();
    std::cout << "Application cleanup complete." << '\n';
}

AppWindow* Application::renderWindow() const { return m_window.get(); }

void Application::OnEvent(Event& event)
{
    m_vulkanLayer->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<AppCloseEvent>([this](Event& event)-> bool
    {
        const auto& closeEvent = dynamic_cast<AppCloseEvent&>(event);
        m_running = !closeEvent.GetClose();
        return true;
    });
}

void Application::Init(const int width, const int height, const std::string& title)
{
    try
    {
        init(width, height, title);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Application initialization failed: " << e.what() << '\n';
        cleanup();
        throw;
    }
};
