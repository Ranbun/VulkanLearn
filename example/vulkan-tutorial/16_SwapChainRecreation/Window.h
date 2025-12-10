#ifndef WINDOW_H
#define WINDOW_H


#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Event.h"
#include <string>
#include <functional>

class AppWindow
{
public:
    using EventCallbackFn = std::function<void(Event&)>;
    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        EventCallbackFn EventCallback;
    };

    void setEventCallBack(EventCallbackFn callback) {
        m_Data.EventCallback = std::move(callback);
    }

    AppWindow(int width, int height, const std::string &title);
    ~AppWindow();

    AppWindow(AppWindow &) = delete;
    AppWindow &operator=(const AppWindow &) = delete;

    bool shouldClose() const;
    void *getNativeWindow() const;

    void pollEvent();

    std::vector<const char *> getRequirementVulkanExtensions();

private:
    void initWindow(int width, int height, const std::string &title);
    void cleanup();

    WindowData m_Data;
    GLFWwindow *m_window;
};

#endif
