#ifndef WINDOW_H
#define WINDOW_H


#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:
    Window(int width, int height, std::string &title);
    ~Window();

    Window(Window &) = delete;
    Window &operator=(const Window &) = delete;

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
