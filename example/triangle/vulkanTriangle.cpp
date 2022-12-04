#include "vulkanTriangle.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <unordered_set>
#include <vector>

#include "GLFW2VulkanToolFunctionsSet.h"

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   ///< no resizing

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
    /// delete Vk Instance
    /// clean others objects before VK Instance
    // TODO: destroy others vk Objects

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

    auto extensions = getRequireExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size()); ///< extension counts
    createInfo.ppEnabledExtensionNames = extensions.data();   ///< extension names


    /// ÆôÓÃ¼ìÑé²ã
    if(enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); ///< gloabl avalidation layers Counts
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

    for(auto layerName: validationLayers)
    {
        bool layerFound = false;
        for(const auto& layerProperties: avaliableLayers )
        {
            if(strcmp(layerName,layerProperties.layerName) == 0)
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

std::vector<const char*> HelloTriangleApplication::getRequireExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if(enableValidationLayers)
    {
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return std::move(extensions);
}
