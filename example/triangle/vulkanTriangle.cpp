#define VK_USE_PLATFORM_WIN32_KHR 

#include "vulkanTriangle.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "GLFW2VulkanToolFunctionsSet.h"
#include "VkCallback.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <set>
#include <GLFW/glfw3native.h>

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include <limits>
#include <cmath>


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
    setupDebugMessenger();
    createSurface(); ///< 创建绘制的表面
    pickPhysicalDevice();
    createLogicDevice();

    /// 创建交换链
    createSwapChain();
}

void HelloTriangleApplication::setupDebugMessenger()
{
    if constexpr (!enableValidationLayers)
    {
        return;
    }

    /// 设置调试信息
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
    /// 删除交换链
    vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);

    /// 逻辑设备
    vkDestroyDevice(m_device, nullptr);

    /// delete Vk Instance
    /// clean others objects before VK Instance
    // TODO: destroy others vk Objects

    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(m_vkInstance, m_callBack, nullptr);
    }

    /// <summary>
    /// 删除SurfaceKHR对象，需要在在实例被清除之前完成此操作
    /// </summary>
    vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);

    vkDestroyInstance(m_vkInstance, nullptr);

    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
}

void HelloTriangleApplication::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

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

    const auto extensions = getRequireExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

bool HelloTriangleApplication::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto layerName : validationLayers)
    {
        auto layerFound = false;
        for (const auto& layerProperties : availableLayers)
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
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        /// 添加扩展 获得检验层的调试信息
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return std::move(extensions); // NOLINT(clang-diagnostic-pessimizing-move)
}


VkResult HelloTriangleApplication::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator,
                                                                VkDebugUtilsMessengerEXT* pCallback) const
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
                                                             const VkAllocationCallbacks* pAllocator) const
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
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with VulKan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device))
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

QueueFamilyIndices HelloTriangleApplication::findQueueFamily(VkPhysicalDevice device) const
{
    assert(this);
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    /// VkQueueFamilyProperties 支持的操作类型 和可以创建队列的个数
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    VkBool32 presentSupport = false;


    auto i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) ///< 是否支持渲染命令
        {
            indices.m_graphicsFamily = i;
        }

        /// 显示命令
        if (queueFamily.queueCount > 0 && presentSupport)
        {
            indices.m_presentFamily = i;
        }


        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

void HelloTriangleApplication::createLogicDevice()
{
    const auto indices = findQueueFamily(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {
        static_cast<int>(indices.m_graphicsFamily.value()), static_cast<int>(indices.m_presentFamily.value())
    };

    constexpr auto queuePriority = 1.0f;
    for (const auto queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1.0;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    constexpr VkPhysicalDeviceFeatures deviceFeatures{};
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    /// 启用交换链
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();


    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_device, indices.m_graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.m_presentFamily.value(), 0, &m_presentQueue);

#if 0
    const QueueFamilyIndices indices = findQueueFamily(m_physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.m_graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    constexpr float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    constexpr VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }


    vkGetDeviceQueue(m_device, indices.m_graphicsFamily.value(), 0, &m_graphicsQueue);
#endif
}

void HelloTriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void HelloTriangleApplication::createSurface()
{
#if 0
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_KHR;
    createInfo.hwnd = glfwGetWin32Window(m_window);
    createInfo.hinstance = GetModuleHandle(nullptr);

    const auto CreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(m_vkInstance, "vkCreateWin32SurfaceKHR"));

    if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(m_vkInstance,&createInfo,nullptr,&m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface");
    }
#endif

    if (glfwCreateWindowSurface(m_vkInstance, m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface");
    }
}

bool HelloTriangleApplication::checkDeviceExtensionSupport(VkPhysicalDevice device) const
{
    assert(this);
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    /// 确保我们所需要的扩展能被当前设备支持
    std::set<std::string> requireExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requireExtensions.erase(extension.extensionName);
    }

    return requireExtensions.empty();
}

SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device) const
{
    assert(this);
    SwapChainSupportDetails details;

    ///获取表面属性 
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.m_capabilities);

    /// 表面支持的格式
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.m_format.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.m_format.data());
    }

    /// 可用的呈现模式
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.m_presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.m_presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    assert(this);
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        /// use B G R A 
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& availableFormat : availableFormats)
    {
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && 
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

auto HelloTriangleApplication::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes) -> VkPresentModeKHR
{
    assert(this);

    auto bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for(const auto & presentMode: availablePresentModes)
    {
        if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR)  ///< 三缓冲
        {
            return presentMode;
        }

        if(presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) ///< 立即显示
        {
            bestMode = presentMode;
        }
    }

    return bestMode;
}

VkExtent2D HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
#undef max
#undef min
    assert(this);

    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent = { WIDTH,HEIGHT };
    actualExtent.width = std::max(capabilities.minImageExtent.width,
    std::min(capabilities.maxImageExtent.width, actualExtent.width));

    actualExtent.width = std::max(capabilities.minImageExtent.height,
        std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;

}

auto HelloTriangleApplication::createSwapChain() -> void
{
    assert(this);
    const auto swapChainSupport = querySwapChainSupport(m_physicalDevice);

    const auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.m_format);
    const auto presentMode = chooseSwapPresentMode(swapChainSupport.m_presentModes);
    const auto extent = chooseSwapExtent(swapChainSupport.m_capabilities);

    auto imageCount = swapChainSupport.m_capabilities.minImageCount + 1;

    if(swapChainSupport.m_capabilities.maxImageCount > 0 && 
        imageCount > swapChainSupport.m_capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.m_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent; createInfo.imageArrayLayers = 1; ///< 指定图像所包含的层次
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  ///< 指定在图像上的操作

    const auto indices = findQueueFamily(m_physicalDevice);

    const uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(indices.m_graphicsFamily.value()),
    static_cast<uint32_t>(indices.m_presentFamily.value()) };

    if(indices.m_graphicsFamily.value() != indices.m_presentFamily.value())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.m_capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; ///< 窗口混合 此设置为忽略

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(m_device,&createInfo,nullptr,&m_swapChain)!= VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain");
    }

    /// 获取交换链图像的数量
    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());

    /// 记录交换链图像格式与图像范围
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}


bool HelloTriangleApplication::isDeviceSuitable(VkPhysicalDevice device) const
{
    assert(this);

#if 0
    // 获取设备的属性 name type support VulKan versions
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // 纹理压缩 64位浮点 多视口渲染支持查询
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
#endif

    const auto indices = findQueueFamily(device);
    const auto extensionSupport = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;

    if (extensionSupport)
    {
        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.m_format.empty() && !swapChainSupport.m_presentModes.empty();
    }

    return indices.isComplete() && extensionSupport && swapChainAdequate;
}

int HelloTriangleApplication::rateDeviceSuitability(VkPhysicalDevice device) const
{
    assert(this);
    // 获取设备的属性 name type support VulKan versions
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // 纹理压缩 64位浮点 多视口渲染支持查询
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    auto score = 0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

    if (!deviceFeatures.geometryShader)
    {
        return 0;
    }

    return score;
}
