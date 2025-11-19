#include "VulkanContext.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>
#include <stdexcept>

#include "Application.h"

#define USE_LINUX 0
#define USE_WINDOW 0
#define USE_GLFW 1

#define ENABLE_X11 0
#define ENABLE_XCB 0

VulkanContext::VulkanContext(const std::vector<const char *> &requiredExtensions) : m_instance(VK_NULL_HANDLE)
{
    init(requiredExtensions);
}

VulkanContext::~VulkanContext() { cleanup(); }

void VulkanContext::cleanup()
{
    m_vulkanDebugger.reset();

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

void VulkanContext::init(const std::vector<const char *> &requiredExtensions)
{
    createInstance(requiredExtensions);
    if (VulkanDebugger::enableValidationLayers())
    {
        setupDebugMessenger();
    }
    createSurface();

    pickPhysicalDevice();
    createLogicDevice();
};

void VulkanContext::createInstance(const std::vector<const char *> &requiredExtensions)
{
    if (VulkanDebugger::enableValidationLayers() && !(VulkanDebugger::checkValidationLayerSupport()))
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

    auto layerExtensions = getVulkanLayersExtensions();

    std::vector<const char *> allRequirementExtensions{requiredExtensions};
    allRequirementExtensions.insert(allRequirementExtensions.end(), layerExtensions.begin(), layerExtensions.end());
    if (!checkRequirementsExtensionSupport(allRequirementExtensions))
    {
        throw std::runtime_error("Missing required Vulkan instance extensions");
    }

    createInfo.enabledExtensionCount = allRequirementExtensions.size();
    createInfo.ppEnabledExtensionNames = allRequirementExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (VulkanDebugger::enableValidationLayers())
    {
        const auto &layers = VulkanDebugger::getRequiredLayers();

        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();

        VulkanDebugger::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    auto res = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

bool VulkanContext::checkRequirementsExtensionSupport(const std::vector<const char *> &requiredExtensions)
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

void VulkanContext::setupDebugMessenger() { m_vulkanDebugger = std::make_unique<VulkanDebugger>(m_instance); }

std::vector<const char *> VulkanContext::getVulkanLayersExtensions()
{
    std::vector<const char *> extensions;
    const auto &debugRequirementExtension = VulkanDebugger::getRequiredExtensionName();
    extensions.insert(extensions.end(), debugRequirementExtension.begin(),
                      debugRequirementExtension.end()); /// requirement message callback
    return std::move(extensions);
};

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

    auto findQueueFamilies = [&](VkPhysicalDevice &device) -> QueueFamilyIndices
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

            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

            /// 如果支持图形队列
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            if (presentSupport)
            {
                indices.presentFamily = i;
            }

            /// others check.....
            if (indices.isComplete())
            {
                break;
            }
        }

        return indices;
    };

    auto isDeviceSuitable = [&findQueueFamilies](VkPhysicalDevice &device)
    {
        auto queueFamily = findQueueFamilies(device);

        /// 获取属性
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        std::cout << "find physical device: " << deviceProperties.deviceName << std::endl;

        /// 获取支持特性
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
        return deviceFeatures.geometryShader & queueFamily.isComplete();
    };

    for (auto &device: physicalDevices)
    {
        if (isDeviceSuitable(device))
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

    /// create queue
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

    if (auto res = vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_logicDevice); res != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logic device.");
    }

    vkGetDeviceQueue(m_logicDevice, queueFamily.presentFamily.value(), 0, &m_presentQueue);

    /// 之前的Vulkan分为实例扩展和设备扩展，如果在版本更老Vulkan版本开发，请在创建Device的时候指定验证层和消息传递的扩展
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
}
