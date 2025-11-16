#include "VulkanContext.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

VulkanContext::VulkanContext(const std::vector<const char *> &requiredExtensions) : m_instance(VK_NULL_HANDLE)
{
    init(requiredExtensions);
}

VulkanContext::~VulkanContext() { cleanup(); }

void VulkanContext::cleanup()
{
    m_vulkanDebugger.reset();
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
