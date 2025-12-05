#ifndef WINDOW_H
#define WINDOW_H


#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class AppWindow
{
public:
    AppWindow(int width, int height, std::string &title);
    ~AppWindow();

    AppWindow(AppWindow &) = delete;
    AppWindow &operator=(const AppWindow &) = delete;

    bool shouldClose();
    void *getNativeWindow();

    void pollEvent();

    std::vector<const char *> getRequirementVulkanExtensions();

private:
    void initWindow(int width, int height, std::string &title);
    void cleanup();
    GLFWwindow *m_window;
};

#endif
