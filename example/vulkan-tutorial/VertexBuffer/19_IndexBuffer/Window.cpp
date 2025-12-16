#include "Window.h"

#include <stdexcept>

#include "Event.h"
#include "VulkanContext.h"

AppWindow::AppWindow(int width, int height, const std::string& title) : m_window(nullptr)
{
    initWindow(width, height, title);
};

AppWindow::~AppWindow() { cleanup(); };

void* AppWindow::getNativeWindow() const { return reinterpret_cast<void*>(m_window); };

;

void AppWindow::pollEvent() { glfwPollEvents(); }

void AppWindow::initWindow(int width, int height, const std::string& title)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window!");
    }

    glfwSetWindowUserPointer(m_window, &m_Data);

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
    {
        auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        WindowResizeEvent event(width, height);
        data->EventCallback(event);
    });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
    {
        auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        const bool close = glfwWindowShouldClose(window);
        AppCloseEvent event(close);
        data->EventCallback(event);
    });
};

void AppWindow::cleanup() const
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
    }

    glfwTerminate();
};

std::vector<const char*> AppWindow::getRequirementVulkanExtensions()
{

    /// 查询 glfw 需要依赖的实例扩展
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (!glfwExtensionCount)
    {
        return {};
    }
    std::vector<const char*> glfwRequirementExtensions{glfwExtensions, glfwExtensions + glfwExtensionCount};
    return std::move(glfwRequirementExtensions);
}
