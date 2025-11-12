#include "Application.h"

#include "window_pro.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <iostream>
#include <string.h>
#include <vector>

Application::Application() {}

Application::~Application() {}
void Application::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void Application::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WP.width, WP.height, WP.title.c_str(), nullptr, nullptr);
}

void Application::initVulkan() { createInstance(); }
void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    };
}
void Application::cleanup()
{
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::checkRequirementsExtensionSupport(std::vector<const char *> &requiredExtensions)
{
    /// Vulkan Instance Extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "Instance Extensions:" << std::endl;
    for (auto i = 0; i < extensionCount; i++)
    {
        std::cout << extensions[i].extensionName << std::endl;
    }

    std::cout << "\n";
    std::cout << "Required Extensions supported:" << std::endl;
    for (auto i = 0; i < requiredExtensions.size(); i++)
    {

        auto it = std::find_if(extensions.begin(), extensions.end(), [&](VkExtensionProperties &per)
                               { return strcmp(per.extensionName, requiredExtensions[i]) == 0; });
        std::cout << requiredExtensions[i] << "\t: " << ((it == extensions.end()) ? "false" : "true") << std::endl;
    }
}
void Application::createInstance()
{
    VkApplicationInfo app_info{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Hello Vulkan",
            .pEngineName = "No Engine",
            .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &app_info,

            /// 启用哪些全局验证层
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,

            /// 启用哪些实例扩展
            .enabledExtensionCount = 0,
            .ppEnabledExtensionNames = nullptr,
    };

    /// 查询 glfw 需要依赖的实例扩展
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> glfwRequirementExtensions{glfwExtensions, glfwExtensions + glfwExtensionCount};

    /// 检测glfw 请求的扩展是否支持
    checkRequirementsExtensionSupport(glfwRequirementExtensions);

    createInfo.enabledExtensionCount = glfwRequirementExtensions.size();
    createInfo.ppEnabledExtensionNames = glfwRequirementExtensions.data();

    auto res = vkCreateInstance(&createInfo, nullptr, &instance);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}
