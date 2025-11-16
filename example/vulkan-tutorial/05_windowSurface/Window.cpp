#include "Window.h"
#include <stdexcept>

Window::Window(int width, int height, std::string &title) : m_window(nullptr) { initWindow(width, height, title); };

Window::~Window() { cleanup(); };

void *Window::getNativeWindow() { return reinterpret_cast<void *>(m_window); };

bool Window::shouldClose() { return glfwWindowShouldClose(m_window); };

void Window::pollEvent() { glfwPollEvents(); }

void Window::initWindow(int width, int height, std::string &title)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window!");
    }
};

void Window::cleanup()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
    }

    glfwTerminate();
};
std::vector<const char *> Window::getRequirementVulkanExtensions()
{
    /// 查询 glfw 需要依赖的实例扩展
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (!glfwExtensionCount)
    {
        return {};
    }
    std::vector<const char *> glfwRequirementExtensions{glfwExtensions, glfwExtensions + glfwExtensionCount};
    return std::move(glfwRequirementExtensions);
}
