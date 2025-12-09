#include "Window.h"
#include <stdexcept>

#include "VulkanContext.h"

AppWindow::AppWindow(int width, int height, std::string &title) : m_window(nullptr)
{
    initWindow(width, height, title);
};

AppWindow::~AppWindow() { cleanup(); };

void *AppWindow::getNativeWindow() const { return reinterpret_cast<void *>(m_window); };

bool AppWindow::shouldClose() const { return glfwWindowShouldClose(m_window); };

void AppWindow::pollEvent() { glfwPollEvents(); }

void AppWindow::setDate(void *data) const {
    glfwSetWindowUserPointer(m_window, data);
}

void AppWindow::initWindow(int width, int height, std::string &title)
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

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        auto * vkcontext = static_cast<VulkanContext*>(glfwGetWindowUserPointer(window));
        vkcontext->resize(width, height);
    });

};

void AppWindow::cleanup()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
    }

    glfwTerminate();
};
std::vector<const char *> AppWindow::getRequirementVulkanExtensions()
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
