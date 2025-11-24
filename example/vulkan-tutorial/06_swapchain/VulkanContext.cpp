#include "VulkanContext.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>

#include "Application.h"
#include "VulkanFeatureManager.h"
#include "VulkanUtils.h"

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


VulkanContext::VulkanContext(const VulkanFeatureManager &feature) :
    m_instance(VK_NULL_HANDLE), m_featureManager(feature)
{
    init();
}

VulkanContext::~VulkanContext()
{
    cleanup();
}

void VulkanContext::cleanup()
{
    m_vulkanDebugger.reset();

    if (m_swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_logicDevice, m_swapChain, nullptr);
    }

    if (m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }

    if (m_logicDevice != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_logicDevice, nullptr);
    }

    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_instance, nullptr);
    }
};

void VulkanContext::init()
{
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicDevice();
    createSwapChain();
};

void VulkanContext::createInstance()
{
    /// 检测验证层
    auto &validationLayers = m_featureManager.getValidationLayers();
    bool enablevalidationLayers = !validationLayers.empty();

    if (enablevalidationLayers && !m_featureManager.checkValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
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

    if (!m_featureManager.checkInstanceExtensionSupport())
    {
        throw std::runtime_error("some extensions requested, but not available!");
    }

    const auto &instanceExtensions = m_featureManager.getEnabledInstanceExtensions();
    createInfo.enabledExtensionCount = instanceExtensions.size();
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (enablevalidationLayers)
    {
        const auto &layers = m_featureManager.getValidationLayers();
        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();

        VulkanDebugger::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    if (auto res = vkCreateInstance(&createInfo, nullptr, &m_instance); res != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    std::cout << "Create VkInstance Success!" << std::endl;
}

void VulkanContext::setupDebugMessenger()
{
    if (const bool enableValidationLayer = m_featureManager.getValidationLayers().empty(); enableValidationLayer)
    {
        m_vulkanDebugger = std::make_unique<VulkanDebugger>(m_instance);
    }
}

void VulkanContext::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to found Gpus with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, physicalDevices.data());

    auto findQueueFamilies = [&](const VkPhysicalDevice &device) -> QueueFamilyIndices
    {
        QueueFamilyIndices indices;

        /// 获取队列族
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        if (queueFamilyCount == 0)
        {
            throw std::runtime_error("Can not found QueueFamily!.");
        }

        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (auto i = 0; i < queueFamilyCount; i++)
        {
            auto &queueFamily = queueFamilies.at(i);
            VkBool32 presentSupport = false;

            /// 检测当前设备是否可以在特定的surface上渲染显示结果 -- 查找可以用于呈现的队列族索引
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
            if (presentSupport)
            {
                indices.presentFamily = i;
            }

            /// 如果支持图形队列 -- 查找可以用于渲染的队列族索引
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            /// others check.....

            if (indices.isComplete())
            {
                break;
            }
        }

        return indices;
    };

    auto querySwapChainSupportDetails = [&](const VkPhysicalDevice &device) -> SwapChainSupportDetails
    {
        SwapChainSupportDetails details;

        /// 查询 某个设备对于特定surface的能力
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount,
                                                      details.presentModes.data());
        }

        return details;
    };

    auto isDeviceSuitable = [&findQueueFamilies, querySwapChainSupportDetails, this](const VkPhysicalDevice &device)
    {
        /// 获取属性
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::cout << "find physical device: " << deviceProperties.deviceName << std::endl;

        const auto queueFamily = findQueueFamilies(device);

        const auto extensionSupported = m_featureManager.validateDeviceSupport(device); ///< 是否支持请求的那些扩展

        bool swapChainAdequate = false;
        if (extensionSupported)
        {
            const auto swapChainDetails = querySwapChainSupportDetails(device);
            swapChainAdequate = !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
        }

        return queueFamily.isComplete() && extensionSupported && swapChainAdequate;
    };

    for (auto &device: physicalDevices)
    {
        if (isDeviceSuitable(device)) /// select the first avaliable device
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    findQueueFamiliesFunc = std::move(findQueueFamilies);
    querySwapChainSupportDetailsFunc = std::move(querySwapChainSupportDetails);
};

