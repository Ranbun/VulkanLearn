#include "vulkanTriangle.h"

#define GLFW_INCLUDE_VULKAN
#include <cassert>
#include <GLFW/glfw3.h>
#include <unordered_set>
#include <vector>

#include "GLFW2VulkanToolFunctionsSet.h"
#include "VkCallback.h"

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

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); ///< make GLFW don't create it(OpenGL Context)
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); ///< no resizing

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "VulKan Window", nullptr, nullptr);
}

void HelloTriangleApplication::initVulKan()
{
    createInstance();
    setupDebugCallback();

    pickPhysicalDevice();
}

void HelloTriangleApplication::setupDebugCallback()
{
    if constexpr (!enableValidationLayers)
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;

    if (CreateDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_callBack) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set debug callback");
    }
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
    /// delete Vk Instance
    /// clean others objects before VK Instance
    // TODO: destroy others vk Objects

    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(m_vkInstance, m_callBack, nullptr);
    }

    vkDestroyInstance(m_vkInstance, nullptr);

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

#if 0
    // get glfw extensions
    uint32_t glfwExtensionCount = 0;
    const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    auto glfwExtensionV = std::unordered_set<std::string>{*glfwExtensions, *(glfwExtensions + glfwExtensionCount - 1)};

    /// get VulKan Extensions
    uint32_t extensionCounts = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCounts, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCounts);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCounts, extensions.data());
    auto supportExtensions = GLFW2Vulkan::matchGLFWExtensionsInVulKanExtensions(glfwExtensionV, extensions);

    std::cout << "GLFW Extensions Support By VulKan: " << std::endl;
    for (auto &extension : supportExtensions)
    {
        std::cout << "\t" << extension << std::endl;
    }

    createInfo.enabledExtensionCount = glfwExtensionCount; ///< extension counts
    createInfo.ppEnabledExtensionNames = glfwExtensions;   ///< extension names

#endif

    const auto extensions = getRequireExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size()); ///< extension counts
    createInfo.ppEnabledExtensionNames = extensions.data(); ///< extension names


    /// 启用检验层
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        ///< gloabl avalidation layers Counts
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0; ///< gloabl avalidation layers Counts
    }


    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available");
    }

    const VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }
}

bool HelloTriangleApplication::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> avaliableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());

    for (const auto layerName : validationLayers)
    {
        bool layerFound = false;
        for (const auto& layerProperties : avaliableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> HelloTriangleApplication::getRequireExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        /// 添加扩展 获得检验层的调试信息
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return std::move(extensions);
}


VkResult HelloTriangleApplication::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator,
                                                                VkDebugUtilsMessengerEXT* pCallback)
{
    assert(this);

    const auto p_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT"));
    if (p_func != nullptr)
    {
        return p_func(instance, pCreateInfo, pAllocator, pCallback);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void HelloTriangleApplication::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback,
                                                             const VkAllocationCallbacks* pAllocator)
{
    assert(this);

    const auto p_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (p_func != nullptr)
    {
        p_func(instance, callback, pAllocator);
    }
}

void HelloTriangleApplication::pickPhysicalDevice()
{
    m_physicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);

    if(deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with VulKan support!");
    }

}
