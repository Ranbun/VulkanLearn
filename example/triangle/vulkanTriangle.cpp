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
#include <limits>
#include <set>
#include <GLFW/glfw3native.h>

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

auto HelloTriangleApplication::run() -> void
{
    initWindow();
    initVulKan();
    mainLoop();
    cleanup();
}

auto HelloTriangleApplication::initWindow() -> void
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); ///< make GLFW don't create it(OpenGL Context)
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); ///< no resizing

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "VulKan Window", nullptr, nullptr);
}

auto HelloTriangleApplication::initVulKan() -> void
{
    /**
     * @brief 创建VulKan实例
     */
    createInstance();

    /**
     * @brief 设置调试信息
     */
    setupDebugMessenger();

    /**
     * @brief 创建绘制的表面
     */
    createSurface();

    /**
     * @brief 选择物理设备
     */
    pickPhysicalDevice();

    /**
     * @brief 创建逻辑设备
     */
    createLogicDevice();

    /// 创建交换链
    createSwapChain();

    /// 创建图片视图
    createImageViews();

    /// 创建渲染流程
    createRenderPass();

    /// 创建图像管线
    createGraphicsPipeline();

    /**
     * @brief 创建帧缓冲
     */
    createFramebuffers();

    /**
     * @brief 创建指令池
     */
     createCommandPool();

     /**
      * @brief 创建指令缓冲
      */
     createCommandBuffers();

     /**
      * @brief 创建同步使用的对象  - 信号量与栅栏
      */
     createSyncObjects();
}

auto HelloTriangleApplication::setupDebugMessenger() -> void
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


auto HelloTriangleApplication::mainLoop() -> void
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        drawFrame();
    }

    /**
     * @brief 等待一个特定指令队列结束执行
    */
    vkDeviceWaitIdle(m_logicDevice);

}

auto HelloTriangleApplication::cleanup() -> void
{
    /**
     * @brief 清空创建的信号量 & 栅栏
    */
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(m_logicDevice,
            m_renderFinishedSemaphore[i], nullptr);
        vkDestroySemaphore(m_logicDevice,
            m_imageAvailableSemaphore[i], nullptr);
        vkDestroyFence(m_logicDevice, m_inFlightFence[i], nullptr);
    }

    /**
     * @brief 清空指令池对象
     */
    vkDestroyCommandPool(m_logicDevice,m_commandPool,nullptr);

    /**
     * @brief 清除帧缓冲对象
     */
    for(auto framebuffer: m_swapChainFramebuffers)
    {
        vkDestroyFramebuffer(m_logicDevice,framebuffer,nullptr);
    }

    /**
     * @brief 删除管线
     */
    vkDestroyPipeline(m_logicDevice, m_graphicsPipeline,nullptr);

    /**
     * @brief 清空管线布局
     */
    vkDestroyPipelineLayout(m_logicDevice, m_pipelineLayout, nullptr);

    /**
     * @brief 清空渲染流程
     */
    vkDestroyRenderPass(m_logicDevice, m_renderPass, nullptr);

    for (const auto& imageView : m_swapChainImagesViews)
    {
        vkDestroyImageView(m_logicDevice, imageView, nullptr);
    }

    /// 删除交换链
    vkDestroySwapchainKHR(m_logicDevice, m_swapChain, nullptr);

    /// 逻辑设备
    vkDestroyDevice(m_logicDevice, nullptr);

    /// delete Vk Instance
    /// clean others objects before VK Instance
    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(m_vkInstance, m_callBack, nullptr);
    }

    /// <summary>
    /// 删除SurfaceKHR对象，需要在在实例被清除之前完成此操作
    /// </summary>
    vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);

    vkDestroyInstance(m_vkInstance, nullptr);

    /**
     * @brief 销毁窗口资源
     */
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
}

auto HelloTriangleApplication::createInstance() -> void
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

auto HelloTriangleApplication::checkValidationLayerSupport() -> bool
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

auto HelloTriangleApplication::getRequireExtensions() const -> std::vector<const char *>
{
    assert(this);
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        /// 添加扩展 获得检验层的调试信息
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return std::move(extensions); // NOLINT(clang-diagnostic-pessimizing-move)
}

auto HelloTriangleApplication::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator,
                                                            VkDebugUtilsMessengerEXT* pCallback) const -> VkResult
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

auto HelloTriangleApplication::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback,
                                                             const VkAllocationCallbacks* pAllocator) const -> void
{
    assert(this);

    const auto p_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (p_func != nullptr)
    {
        p_func(instance, callback, pAllocator);
    }
}

auto HelloTriangleApplication::pickPhysicalDevice() -> void
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

auto HelloTriangleApplication::findQueueFamily(VkPhysicalDevice device) const -> QueueFamilyIndices
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