void VulkanContext::createLogicDevice()
{
    VkDeviceCreateInfo deviceCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                        .pNext = nullptr,
                                        .flags = 0,
                                        .queueCreateInfoCount = 0,
                                        .pQueueCreateInfos = nullptr,
                                        .enabledLayerCount = 0,
                                        .ppEnabledLayerNames = nullptr,
                                        .enabledExtensionCount = 0,
                                        .ppEnabledExtensionNames = nullptr,
                                        .pEnabledFeatures = nullptr};

    /// find queue family : create queue
    auto queueFamily = findQueueFamiliesFunc(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> queueFamilies{queueFamily.graphicsFamily.value(), queueFamily.presentFamily.value()};

    float queuePriorities = 1.0f; /// 设置队列的优先级 [0.0f - 1.0f]
    for (auto queueFamily: queueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                .pNext = nullptr,
                                                .flags = 0,
                                                .queueFamilyIndex = queueFamily,
                                                .queueCount = 1,
                                                .pQueuePriorities = &queuePriorities};
        queueCreateInfos.emplace_back(queueCreateInfo);
    }


    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();


    /// 指定设备启用的特性
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    /// 设备扩展
    auto requiredExtensions = m_featureManager.getEnabledDeviceExtensions();
    deviceCreateInfo.enabledExtensionCount = requiredExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (auto res = vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_logicDevice); res != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logic device.");
    }

    vkGetDeviceQueue(m_logicDevice, queueFamily.presentFamily.value(), 0, &m_presentQueue);

    /// 之前的Vulkan分为实例扩展和设备扩展，如果在版本更老Vulkan版本开发，请在创建Device的时候指定验证层和消息传递的扩展

    std::cout << "Create VkDevice Success!" << std::endl;
};

void VulkanContext::createSurface()
{
    auto &app = Application::Instance();
    auto *window = static_cast<GLFWwindow *>(app.RenderWindow()->getNativeWindow());

    if (auto res = glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface); res != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    if (m_surface == VK_NULL_HANDLE)
    {
        throw std::runtime_error("must create VkSurface!");
    }

    std::cout << "Create VkSurface Success!" << std::endl;
}
void VulkanContext::createSwapChain()
{
    SwapChainSupportDetails details = querySwapChainSupportDetailsFunc(m_physicalDevice);

    using namespace VulkanUtils;
    auto extent = chooseSwapChainExtent(details.capabilities);
    auto surfaceFormat = chooseSwapChainSurfaceFormat(details.formats);
    auto presentMode = chooseSwapChainPresentMode(details.presentModes);

    /// 交换链需要多少张图像
    /// 仅仅满足这个最低要求意味着我们有时可能需要等待驱动程序完成内部操作才能获取另一张用于渲染的图像。因此，建议请求的图像数量至少比最低要求多一张
    auto imageCount = details.capabilities.minImageCount + 1;
    /// 图像的数量不能超过最大图像数量
    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
    {
        imageCount = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo{.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                                 .pNext = nullptr,
                                                 .flags = 0,
                                                 .surface = m_surface,
                                                 .minImageCount = imageCount,
                                                 .imageFormat = surfaceFormat.format,
                                                 .imageColorSpace = surfaceFormat.colorSpace,
                                                 .imageExtent = extent,
                                                 .imageArrayLayers = 1,
                                                 .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                 .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, /// 不在队列之间共享
                                                 .queueFamilyIndexCount = 0,
                                                 .pQueueFamilyIndices = nullptr,
                                                 .preTransform = details.capabilities.currentTransform,
                                                 .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                                 .presentMode = presentMode,
                                                 .clipped = VK_TRUE,
                                                 .oldSwapchain = VK_NULL_HANDLE};

    auto queueFamily = findQueueFamiliesFunc(m_physicalDevice);
    std::vector<uint32_t> queueFamilyIndices{queueFamily.graphicsFamily.value(), queueFamily.presentFamily.value()};
    std::set<uint32_t> indices{queueFamily.graphicsFamily.value(), queueFamily.presentFamily.value()};
    if (indices.size() == 2)
    {
        /// 表示两个队列 不相同 需要在两个队列之间共享
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    if (queueFamilyIndices[0] != queueFamilyIndices[1]) /// 渲染队列和显示队列不是同一个
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
        /// imageSharingMode 表示交换链图像是否需要在不同的队列共享 -- 这个由渲染队列和表示队列是否是一个来决定
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    if (auto res = vkCreateSwapchainKHR(m_logicDevice, &swapChainCreateInfo, nullptr, &m_swapChain); res != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swapChain!");
    }

    std::cout << "Create SwapChain Success!" << std::endl;

    /// get swapchain image
    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(m_logicDevice, m_swapChain, &swapChainImageCount, nullptr);
    m_swapChainImages.resize(swapChainImageCount);
    vkGetSwapchainImagesKHR(m_logicDevice, m_swapChain, &swapChainImageCount, m_swapChainImages.data());

    /// restore format & extent
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}
