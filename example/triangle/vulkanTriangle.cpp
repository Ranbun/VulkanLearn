#include "vulkanTriangle.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


void HelloTriangleApplication::run()
{
    initWindow();
    initVulKan();
    mainLoop();
    cleanup();
}

void HelloTriangleApplication::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); ///< ��ֹGLFW����������(OpenGL Context)
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); ///< ��ֹresize

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Window", nullptr, nullptr);
}

void HelloTriangleApplication::initVulKan()
{
    createInstance();
}

void HelloTriangleApplication::mainLoop() const
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}

void HelloTriangleApplication::cleanup()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
}

void HelloTriangleApplication::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;


    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;

    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount; ///< ��չ���� 
    createInfo.ppEnabledExtensionNames = glfwExtensions;  ///< ��չ����
    createInfo.enabledLayerCount = 0; ///< ȫ��У��� 

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);


}