auto HelloTriangleApplication::createLogicDevice() -> void
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


    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_logicDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_logicDevice, indices.m_graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicDevice, indices.m_presentFamily.value(), 0, &m_presentQueue);
}

auto HelloTriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) -> void
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

auto HelloTriangleApplication::createSurface() -> void
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

auto HelloTriangleApplication::checkDeviceExtensionSupport(VkPhysicalDevice device) const -> bool
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

auto HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device) const -> SwapChainSupportDetails
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

auto HelloTriangleApplication::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) -> VkSurfaceFormatKHR
{
    assert(this);
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        /// use B G R A
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
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

    for (const auto& presentMode : availablePresentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) ///< 三缓冲
        {
            return presentMode;
        }

        if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) ///< 立即显示
        {
            bestMode = presentMode;
        }
    }

    return bestMode;
}

auto HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) -> VkExtent2D
{
#undef max
#undef min
    assert(this);

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent = {WIDTH, HEIGHT};
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

    if (swapChainSupport.m_capabilities.maxImageCount > 0 &&
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
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; ///< 指定图像所包含的层次
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; ///< 指定在图像上的操作

    const auto indices = findQueueFamily(m_physicalDevice);

    const uint32_t queueFamilyIndices[] = {
        static_cast<uint32_t>(indices.m_graphicsFamily.value()),
        static_cast<uint32_t>(indices.m_presentFamily.value())
    };

    if (indices.m_graphicsFamily.value() != indices.m_presentFamily.value())
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

    if (vkCreateSwapchainKHR(m_logicDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain");
    }

    /// 获取交换链图像的数量
    vkGetSwapchainImagesKHR(m_logicDevice, m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_logicDevice, m_swapChain, &imageCount, m_swapChainImages.data());

    /// 记录交换链图像格式与图像范围
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

auto HelloTriangleApplication::createImageViews() -> void
{
    assert(this);
    m_swapChainImagesViews.resize(m_swapChainImages.size());

    for (auto i = 0; i < m_swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_logicDevice, &createInfo, nullptr, &m_swapChainImagesViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image view!");
        }
    }
}

auto HelloTriangleApplication::createGraphicsPipeline() -> void
{
    /// 读取着色器文件
    const auto vertShaderCode = ToolSets::readFile("./sources/shaders/vert.spv");
    const auto fragShaderCode = ToolSets::readFile("./sources/shaders/frag.spv");

    const auto vertShaderModule = createShaderModule(vertShaderCode);
    const auto fragShaderModule = createShaderModule(fragShaderCode);

    /// 顶点着色器阶段 - 设置
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.pSpecializationInfo = nullptr;

    /**
     * @brief 片段着色器设置
     */
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.pSpecializationInfo = nullptr;

    /**
     * @brief 组装为数组
     */
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    /**
     * @brief 顶点输入
     */
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    /**
     * @brief 绑定
     */
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    /**
     * @brief 属性
     */
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    /// 输入装配
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    /// 视口和裁剪矩形
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    /// 视口和裁剪
    VkViewport viewport = {};
    {
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapChainExtent.width);
        viewport.height = static_cast<float>(m_swapChainExtent.height);

        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
    }

    viewportState.pViewports = &viewport;

    /**
     * @brief 设置裁剪
     */
    VkRect2D scissor = {};
    {
        scissor.offset = {0, 0};
        scissor.extent = m_swapChainExtent;
    }
    viewportState.pScissors = &scissor;

    /// 光栅化
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE; ///< 禁止所有图元输出到帧缓冲
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    /**
     * @brief 下面的值将作用在深度上
     * @note 此处我们将之屏蔽
     */
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    /// 多重采样
        /// 此处先禁用多重采样
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    /// 深度与模板测试
    {
        /// 暂时先不进行深度与模板测试的配置
    }

    /**
     * @brief 颜色混合
     * @note VkPipelineColorBlendAttachmentState 可以为每个帧缓冲单独配置颜色混合的方式
     */
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    ///动态修改的管线状态
    std::vector<VkDynamicState> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
#if 0
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
#endif
    dynamicState.pDynamicStates = nullptr;
    dynamicState.dynamicStateCount = 0;

    /// 管线布局
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    /**
     * @brief 创建管线布局
     */
    if (vkCreatePipelineLayout(m_logicDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    /**
     * @brief 创建渲染管线
     * @note 引用固定功能 管线布局 渲染流程
     */
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    /**
     * @brief 引用创建的着色器阶段
     */
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    /**
     * @brief 顶点输入
     */
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    /**
     * @brief 输入装配
     */
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    /**
     * @brief 视口
     */
    pipelineInfo.pViewportState = &viewportState;
    /**
     * @brief 光栅化
     */
    pipelineInfo.pRasterizationState = & rasterizer;
    /**
     * @brief 采样
     */
    pipelineInfo.pMultisampleState = &multisampling;
    /**
     * @brief 深度与裁剪
     */
    pipelineInfo.pDepthStencilState = nullptr;
    /**
     * @brief 颜色混合
     */
    pipelineInfo.pColorBlendState = &colorBlending;
    /**
     * @brief 动态更改
     */
    pipelineInfo.pDynamicState = nullptr;

    pipelineInfo.layout = m_pipelineLayout;

    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;  ///< 使用的子流程 在子流程数组中的索引
    /**
     * @brief 指定已经创建好的管线作为基础管线
     */
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    /**
     * @brief 指定将要创建的管线作为基础管线
     */
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(m_logicDevice,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&m_graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    /**
     * @brief 清除创建的着色器模块
     */
    vkDestroyShaderModule(m_logicDevice, vertShaderModule, nullptr);
    vkDestroyShaderModule(m_logicDevice, fragShaderModule, nullptr);
}

auto HelloTriangleApplication::createShaderModule(const std::vector<char>& code) const -> VkShaderModule
{
    VkShaderModuleCreateInfo createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_logicDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}

auto HelloTriangleApplication::createRenderPass() -> void
{
    assert(this);

    /// 附着描述
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    /// 渲染前后的处理 - 颜色和深度处理
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    /// 模板缓冲的处理
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    /// 图像布局 - 纹理的处理
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;  ///< 流程开始前的图像的布局
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; ///< 流程结束后的图形的布局

    /**
     * @brief 颜色附着
     */
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0; ///< 只有一个附着 所以索引为 0
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    /**
     * @brief 子流程
     */
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    /**
     * @brief 渲染流程开始前的子流程的操作
    */
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if(vkCreateRenderPass(m_logicDevice,&renderPassInfo,nullptr,&m_renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }

}

auto HelloTriangleApplication::isDeviceSuitable(VkPhysicalDevice device) const -> bool
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

    auto swapChainAdequate = false;

    if (extensionSupport)
    {
        const auto swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.m_format.empty() && !swapChainSupport.m_presentModes.empty();
    }

    return indices.isComplete() && extensionSupport && swapChainAdequate;
}

[[maybe_unused]] auto HelloTriangleApplication::rateDeviceSuitability(VkPhysicalDevice device) const -> int
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

void HelloTriangleApplication::createFramebuffers()
{
    m_swapChainFramebuffers.resize(m_swapChainImagesViews.size());

    for(size_t i = 0; i < m_swapChainImagesViews.size(); i++)
    {
        VkImageView attachments[] = {m_swapChainImagesViews[i]};

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

        /**
         * @brief 指定渲染流程对象
         */

        framebufferInfo.renderPass = m_renderPass;
        /**
         * @brief 指定附着个数
         */
        framebufferInfo.attachmentCount = 1;

        /**
         * @brief 附着数组
         */
        framebufferInfo.pAttachments = attachments;

        /**
         * @brief 帧缓冲大小
         */
        framebufferInfo.width = m_swapChainExtent.width;
        framebufferInfo.height = m_swapChainExtent.height;

        /**
         * @brief 图层数
         */
        framebufferInfo.layers = 1;

        /**
         * @brief 创建图层数
         */
        if(vkCreateFramebuffer(m_logicDevice,&framebufferInfo, nullptr,&m_swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
void HelloTriangleApplication::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamily(m_physicalDevice);

    VkCommandPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    /// TODO 此处未判断查找结果是否满足要求
    poolCreateInfo.queueFamilyIndex = queueFamilyIndices.m_graphicsFamily.value();
    poolCreateInfo.flags = 0;

    if(vkCreateCommandPool(m_logicDevice,&poolCreateInfo,nullptr,&m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

void HelloTriangleApplication::createCommandBuffers()
{
    /**
     * @brief 为每一个帧缓冲创建一个指令缓冲
     */
    m_commandBuffers.resize(m_swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    if (vkAllocateCommandBuffers(m_logicDevice, &allocateInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

#if 0
    for (size_t i = 0; i < m_commandBuffers.size(); i++)
    {
        /**
         * @brief 记录指令到指令缓冲
        */
        VkCommandBufferBeginInfo beginInfo = {};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr;  ///< 辅助指令缓冲

        /**
         * @brief 开始指令缓冲的记录操作
         */
        if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        /**
         * @brief 开始渲染流程
        */
        VkRenderPassBeginInfo renderpassInfo = {};
        renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderpassInfo.renderPass = m_renderPass;
        renderpassInfo.framebuffer = m_swapChainFramebuffers[i];  ///< 指定使用的渲染流程对象
        renderpassInfo.renderArea.offset = { 0,0 };
        renderpassInfo.renderArea.extent = m_swapChainExtent;

        VkClearValue clearColor = { 0.0f,0.0f,0.0f ,0.0f };
        renderpassInfo.clearValueCount = 1;
        renderpassInfo.pClearValues = &clearColor;

        /**
         * @brief 开始渲染流程的命令
         */
        vkCmdBeginRenderPass(m_commandBuffers[i],&renderpassInfo,VK_SUBPASS_CONTENTS_INLINE);

        /**
         * @brief 基础绘制命令
         */
         /**
          * @brief 绑定图形管线
          */
        vkCmdBindPipeline(m_commandBuffers[i],VK_PIPELINE_BIND_POINT_GRAPHICS,m_graphicsPipeline);

        /**
         * @brief 指令调用三角形的绘制
         */
        vkCmdDraw(m_commandBuffers[i],3,1,0,0);

        /**
         * @brief 结束渲染流程
         */
        vkCmdEndRenderPass(m_commandBuffers[i]);

        if(vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed t record command buffer!");
        }
    }
#endif
}
void HelloTriangleApplication::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
        /**
         * @brief 记录指令到指令缓冲
        */
        VkCommandBufferBeginInfo beginInfo = {};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr;///< 辅助指令缓冲

        /**
         * @brief 开始指令缓冲的记录操作
         */
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        /**
         * @brief 开始渲染流程
        */
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];///< 指定使用的渲染流程对象
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_swapChainExtent;

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        /**
         * @brief 开始渲染流程的命令
         */
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        /**
         * @brief 基础绘制命令
         */
        /**
          * @brief 绑定图形管线
          */
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

        /**
         * @brief 视口
         */
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) m_swapChainExtent.width;
        viewport.height = (float) m_swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        /**
         * @brief 裁剪
         */
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        /**
         * @brief 指令调用三角形的绘制
         */
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        /**
         * @brief 结束渲染流程
         */
        vkCmdEndRenderPass(commandBuffer);

        /**
         * @brief 结束命令记录
         */
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed t record command buffer!");
        }
}
void HelloTriangleApplication::drawFrame()
{
    vkWaitForFences(m_logicDevice, 1,& m_inFlightFence[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    vkResetFences(m_logicDevice, 1, &m_inFlightFence[m_currentFrame]);

    /// 从交换链获取一张图像
    /// 对帧缓冲附着执行指令缓冲中的渲染指令
    /// 返回渲染后的图像到交换链进行呈现操作

    // 栅栏 and 信号量： 使用栅栏(fence) 来对应用程序本身和渲
    //染操作进行同步。使用信号量(semaphore) 来对一个指令队列内的操作或
    //多个不同指令队列的操作进行同步。

    /**
     * @brief 获取图像
     */
    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_logicDevice,m_swapChain,std::numeric_limits<uint64_t>::max(),m_imageAvailableSemaphore[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
    vkQueueWaitIdle(m_presentQueue);

    recordCommandBuffer(m_commandBuffers[imageIndex], imageIndex);

    /**
     * @brief 提交指令缓冲
     */
      VkSubmitInfo submitInfo = {};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

      VkSemaphore waitSemaphore[] = {m_imageAvailableSemaphore[m_currentFrame]};

      VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = waitSemaphore;
      submitInfo.pWaitDstStageMask = waitStages;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];

      VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphore[m_currentFrame]};
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = signalSemaphores;

      if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence[m_currentFrame]) != VK_SUCCESS)
      {
          throw std::runtime_error("failed to submit draw commandbuffer!");
      }

      VkPresentInfoKHR presentInfo = {};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = signalSemaphores;

      VkSwapchainKHR swapChains[] = { m_swapChain };

      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = swapChains;
      presentInfo.pImageIndices = &imageIndex;
      presentInfo.pResults = nullptr;

      /**
       * @brief 请求交换链呈现图像
      */
      vkQueuePresentKHR(m_presentQueue, &presentInfo);

      m_currentFrame = (m_currentFrame + 1) %
          MAX_FRAMES_IN_FLIGHT;
}

void HelloTriangleApplication::createSyncObjects()
{
    m_imageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFence.resize(MAX_FRAMES_IN_FLIGHT);


    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceinfo = {};
    fenceinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if(vkCreateSemaphore(m_logicDevice,&semaphoreInfo,nullptr,&m_imageAvailableSemaphore[i]) != VK_SUCCESS ||
           vkCreateSemaphore(m_logicDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(m_logicDevice,&fenceinfo,nullptr,&m_inFlightFence[i] ) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization!");
        }
    }

}

HelloTriangleApplication::HelloTriangleApplication() = default;
