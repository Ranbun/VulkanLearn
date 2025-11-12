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


const std::vector<const char *> validationLayers{"VK_LAYER_KHRONOS_validation"};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    void *pUserData)
{

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}


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

void Application::initVulkan()
{
    createInstance();
    setupDebugMessenger();
}
void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    };
}
void Application::cleanup()
{
    if (enableValidationLayers())
    {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::createInstance()
{
    if (enableValidationLayers() && !checkValidationLayerSupport())
    {
        throw std::runtime_error("validation layer requested, but not supported");
    }

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

    const std::vector<const char *> &requirementExtensions = getRequirementExtensions();

    createInfo.enabledExtensionCount = requirementExtensions.size();
    createInfo.ppEnabledExtensionNames = requirementExtensions.data();

    if (enableValidationLayers())
    {
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    auto res = vkCreateInstance(&createInfo, nullptr, &instance);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

bool Application::checkRequirementsExtensionSupport(std::vector<const char *> &requiredExtensions)
{
    /// Vulkan Instance Extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "\n";
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
        if (it == extensions.end())
        {
            return false;
        }
    }
    return true;
}

bool Application::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::cout << "\n";
    std::cout << "available Layers: " << std::endl;
    for (auto &pro: availableLayers)
    {
        std::cout << pro.layerName << "\n";
    }

    auto getEnableValidationLayers = []() { return validationLayers; };

    /// check validations
    std::cout << "\n";
    std::cout << "Required Layers supported:" << std::endl;
    const auto &enableValidationLayers = getEnableValidationLayers();
    for (auto &validation: enableValidationLayers)
    {
        auto it = std::find_if(availableLayers.begin(), availableLayers.end(), [&validation](VkLayerProperties &proper)
                               { return strcmp(proper.layerName, validation) == 0; });

        std::cout << validation << "\t: ";
        if (it == availableLayers.end())
        {
            std::cout << "false" << std::endl;
            return false;
        }

        std::cout << "true" << std::endl;
    }

    return true;
}
std::vector<const char *> Application::getRequirementExtensions()
{

    /// 查询 glfw 需要依赖的实例扩展
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> glfwRequirementExtensions{glfwExtensions, glfwExtensions + glfwExtensionCount};

    if (enableValidationLayers())
    {
        glfwRequirementExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); /// requirement message callback
    }

    /// 检测请求的扩展是否支持
    auto res = checkRequirementsExtensionSupport(glfwRequirementExtensions);
    if (!res)
    {
        throw std::runtime_error("extension requested, but not supported");
    }

    return std::move(glfwRequirementExtensions);
};

void Application::setupDebugMessenger()
{
    if (!enableValidationLayers())
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                                                  .pNext = nullptr,
                                                  .flags = 0,
                                                  .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                                                  .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                                                  .pfnUserCallback = debugCallback,
                                                  .pUserData = nullptr};

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}
