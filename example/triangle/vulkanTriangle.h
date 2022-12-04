#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#define GLFW_INCLUDE_VULKAN

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

/// use validation layers ?
#ifdef NODEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

class GLFWwindow;

class HelloTriangleApplication
{
public:
    void run();

private:

    void initWindow();

    void initVulKan();

    void mainLoop() const;

    void cleanup();

    void createInstance();

    /// @brief 检测所有的检验层都能从列表中找到
    static bool checkValidationLayerSupport();

    /// message callback
    std::vector<const char *> getRequireExtensions();


private:

    GLFWwindow* m_window{nullptr};

    /// VulKan
    VkInstance m_vkInstance = nullptr;

};

