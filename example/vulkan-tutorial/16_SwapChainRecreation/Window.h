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

    bool shouldClose() const;
    void *getNativeWindow() const;

    void pollEvent();

    void setDate(void * data) const;

    std::vector<const char *> getRequirementVulkanExtensions();

private:
    void initWindow(int width, int height, std::string &title);
    void cleanup();
    GLFWwindow *m_window;
};

#endif
