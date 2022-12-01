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

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); ///< 阻止GLFW创建上下文(OpenGL Context)
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); ///< 禁止resize

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "VulKan Window", nullptr, nullptr);
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

    /// 获取glfw的扩展
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::cout << "GLFW available extension: " << std::endl;
    for(auto i = 0;i < glfwExtensionCount;i++)
    {
        std::cout << "\t" << *(glfwExtensions+i) << std::endl;
    }


    /// 获取VulKan的扩展
    uint32_t extensionCounts = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCounts
        , nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCounts);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCounts, extensions.data());
    std::cout << "VulKan available extension: " << std::endl;
    for (const auto& it : extensions)
    {
        std::cout << "\t" << it.extensionName << std::endl;
    }

    createInfo.enabledExtensionCount = glfwExtensionCount; ///< 扩展数量 
    createInfo.ppEnabledExtensionNames = glfwExtensions;  ///< 扩展名称
    createInfo.enabledLayerCount = 0; ///< 全局校验层 

    const VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }



}
